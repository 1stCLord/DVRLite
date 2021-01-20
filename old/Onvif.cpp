#include "Onvif.h"
#include "gsoap-onvif/wsaapi.h"
#include "gsoap-onvif/wsseapi.h"
#include  <openssl/rsa.h>
#include "gsoap-onvif/soapDeviceBindingProxy.h"
#include "gsoap-onvif/soapPullPointSubscriptionBindingProxy.h"
#include "gsoap-onvif/DeviceBinding.nsmap"

#include <iostream>
#include <string>

#include "DVRLite.h"

Onvif::Onvif(DVRLite *dvrlite) : dvrlite(dvrlite)
{
	DeviceBindingProxy deviceBinding = DeviceBindingProxy("http://192.168.0.75:8000/onvif/device_service");
	soap_register_plugin(deviceBinding.soap, soap_wsse);

	soap_wsse_add_UsernameTokenDigest(deviceBinding.soap, NULL, "admin", "gibraltar");
	soap_wsse_add_Timestamp(deviceBinding.soap, "Time", 10);

	soap_wsse_add_UsernameTokenDigest(deviceBinding.soap, NULL, "admin", "gibraltar");
	_tds__GetCapabilities capabilities;
	_tds__GetCapabilitiesResponse capabilitiesResponse;
	deviceBinding.GetCapabilities(&capabilities, capabilitiesResponse);

	soap_wsse_add_UsernameTokenDigest(deviceBinding.soap, NULL, "admin", "gibraltar");
	_tds__GetDeviceInformation deviceInformation;
	_tds__GetDeviceInformationResponse deviceInformationResponse;
	deviceBinding.GetDeviceInformation(&deviceInformation, deviceInformationResponse);


	PullPointSubscriptionBindingProxy ppsbp = PullPointSubscriptionBindingProxy(capabilitiesResponse.Capabilities->Events->XAddr.c_str());
	soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
	soap_register_plugin(ppsbp.soap, soap_wsse);

	soap_wsse_add_Timestamp(ppsbp.soap, "Time", 10);
	soap_register_plugin(ppsbp.soap, soap_wsa);

	/*_tev__GetEventProperties tev__GetEventProperties;
	_tev__GetEventPropertiesResponse tev__GetEventPropertiesResponse;
	soap_wsa_request(ppsbp.soap, NULL, NULL, "http://www.onvif.org/ver10/events/wsdl/EventPortType/GetEventPropertiesRequest");
	ppsbp.GetEventProperties(&tev__GetEventProperties, tev__GetEventPropertiesResponse);*/

	/*soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
	_wsnt__Subscribe subscribe;
	_wsnt__SubscribeResponse subscribeResponse;
	ppsbp.Subscribe(&subscribe, subscribeResponse);

	soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
	_wsnt__GetMessages getMessages;
	_wsnt__GetMessagesResponse getMessagesResponse;
	ppsbp.GetMessages(&getMessages, getMessagesResponse);
	soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
	_wsnt__GetCurrentMessage getCurrentMessage;
	_wsnt__GetCurrentMessageResponse getCurrentMessageResponse;
	ppsbp.GetCurrentMessage(&getCurrentMessage, getCurrentMessageResponse);
	soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
	_tev__PullMessages pullMessages;
	_tev__PullMessagesResponse pullMessagesResponse;
	ppsbp.PullMessages(&pullMessages, pullMessagesResponse);

	soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
	_wsnt__Unsubscribe unsubscribe;
	_wsnt__UnsubscribeResponse unsubscribeResponse;
	ppsbp.Unsubscribe(&unsubscribe, unsubscribeResponse);*/

	soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
	_tev__CreatePullPointSubscription createPullPointSubscription;
	std::string initialTerminationTime = "PT60S";
	createPullPointSubscription.InitialTerminationTime = &initialTerminationTime;
	_tev__CreatePullPointSubscriptionResponse createPullPointSubscriptionResponse;
	ppsbp.CreatePullPointSubscription(&createPullPointSubscription, createPullPointSubscriptionResponse);
	if (createPullPointSubscriptionResponse.SubscriptionReference.__size > 0)
	{
		const int prefixlen = strlen("<wsa5:Address>");
		int len = strlen(createPullPointSubscriptionResponse.SubscriptionReference.__any[0]) - ((2 * prefixlen) + 1);
		std::string pullpoint(createPullPointSubscriptionResponse.SubscriptionReference.__any[0] + prefixlen, len);
		const char* replyTo = soap_wsa_ReplyTo(createPullPointSubscriptionResponse.soap);
		/*soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
		_wsnt__GetCurrentMessage getCurrentMessage;
		_wsnt__GetCurrentMessageResponse getCurrentMessageResponse;
		ppsbp.GetCurrentMessage(&getCurrentMessage, getCurrentMessageResponse);*/
		//for (int i = 0; i < 10; i++)
		{
			soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
			_tev__PullMessages pullMessages;
			pullMessages.Timeout = "PT60S";
			pullMessages.MessageLimit = 1;
			char* const pullpoint_ptr = pullpoint.data();
			soap_wsa_request(ppsbp.soap, nullptr, pullpoint.c_str(), "PullMessages");
			_tev__PullMessagesResponse pullMessagesResponse;
			ppsbp.PullMessages(pullpoint.c_str(), nullptr, &pullMessages, pullMessagesResponse);
			if (!pullMessagesResponse.wsnt__NotificationMessage.empty())
				printf("yas!");
		}
		/*soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
		_wsnt__GetMessages getMessages;
		_wsnt__GetMessagesResponse getMessagesResponse;
		ppsbp.GetMessages(&getMessages, getMessagesResponse);*/
		soap_wsse_add_UsernameTokenDigest(ppsbp.soap, NULL, "admin", "gibraltar");
		_wsnt__Unsubscribe unsubscribe;
		_wsnt__UnsubscribeResponse unsubscribeResponse;
		ppsbp.Unsubscribe(pullpoint.c_str(), nullptr, &unsubscribe, unsubscribeResponse);
	}
}

Onvif::~Onvif()
{
    /*if(curl)
    curl_easy_cleanup(curl);*/
}