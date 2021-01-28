#include "PullPointSubscription.h"
#include "gsoap-onvif/wsaapi.h"
#include "gsoap-onvif/wsseapi.h"
#include  <openssl/rsa.h>
#include "gsoap-onvif/soapPullPointSubscriptionBindingProxy.h"
#include "gsoap-onvif/soapDeviceBindingProxy.h"
#include "gsoap-onvif/soapMediaBindingProxy.h"
#include "gsoap-onvif/DeviceBinding.nsmap"
#include "DVRLite.h"

#define PULL_DURATION "PT5S"
#define SUBSCRIPTION_DURATION "PT5M"
#define SECONDS_TO_RENEW 10

PullPointSubscription::PullPointSubscription(Source& source, std::function<void(void)> alert) :
	running(true),
	alert(alert),
	source(source)
{
	thread = std::thread(&PullPointSubscription::Run,this);
}

PullPointSubscription::~PullPointSubscription()
{
	running = false;
	if (thread.joinable())
		thread.join();
}

std::string PullPointSubscription::Init() 
{
	DVRLite::Log("Initialise PullPointSubscription " + source.GetName());

	std::string endpoint = source.GetOnvifAddress();
	std::string username = source.GetUsername();
	std::string password = source.GetPassword();
	DeviceBindingProxy deviceBinding = DeviceBindingProxy(endpoint.c_str());
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
		DVRLite::Log("PullPointSubscription Got Caps " + source.GetName());
		MediaBindingProxy mediaBinding = MediaBindingProxy(capabilitiesResponse.Capabilities->Media->XAddr.c_str());
		soap_wsse_add_UsernameTokenDigest(mediaBinding.soap, NULL, username.c_str(), password.c_str());
		soap_register_plugin(mediaBinding.soap, soap_wsse);
		_trt__GetStreamUri getStreamUri;
		_trt__GetStreamUriResponse getStreamUriResponse;
		mediaBinding.GetStreamUri(&getStreamUri, getStreamUriResponse);
		if(getStreamUriResponse.MediaUri)
			source.SetOnvifVideoAddress(getStreamUriResponse.MediaUri->Uri);



		soap_wsse_add_UsernameTokenDigest(mediaBinding.soap, NULL, username.c_str(), password.c_str());
		_trt__GetSnapshotUri getSnapshotUri;
		_trt__GetSnapshotUriResponse getSnapshotUriResponse;
		mediaBinding.GetSnapshotUri(&getSnapshotUri, getSnapshotUriResponse);
		if (getSnapshotUriResponse.MediaUri)
			source.SetOnvifSnapshotAddress(getSnapshotUriResponse.MediaUri->Uri);

		pullpointSubscriptionBindingProxy = std::unique_ptr<PullPointSubscriptionBindingProxy>(new PullPointSubscriptionBindingProxy(capabilitiesResponse.Capabilities->Events->XAddr.c_str()));
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
			DVRLite::Log("Initialise PullPointSubscription Got Subscription Address " + source.GetName() + " " + pullpoint);
			return pullpoint;
		}
	}
	return std::string();
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
		if (!pullMessagesResponse.wsnt__NotificationMessage.empty())
			alert();

		if (pullMessagesResponse.soap->error != SOAP_OK && pullMessagesResponse.soap->error != SOAP_MUSTUNDERSTAND)
		{
			DVRLite::Log("Subscription Expired " + source.GetName());
			return false;
		}
		else if (pullMessagesResponse.soap->error == SOAP_OK && (pullMessagesResponse.TerminationTime - pullMessagesResponse.CurrentTime) < SECONDS_TO_RENEW)
		{
			Renew();
		}

		return true;
	}
	return false;
}

void PullPointSubscription::Renew()
{
	DVRLite::Log("Renew subscription " + source.GetName());

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
	DVRLite::Log("Uninitialise PullPointSubscription " + source.GetName());
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
			pullpoint = Init();
		if (!PullMessages())pullpoint.clear();
	}
	Uninit();
}
