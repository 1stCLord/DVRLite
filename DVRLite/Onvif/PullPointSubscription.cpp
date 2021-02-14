#include "PullPointSubscription.h"
#include "gsoap-onvif/wsaapi.h"
#include "gsoap-onvif/wsseapi.h"
#include  <openssl/rsa.h>
#include "gsoap-onvif/soapPullPointSubscriptionBindingProxy.h"
#include "gsoap-onvif/soapDeviceBindingProxy.h"
#include "gsoap-onvif/soapMediaBindingProxy.h"
#include "gsoap-onvif/DeviceBinding.nsmap"
#include "DVRLite.h"

#define PULL_DURATION "PT1S"
#ifdef _DEBUG
#define SUBSCRIPTION_DURATION "PT30S"
#define SECONDS_TO_RENEW 20
#else
#define SUBSCRIPTION_DURATION "PT10M"
#define SECONDS_TO_RENEW 30
#endif

namespace DVRLite
{

	PullPointSubscription::PullPointSubscription(Source& source, std::function<void(const std::string&)> alert) :
		running(true),
		alert(alert),
		source(source)
	{
		thread = std::thread(&PullPointSubscription::Run, this);
	}

	PullPointSubscription::~PullPointSubscription()
	{
		running = false;
		if (thread.joinable())
			thread.join();
	}

	std::string PullPointSubscription::Init()
	{
		Log(filter, "Initialise PullPointSubscription " + source.GetName());

		std::string endpoint = source.GetOnvifAddress();
		std::string username = source.GetUsername();
		std::string password = source.GetPassword();
		DeviceBindingProxy deviceBinding = DeviceBindingProxy(endpoint.c_str());
		PullPointSubscription::SetTimeouts(deviceBinding.soap);
		soap_register_plugin(deviceBinding.soap, soap_wsse);

		soap_wsse_add_Timestamp(deviceBinding.soap, "Time", 10);

		soap_wsse_add_UsernameTokenDigest(deviceBinding.soap, NULL, username.c_str(), password.c_str());
		_tds__GetCapabilities capabilities;
		_tds__GetCapabilitiesResponse capabilitiesResponse;
		deviceBinding.GetCapabilities(&capabilities, capabilitiesResponse);

		soap_wsse_add_UsernameTokenDigest(deviceBinding.soap, NULL, username.c_str(), password.c_str());
		_tds__GetDeviceInformation deviceInformation;
		_tds__GetDeviceInformationResponse deviceInformationResponse;
		deviceBinding.GetDeviceInformation(&deviceInformation, deviceInformationResponse);

		if (capabilitiesResponse.Capabilities)
		{
			Log(filter, "PullPointSubscription Got Caps " + source.GetName());
			MediaBindingProxy mediaBinding = MediaBindingProxy(capabilitiesResponse.Capabilities->Media->XAddr.c_str());
			PullPointSubscription::SetTimeouts(mediaBinding.soap);
			soap_wsse_add_UsernameTokenDigest(mediaBinding.soap, NULL, username.c_str(), password.c_str());
			soap_register_plugin(mediaBinding.soap, soap_wsse);
			_trt__GetStreamUri getStreamUri;
			_trt__GetStreamUriResponse getStreamUriResponse;
			mediaBinding.GetStreamUri(&getStreamUri, getStreamUriResponse);
			if (getStreamUriResponse.MediaUri)
			{
				Log(filter, "PullPointSubscription Got StreamUri " + source.GetName() + " " + getStreamUriResponse.MediaUri->Uri);
				source.SetOnvifVideoAddress(getStreamUriResponse.MediaUri->Uri);
			}

			soap_wsse_add_UsernameTokenDigest(mediaBinding.soap, NULL, username.c_str(), password.c_str());
			_trt__GetSnapshotUri getSnapshotUri;
			_trt__GetSnapshotUriResponse getSnapshotUriResponse;
			mediaBinding.GetSnapshotUri(&getSnapshotUri, getSnapshotUriResponse);
			if (getSnapshotUriResponse.MediaUri)
			{
				Log(filter, "PullPointSubscription Got SnapshotUri " + source.GetName() + " " + getSnapshotUriResponse.MediaUri->Uri);
				source.SetOnvifSnapshotAddress(getSnapshotUriResponse.MediaUri->Uri);
			}

			pullpointSubscriptionBindingProxy = std::unique_ptr<PullPointSubscriptionBindingProxy>(new PullPointSubscriptionBindingProxy(capabilitiesResponse.Capabilities->Events->XAddr.c_str()));
			PullPointSubscription::SetTimeouts(pullpointSubscriptionBindingProxy->soap);
			soap_wsse_add_UsernameTokenDigest(pullpointSubscriptionBindingProxy->soap, NULL, username.c_str(), password.c_str());
			soap_register_plugin(pullpointSubscriptionBindingProxy->soap, soap_wsse);

			soap_wsse_add_Timestamp(pullpointSubscriptionBindingProxy->soap, "Time", 10);
			soap_register_plugin(pullpointSubscriptionBindingProxy->soap, soap_wsa);

			soap_wsse_add_UsernameTokenDigest(pullpointSubscriptionBindingProxy->soap, NULL, username.c_str(), password.c_str());
			_tev__CreatePullPointSubscription createPullPointSubscription;
			std::string initialTerminationTime = SUBSCRIPTION_DURATION;
			createPullPointSubscription.InitialTerminationTime = &initialTerminationTime;
			_tev__CreatePullPointSubscriptionResponse createPullPointSubscriptionResponse;
			pullpointSubscriptionBindingProxy->CreatePullPointSubscription(&createPullPointSubscription, createPullPointSubscriptionResponse);
			if (createPullPointSubscriptionResponse.SubscriptionReference.__size > 0)
			{
				const int prefixlen = strlen("<wsa5:Address>");
				int len = strlen(createPullPointSubscriptionResponse.SubscriptionReference.__any[0]) - ((2 * prefixlen) + 1);
				std::string pullpoint(createPullPointSubscriptionResponse.SubscriptionReference.__any[0] + prefixlen, len);
				Log(filter, "Initialise PullPointSubscription Got Subscription Address " + source.GetName() + " " + pullpoint);
				return pullpoint;
			}
		}
		Log(filter, "Initialise PullPointSubscription Failed to get Subscription Address " + source.GetName());
		return std::string();
	}

