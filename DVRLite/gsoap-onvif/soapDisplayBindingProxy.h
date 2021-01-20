/* soapDisplayBindingProxy.h
   Generated by gSOAP 2.8.109 for onvif.h

gSOAP XML Web services tools
Copyright (C) 2000-2020, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#ifndef soapDisplayBindingProxy_H
#define soapDisplayBindingProxy_H
#include "soapH.h"

    class SOAP_CMAC DisplayBindingProxy {
      public:
        /// Context to manage proxy IO and data
        struct soap *soap;
        /// flag indicating that this context is owned by this proxy and should be deleted by the destructor
        bool soap_own;
        /// Endpoint URL of service 'DisplayBindingProxy' (change as needed)
        const char *soap_endpoint;
        /// Variables globally declared in onvif.h, if any
        /// Construct a proxy with new managing context
        DisplayBindingProxy();
        /// Copy constructor
        DisplayBindingProxy(const DisplayBindingProxy& rhs);
        /// Construct proxy given a shared managing context
        DisplayBindingProxy(struct soap*);
        /// Construct proxy given a shared managing context and endpoint URL
        DisplayBindingProxy(struct soap*, const char *soap_endpoint_url);
        /// Constructor taking an endpoint URL
        DisplayBindingProxy(const char *soap_endpoint_url);
        /// Constructor taking input and output mode flags for the new managing context
        DisplayBindingProxy(soap_mode iomode);
        /// Constructor taking endpoint URL and input and output mode flags for the new managing context
        DisplayBindingProxy(const char *soap_endpoint_url, soap_mode iomode);
        /// Constructor taking input and output mode flags for the new managing context
        DisplayBindingProxy(soap_mode imode, soap_mode omode);
        /// Destructor deletes deserialized data and its managing context, when the context was allocated by the constructor
        virtual ~DisplayBindingProxy();
        /// Initializer used by constructors
        virtual void DisplayBindingProxy_init(soap_mode imode, soap_mode omode);
        /// Return a copy that has a new managing context with the same engine state
        virtual DisplayBindingProxy *copy();
        /// Copy assignment
        DisplayBindingProxy& operator=(const DisplayBindingProxy&);
        /// Delete all deserialized data (uses soap_destroy() and soap_end())
        virtual void destroy();
        /// Delete all deserialized data and reset to default
        virtual void reset();
        /// Disables and removes SOAP Header from message by setting soap->header = NULL
        virtual void soap_noheader();
        /// Add SOAP Header to message
        virtual void soap_header(struct _wsse__Security *wsse__Security, char *wsa__MessageID, struct wsa__Relationship *wsa__RelatesTo, struct wsa__EndpointReferenceType *wsa__From, struct wsa__EndpointReferenceType *wsa__ReplyTo, struct wsa__EndpointReferenceType *wsa__FaultTo, char *wsa__To, char *wsa__Action, struct wsdd__AppSequenceType *wsdd__AppSequence, char *wsa5__MessageID, struct wsa5__RelatesToType *wsa5__RelatesTo, struct wsa5__EndpointReferenceType *wsa5__From, struct wsa5__EndpointReferenceType *wsa5__ReplyTo, struct wsa5__EndpointReferenceType *wsa5__FaultTo, char *wsa5__To, char *wsa5__Action, struct chan__ChannelInstanceType *chan__ChannelInstance);
        /// Get SOAP Header structure (i.e. soap->header, which is NULL when absent)
        virtual ::SOAP_ENV__Header *soap_header();
        /// Get SOAP Fault structure (i.e. soap->fault, which is NULL when absent)
        virtual ::SOAP_ENV__Fault *soap_fault();
        /// Get SOAP Fault subcode QName string (NULL when absent)
        virtual const char *soap_fault_subcode();
        /// Get SOAP Fault string/reason (NULL when absent)
        virtual const char *soap_fault_string();
        /// Get SOAP Fault detail XML string (NULL when absent)
        virtual const char *soap_fault_detail();
        /// Close connection (normally automatic, except for send_X ops)
        virtual int soap_close_socket();
        /// Force close connection (can kill a thread blocked on IO)
        virtual int soap_force_close_socket();
        /// Print fault
        virtual void soap_print_fault(FILE*);
    #ifndef WITH_LEAN
    #ifndef WITH_COMPAT
        /// Print fault to stream
        virtual void soap_stream_fault(std::ostream&);
    #endif
        /// Write fault to buffer
        virtual char *soap_sprint_fault(char *buf, size_t len);
    #endif
        //
        /// Web service synchronous operation 'GetServiceCapabilities' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int GetServiceCapabilities(_tls__GetServiceCapabilities *tls__GetServiceCapabilities, _tls__GetServiceCapabilitiesResponse &tls__GetServiceCapabilitiesResponse) { return this->GetServiceCapabilities(NULL, NULL, tls__GetServiceCapabilities, tls__GetServiceCapabilitiesResponse); }
        /// Web service synchronous operation 'GetServiceCapabilities' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int GetServiceCapabilities(const char *soap_endpoint_url, const char *soap_action, _tls__GetServiceCapabilities *tls__GetServiceCapabilities, _tls__GetServiceCapabilitiesResponse &tls__GetServiceCapabilitiesResponse) { return this->send_GetServiceCapabilities(soap_endpoint_url, soap_action, tls__GetServiceCapabilities) || this->recv_GetServiceCapabilities(tls__GetServiceCapabilitiesResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_GetServiceCapabilities' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_GetServiceCapabilities(const char *soap_endpoint_url, const char *soap_action, _tls__GetServiceCapabilities *tls__GetServiceCapabilities);
        /// Web service asynchronous operation 'recv_GetServiceCapabilities' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_GetServiceCapabilities(_tls__GetServiceCapabilitiesResponse &tls__GetServiceCapabilitiesResponse);
        //
        /// Web service synchronous operation 'GetLayout' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int GetLayout(_tls__GetLayout *tls__GetLayout, _tls__GetLayoutResponse &tls__GetLayoutResponse) { return this->GetLayout(NULL, NULL, tls__GetLayout, tls__GetLayoutResponse); }
        /// Web service synchronous operation 'GetLayout' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int GetLayout(const char *soap_endpoint_url, const char *soap_action, _tls__GetLayout *tls__GetLayout, _tls__GetLayoutResponse &tls__GetLayoutResponse) { return this->send_GetLayout(soap_endpoint_url, soap_action, tls__GetLayout) || this->recv_GetLayout(tls__GetLayoutResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_GetLayout' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_GetLayout(const char *soap_endpoint_url, const char *soap_action, _tls__GetLayout *tls__GetLayout);
        /// Web service asynchronous operation 'recv_GetLayout' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_GetLayout(_tls__GetLayoutResponse &tls__GetLayoutResponse);
        //
        /// Web service synchronous operation 'SetLayout' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int SetLayout(_tls__SetLayout *tls__SetLayout, _tls__SetLayoutResponse &tls__SetLayoutResponse) { return this->SetLayout(NULL, NULL, tls__SetLayout, tls__SetLayoutResponse); }
        /// Web service synchronous operation 'SetLayout' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int SetLayout(const char *soap_endpoint_url, const char *soap_action, _tls__SetLayout *tls__SetLayout, _tls__SetLayoutResponse &tls__SetLayoutResponse) { return this->send_SetLayout(soap_endpoint_url, soap_action, tls__SetLayout) || this->recv_SetLayout(tls__SetLayoutResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_SetLayout' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_SetLayout(const char *soap_endpoint_url, const char *soap_action, _tls__SetLayout *tls__SetLayout);
        /// Web service asynchronous operation 'recv_SetLayout' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_SetLayout(_tls__SetLayoutResponse &tls__SetLayoutResponse);
        //
        /// Web service synchronous operation 'GetDisplayOptions' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int GetDisplayOptions(_tls__GetDisplayOptions *tls__GetDisplayOptions, _tls__GetDisplayOptionsResponse &tls__GetDisplayOptionsResponse) { return this->GetDisplayOptions(NULL, NULL, tls__GetDisplayOptions, tls__GetDisplayOptionsResponse); }
        /// Web service synchronous operation 'GetDisplayOptions' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int GetDisplayOptions(const char *soap_endpoint_url, const char *soap_action, _tls__GetDisplayOptions *tls__GetDisplayOptions, _tls__GetDisplayOptionsResponse &tls__GetDisplayOptionsResponse) { return this->send_GetDisplayOptions(soap_endpoint_url, soap_action, tls__GetDisplayOptions) || this->recv_GetDisplayOptions(tls__GetDisplayOptionsResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_GetDisplayOptions' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_GetDisplayOptions(const char *soap_endpoint_url, const char *soap_action, _tls__GetDisplayOptions *tls__GetDisplayOptions);
        /// Web service asynchronous operation 'recv_GetDisplayOptions' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_GetDisplayOptions(_tls__GetDisplayOptionsResponse &tls__GetDisplayOptionsResponse);
        //
        /// Web service synchronous operation 'GetPaneConfigurations' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int GetPaneConfigurations(_tls__GetPaneConfigurations *tls__GetPaneConfigurations, _tls__GetPaneConfigurationsResponse &tls__GetPaneConfigurationsResponse) { return this->GetPaneConfigurations(NULL, NULL, tls__GetPaneConfigurations, tls__GetPaneConfigurationsResponse); }
        /// Web service synchronous operation 'GetPaneConfigurations' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int GetPaneConfigurations(const char *soap_endpoint_url, const char *soap_action, _tls__GetPaneConfigurations *tls__GetPaneConfigurations, _tls__GetPaneConfigurationsResponse &tls__GetPaneConfigurationsResponse) { return this->send_GetPaneConfigurations(soap_endpoint_url, soap_action, tls__GetPaneConfigurations) || this->recv_GetPaneConfigurations(tls__GetPaneConfigurationsResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_GetPaneConfigurations' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_GetPaneConfigurations(const char *soap_endpoint_url, const char *soap_action, _tls__GetPaneConfigurations *tls__GetPaneConfigurations);
        /// Web service asynchronous operation 'recv_GetPaneConfigurations' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_GetPaneConfigurations(_tls__GetPaneConfigurationsResponse &tls__GetPaneConfigurationsResponse);
        //
        /// Web service synchronous operation 'GetPaneConfiguration' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int GetPaneConfiguration(_tls__GetPaneConfiguration *tls__GetPaneConfiguration, _tls__GetPaneConfigurationResponse &tls__GetPaneConfigurationResponse) { return this->GetPaneConfiguration(NULL, NULL, tls__GetPaneConfiguration, tls__GetPaneConfigurationResponse); }
        /// Web service synchronous operation 'GetPaneConfiguration' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int GetPaneConfiguration(const char *soap_endpoint_url, const char *soap_action, _tls__GetPaneConfiguration *tls__GetPaneConfiguration, _tls__GetPaneConfigurationResponse &tls__GetPaneConfigurationResponse) { return this->send_GetPaneConfiguration(soap_endpoint_url, soap_action, tls__GetPaneConfiguration) || this->recv_GetPaneConfiguration(tls__GetPaneConfigurationResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_GetPaneConfiguration' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_GetPaneConfiguration(const char *soap_endpoint_url, const char *soap_action, _tls__GetPaneConfiguration *tls__GetPaneConfiguration);
        /// Web service asynchronous operation 'recv_GetPaneConfiguration' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_GetPaneConfiguration(_tls__GetPaneConfigurationResponse &tls__GetPaneConfigurationResponse);
        //
        /// Web service synchronous operation 'SetPaneConfigurations' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int SetPaneConfigurations(_tls__SetPaneConfigurations *tls__SetPaneConfigurations, _tls__SetPaneConfigurationsResponse &tls__SetPaneConfigurationsResponse) { return this->SetPaneConfigurations(NULL, NULL, tls__SetPaneConfigurations, tls__SetPaneConfigurationsResponse); }
        /// Web service synchronous operation 'SetPaneConfigurations' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int SetPaneConfigurations(const char *soap_endpoint_url, const char *soap_action, _tls__SetPaneConfigurations *tls__SetPaneConfigurations, _tls__SetPaneConfigurationsResponse &tls__SetPaneConfigurationsResponse) { return this->send_SetPaneConfigurations(soap_endpoint_url, soap_action, tls__SetPaneConfigurations) || this->recv_SetPaneConfigurations(tls__SetPaneConfigurationsResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_SetPaneConfigurations' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_SetPaneConfigurations(const char *soap_endpoint_url, const char *soap_action, _tls__SetPaneConfigurations *tls__SetPaneConfigurations);
        /// Web service asynchronous operation 'recv_SetPaneConfigurations' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_SetPaneConfigurations(_tls__SetPaneConfigurationsResponse &tls__SetPaneConfigurationsResponse);
        //
        /// Web service synchronous operation 'SetPaneConfiguration' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int SetPaneConfiguration(_tls__SetPaneConfiguration *tls__SetPaneConfiguration, _tls__SetPaneConfigurationResponse &tls__SetPaneConfigurationResponse) { return this->SetPaneConfiguration(NULL, NULL, tls__SetPaneConfiguration, tls__SetPaneConfigurationResponse); }
        /// Web service synchronous operation 'SetPaneConfiguration' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int SetPaneConfiguration(const char *soap_endpoint_url, const char *soap_action, _tls__SetPaneConfiguration *tls__SetPaneConfiguration, _tls__SetPaneConfigurationResponse &tls__SetPaneConfigurationResponse) { return this->send_SetPaneConfiguration(soap_endpoint_url, soap_action, tls__SetPaneConfiguration) || this->recv_SetPaneConfiguration(tls__SetPaneConfigurationResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_SetPaneConfiguration' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_SetPaneConfiguration(const char *soap_endpoint_url, const char *soap_action, _tls__SetPaneConfiguration *tls__SetPaneConfiguration);
        /// Web service asynchronous operation 'recv_SetPaneConfiguration' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_SetPaneConfiguration(_tls__SetPaneConfigurationResponse &tls__SetPaneConfigurationResponse);
        //
        /// Web service synchronous operation 'CreatePaneConfiguration' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int CreatePaneConfiguration(_tls__CreatePaneConfiguration *tls__CreatePaneConfiguration, _tls__CreatePaneConfigurationResponse &tls__CreatePaneConfigurationResponse) { return this->CreatePaneConfiguration(NULL, NULL, tls__CreatePaneConfiguration, tls__CreatePaneConfigurationResponse); }
        /// Web service synchronous operation 'CreatePaneConfiguration' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int CreatePaneConfiguration(const char *soap_endpoint_url, const char *soap_action, _tls__CreatePaneConfiguration *tls__CreatePaneConfiguration, _tls__CreatePaneConfigurationResponse &tls__CreatePaneConfigurationResponse) { return this->send_CreatePaneConfiguration(soap_endpoint_url, soap_action, tls__CreatePaneConfiguration) || this->recv_CreatePaneConfiguration(tls__CreatePaneConfigurationResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_CreatePaneConfiguration' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_CreatePaneConfiguration(const char *soap_endpoint_url, const char *soap_action, _tls__CreatePaneConfiguration *tls__CreatePaneConfiguration);
        /// Web service asynchronous operation 'recv_CreatePaneConfiguration' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_CreatePaneConfiguration(_tls__CreatePaneConfigurationResponse &tls__CreatePaneConfigurationResponse);
        //
        /// Web service synchronous operation 'DeletePaneConfiguration' with default endpoint and default SOAP Action header, returns SOAP_OK or error code
        virtual int DeletePaneConfiguration(_tls__DeletePaneConfiguration *tls__DeletePaneConfiguration, _tls__DeletePaneConfigurationResponse &tls__DeletePaneConfigurationResponse) { return this->DeletePaneConfiguration(NULL, NULL, tls__DeletePaneConfiguration, tls__DeletePaneConfigurationResponse); }
        /// Web service synchronous operation 'DeletePaneConfiguration' to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int DeletePaneConfiguration(const char *soap_endpoint_url, const char *soap_action, _tls__DeletePaneConfiguration *tls__DeletePaneConfiguration, _tls__DeletePaneConfigurationResponse &tls__DeletePaneConfigurationResponse) { return this->send_DeletePaneConfiguration(soap_endpoint_url, soap_action, tls__DeletePaneConfiguration) || this->recv_DeletePaneConfiguration(tls__DeletePaneConfigurationResponse) ? this->soap->error : SOAP_OK; }
        /// Web service asynchronous operation 'send_DeletePaneConfiguration' to send a request message to the specified endpoint and SOAP Action header, returns SOAP_OK or error code
        virtual int send_DeletePaneConfiguration(const char *soap_endpoint_url, const char *soap_action, _tls__DeletePaneConfiguration *tls__DeletePaneConfiguration);
        /// Web service asynchronous operation 'recv_DeletePaneConfiguration' to receive a response message from the connected endpoint, returns SOAP_OK or error code
        virtual int recv_DeletePaneConfiguration(_tls__DeletePaneConfigurationResponse &tls__DeletePaneConfigurationResponse);
    };
#endif
