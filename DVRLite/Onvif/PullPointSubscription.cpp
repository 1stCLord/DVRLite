#include "PullPointSubscription.h"
#include "gsoap-onvif/wsaapi.h"
#include "gsoap-onvif/wsseapi.h"
#include  <openssl/rsa.h>
#include "gsoap-onvif/soapPullPointSubscriptionBindingProxy.h"
#include "gsoap-onvif/soapDeviceBindingProxy.h"
#include "gsoap-onvif/soapMediaBindingProxy.h"
#include "gsoap-onvif/DeviceBinding.nsmap"
#include "DVRLite.h"

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

		pullpointSubscriptionBindingProxy = std::unique_ptr<PullPointSubscriptionBindingProxy>(new PullPointSubscriptionBindingProxy(capabilitiesResponse.Capabilities->Events->XAddr.c_str()));
		soap_wsse_add_UsernameTokenDigest(pullpointSubscriptionBindingProxy->soap, NULL, username.c_str(), password.c_str());
		soap_register_plugin(pullpointSubscriptionBindingProxy->soap, soap_wsse);

		soap_wsse_add_Timestamp(pullpointSubscriptionBindingProxy->soap, "Time", 10);
		soap_register_plugin(pullpointSubscriptionBindingProxy->soap, soap_wsa);

		soap_wsse_add_UsernameTokenDigest(pullpointSubscriptionBindingProxy->soap, NULL, username.c_str(), password.c_str());
		_tev__CreatePullPointSubscription createPullPointSubscription;
		std::string initialTerminationTime = "PT60S";
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

void PullPointSubscription::PullMessages()
{
	if (pullpointSubscriptionBindingProxy)
	{
		//DVRLite::Log("PullMessages " + source.GetName());
		std::string username = source.GetUsername();
		std::string password = source.GetPassword();

		soap_wsse_add_UsernameTokenDigest(pullpointSubscriptionBindingProxy->soap, NULL, username.c_str(), password.c_str());
		_tev__PullMessages pullMessages;
		pullMessages.Timeout = "PT10S";
		pullMessages.MessageLimit = 1;
		char* const pullpoint_ptr = pullpoint.data();
		soap_wsa_request(pullpointSubscriptionBindingProxy->soap, nullptr, pullpoint.c_str(), "PullMessages");
		_tev__PullMessagesResponse pullMessagesResponse;
		pullpointSubscriptionBindingProxy->PullMessages(pullpoint.c_str(), nullptr, &pullMessages, pullMessagesResponse);
		if (!pullMessagesResponse.wsnt__NotificationMessage.empty())
			alert();
	}
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
		PullMessages();
	}
	Uninit();
}