	const std::string dataTag = "<tt:Data>";
	const std::string simpleItemOpen = "<tt:SimpleItem ";
	std::string PullPointSubscription::ManualParseMessage(const std::string& buffer)
	{
		size_t begin = buffer.find(dataTag) + dataTag.length();
		if (begin == std::string::npos)return "";
		begin = buffer.find(simpleItemOpen, begin) + simpleItemOpen.length();
		if (begin == std::string::npos)return "";
		size_t end = buffer.find("/>", begin);
		if (end == std::string::npos)return "";
		return buffer.substr(begin, end - begin);
	}

	void PullPointSubscription::SetTimeouts(soap* soap)
	{
		soap->connect_timeout = 10;
		soap->connect_retry = 1;
		soap->send_timeout = 5;
		soap->recv_timeout = 5;
		soap->transfer_timeout = 10;
	}

	bool PullPointSubscription::PullMessages()
	{
		if (pullpointSubscriptionBindingProxy)
		{
			std::string username = source.GetUsername();
			std::string password = source.GetPassword();

			soap_wsse_add_UsernameTokenDigest(pullpointSubscriptionBindingProxy->soap, NULL, username.c_str(), password.c_str());
			_tev__PullMessages pullMessages;
			pullMessages.Timeout = PULL_DURATION;
			pullMessages.MessageLimit = 1;
			char* const pullpoint_ptr = pullpoint.data();
			soap_wsa_request(pullpointSubscriptionBindingProxy->soap, nullptr, pullpoint.c_str(), "PullMessages");
			_tev__PullMessagesResponse pullMessagesResponse;
			pullpointSubscriptionBindingProxy->PullMessages(pullpoint.c_str(), nullptr, &pullMessages, pullMessagesResponse);
			for (wsnt__NotificationMessageHolderType* notification : pullMessagesResponse.wsnt__NotificationMessage)
			{
				std::string buffer = std::string(notification->soap->buf, notification->soap->buflen);
				alert(ManualParseMessage(buffer));
			}

			if (pullMessagesResponse.soap != nullptr)
			{
				if (pullMessagesResponse.soap->error != SOAP_OK && pullMessagesResponse.soap->error != SOAP_MUSTUNDERSTAND)
				{
					Log(filter, "Subscription Expired " + source.GetName());
					return false;
				}
				else if (pullMessagesResponse.soap->error == SOAP_OK && (pullMessagesResponse.TerminationTime - pullMessagesResponse.CurrentTime) < SECONDS_TO_RENEW)
				{
					Renew();
				}
			}
			return true;
		}
		return false;
	}

	void PullPointSubscription::Renew()
	{
		Log(filter, "Renew subscription " + source.GetName());

		std::string username = source.GetUsername();
		std::string password = source.GetPassword();
		soap_wsse_add_UsernameTokenDigest(pullpointSubscriptionBindingProxy->soap, NULL, username.c_str(), password.c_str());

		_wsnt__Renew renew;
		std::string terminationTime = SUBSCRIPTION_DURATION;
		renew.TerminationTime = &terminationTime;
		soap_wsa_request(pullpointSubscriptionBindingProxy->soap, nullptr, pullpoint.c_str(), "Renew");
		_wsnt__RenewResponse renewResponse;
		pullpointSubscriptionBindingProxy->Renew(pullpoint.c_str(), nullptr, &renew, renewResponse);
	}

	void PullPointSubscription::Uninit()
	{
		Log(filter, "Uninitialise PullPointSubscription " + source.GetName());
		std::string username = source.GetUsername();
		std::string password = source.GetPassword();

		soap_wsse_add_UsernameTokenDigest(pullpointSubscriptionBindingProxy->soap, NULL, username.c_str(), password.c_str());
		_wsnt__Unsubscribe unsubscribe;
		_wsnt__UnsubscribeResponse unsubscribeResponse;
		pullpointSubscriptionBindingProxy->Unsubscribe(pullpoint.c_str(), nullptr, &unsubscribe, unsubscribeResponse);
		pullpointSubscriptionBindingProxy.reset();
	}

	void PullPointSubscription::Run()
	{
		while (running)
		{
			if (pullpoint.empty())
			{
				std::this_thread::sleep_for(std::chrono::seconds(5));
				pullpoint = Init();
			}
			if (!pullpoint.empty())
			{
				if (!PullMessages())
					pullpoint.clear();
			}
		}
		Uninit();
	}
}