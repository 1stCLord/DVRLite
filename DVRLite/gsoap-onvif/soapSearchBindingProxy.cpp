/* soapSearchBindingProxy.cpp
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

#include "soapSearchBindingProxy.h"

SearchBindingProxy::SearchBindingProxy()
{	this->soap = soap_new();
	this->soap_own = true;
	SearchBindingProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

SearchBindingProxy::SearchBindingProxy(const SearchBindingProxy& rhs)
{	this->soap = rhs.soap;
	this->soap_own = false;
	this->soap_endpoint = rhs.soap_endpoint;
}

SearchBindingProxy::SearchBindingProxy(struct soap *_soap)
{	this->soap = _soap;
	this->soap_own = false;
	SearchBindingProxy_init(_soap->imode, _soap->omode);
}

SearchBindingProxy::SearchBindingProxy(struct soap *_soap, const char *soap_endpoint_url)
{	this->soap = _soap;
	this->soap_own = false;
	SearchBindingProxy_init(_soap->imode, _soap->omode);
	soap_endpoint = soap_endpoint_url;
}

SearchBindingProxy::SearchBindingProxy(const char *soap_endpoint_url)
{	this->soap = soap_new();
	this->soap_own = true;
	SearchBindingProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
	soap_endpoint = soap_endpoint_url;
}

SearchBindingProxy::SearchBindingProxy(soap_mode iomode)
{	this->soap = soap_new();
	this->soap_own = true;
	SearchBindingProxy_init(iomode, iomode);
}

SearchBindingProxy::SearchBindingProxy(const char *soap_endpoint_url, soap_mode iomode)
{	this->soap = soap_new();
	this->soap_own = true;
	SearchBindingProxy_init(iomode, iomode);
	soap_endpoint = soap_endpoint_url;
}

SearchBindingProxy::SearchBindingProxy(soap_mode imode, soap_mode omode)
{	this->soap = soap_new();
	this->soap_own = true;
	SearchBindingProxy_init(imode, omode);
}

SearchBindingProxy::~SearchBindingProxy()
{	if (this->soap_own)
	{	this->destroy();
		soap_free(this->soap);
	}
}

void SearchBindingProxy::SearchBindingProxy_init(soap_mode imode, soap_mode omode)
{	soap_imode(this->soap, imode);
	soap_omode(this->soap, omode);
	soap_endpoint = NULL;
	static const struct Namespace namespaces[] = {
        { "SOAP-ENV", "http://www.w3.org/2003/05/soap-envelope", "http://schemas.xmlsoap.org/soap/envelope/", NULL },
        { "SOAP-ENC", "http://www.w3.org/2003/05/soap-encoding", "http://schemas.xmlsoap.org/soap/encoding/", NULL },
        { "xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://www.w3.org/*/XMLSchema-instance", NULL },
        { "xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema", NULL },
        { "c14n", "http://www.w3.org/2001/10/xml-exc-c14n#", NULL, NULL },
        { "ds", "http://www.w3.org/2000/09/xmldsig#", NULL, NULL },
        { "saml1", "urn:oasis:names:tc:SAML:1.0:assertion", NULL, NULL },
        { "saml2", "urn:oasis:names:tc:SAML:2.0:assertion", NULL, NULL },
        { "wsu", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd", NULL, NULL },
        { "xenc", "http://www.w3.org/2001/04/xmlenc#", NULL, NULL },
        { "wsc", "http://docs.oasis-open.org/ws-sx/ws-secureconversation/200512", "http://schemas.xmlsoap.org/ws/2005/02/sc", NULL },
        { "wsse", "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd", "http://docs.oasis-open.org/wss/oasis-wss-wssecurity-secext-1.1.xsd", NULL },
        { "wsa", "http://schemas.xmlsoap.org/ws/2004/08/addressing", "http://www.w3.org/2005/08/addressing", NULL },
        { "wsdd", "http://schemas.xmlsoap.org/ws/2005/04/discovery", NULL, NULL },
        { "chan", "http://schemas.microsoft.com/ws/2005/02/duplex", NULL, NULL },
        { "wsa5", "http://www.w3.org/2005/08/addressing", "http://schemas.xmlsoap.org/ws/2004/08/addressing", NULL },
        { "ns1", "http://www.onvif.org/ver20/analytics/humanface", NULL, NULL },
        { "ns2", "http://www.onvif.org/ver20/analytics/humanbody", NULL, NULL },
        { "xmime", "http://tempuri.org/xmime.xsd", NULL, NULL },
        { "xop", "http://www.w3.org/2004/08/xop/include", NULL, NULL },
        { "tt", "http://www.onvif.org/ver10/schema", NULL, NULL },
        { "wsrfbf", "http://docs.oasis-open.org/wsrf/bf-2", NULL, NULL },
        { "wstop", "http://docs.oasis-open.org/wsn/t-1", NULL, NULL },
        { "wsrfr", "http://docs.oasis-open.org/wsrf/r-2", NULL, NULL },
        { "tad", "http://www.onvif.org/ver10/analyticsdevice/wsdl", NULL, NULL },
        { "tan", "http://www.onvif.org/ver20/analytics/wsdl", NULL, NULL },
        { "tdn", "http://www.onvif.org/ver10/network/wsdl", NULL, NULL },
        { "tds", "http://www.onvif.org/ver10/device/wsdl", NULL, NULL },
        { "tev", "http://www.onvif.org/ver10/events/wsdl", NULL, NULL },
        { "wsnt", "http://docs.oasis-open.org/wsn/b-2", NULL, NULL },
        { "timg", "http://www.onvif.org/ver20/imaging/wsdl", NULL, NULL },
        { "tls", "http://www.onvif.org/ver10/display/wsdl", NULL, NULL },
        { "tmd", "http://www.onvif.org/ver10/deviceIO/wsdl", NULL, NULL },
        { "tptz", "http://www.onvif.org/ver20/ptz/wsdl", NULL, NULL },
        { "trc", "http://www.onvif.org/ver10/recording/wsdl", NULL, NULL },
        { "trp", "http://www.onvif.org/ver10/replay/wsdl", NULL, NULL },
        { "trt", "http://www.onvif.org/ver10/media/wsdl", NULL, NULL },
        { "trv", "http://www.onvif.org/ver10/receiver/wsdl", NULL, NULL },
        { "tse", "http://www.onvif.org/ver10/search/wsdl", NULL, NULL },
        { NULL, NULL, NULL, NULL}
    };
	soap_set_namespaces(this->soap, namespaces);
}

SearchBindingProxy *SearchBindingProxy::copy()
{	SearchBindingProxy *dup = SOAP_NEW_UNMANAGED(SearchBindingProxy);
	if (dup)
	{	soap_done(dup->soap);
		soap_copy_context(dup->soap, this->soap);
	}
	return dup;
}

SearchBindingProxy& SearchBindingProxy::operator=(const SearchBindingProxy& rhs)
{	if (this->soap != rhs.soap)
	{	if (this->soap_own)
			soap_free(this->soap);
		this->soap = rhs.soap;
		this->soap_own = false;
		this->soap_endpoint = rhs.soap_endpoint;
	}
	return *this;
}

void SearchBindingProxy::destroy()
{	soap_destroy(this->soap);
	soap_end(this->soap);
}

void SearchBindingProxy::reset()
{	this->destroy();
	soap_done(this->soap);
	soap_initialize(this->soap);
	SearchBindingProxy_init(SOAP_IO_DEFAULT, SOAP_IO_DEFAULT);
}

void SearchBindingProxy::soap_noheader()
{	this->soap->header = NULL;
}

void SearchBindingProxy::soap_header(struct _wsse__Security *wsse__Security, char *wsa__MessageID, struct wsa__Relationship *wsa__RelatesTo, struct wsa__EndpointReferenceType *wsa__From, struct wsa__EndpointReferenceType *wsa__ReplyTo, struct wsa__EndpointReferenceType *wsa__FaultTo, char *wsa__To, char *wsa__Action, struct wsdd__AppSequenceType *wsdd__AppSequence, char *wsa5__MessageID, struct wsa5__RelatesToType *wsa5__RelatesTo, struct wsa5__EndpointReferenceType *wsa5__From, struct wsa5__EndpointReferenceType *wsa5__ReplyTo, struct wsa5__EndpointReferenceType *wsa5__FaultTo, char *wsa5__To, char *wsa5__Action, struct chan__ChannelInstanceType *chan__ChannelInstance)
{
	::soap_header(this->soap);
	this->soap->header->wsse__Security = wsse__Security;
	this->soap->header->wsa__MessageID = wsa__MessageID;
	this->soap->header->wsa__RelatesTo = wsa__RelatesTo;
	this->soap->header->wsa__From = wsa__From;
	this->soap->header->wsa__ReplyTo = wsa__ReplyTo;
	this->soap->header->wsa__FaultTo = wsa__FaultTo;
	this->soap->header->wsa__To = wsa__To;
	this->soap->header->wsa__Action = wsa__Action;
	this->soap->header->wsdd__AppSequence = wsdd__AppSequence;
	this->soap->header->wsa5__MessageID = wsa5__MessageID;
	this->soap->header->wsa5__RelatesTo = wsa5__RelatesTo;
	this->soap->header->wsa5__From = wsa5__From;
	this->soap->header->wsa5__ReplyTo = wsa5__ReplyTo;
	this->soap->header->wsa5__FaultTo = wsa5__FaultTo;
	this->soap->header->wsa5__To = wsa5__To;
	this->soap->header->wsa5__Action = wsa5__Action;
	this->soap->header->chan__ChannelInstance = chan__ChannelInstance;
}

::SOAP_ENV__Header *SearchBindingProxy::soap_header()
{	return this->soap->header;
}

::SOAP_ENV__Fault *SearchBindingProxy::soap_fault()
{	return this->soap->fault;
}

const char *SearchBindingProxy::soap_fault_subcode()
{	return ::soap_fault_subcode(this->soap);
}

const char *SearchBindingProxy::soap_fault_string()
{	return ::soap_fault_string(this->soap);
}

const char *SearchBindingProxy::soap_fault_detail()
{	return ::soap_fault_detail(this->soap);
}

int SearchBindingProxy::soap_close_socket()
{	return ::soap_closesock(this->soap);
}

int SearchBindingProxy::soap_force_close_socket()
{	return ::soap_force_closesock(this->soap);
}

void SearchBindingProxy::soap_print_fault(FILE *fd)
{	::soap_print_fault(this->soap, fd);
}

#ifndef WITH_LEAN
#ifndef WITH_COMPAT
void SearchBindingProxy::soap_stream_fault(std::ostream& os)
{	::soap_stream_fault(this->soap, os);
}
#endif

char *SearchBindingProxy::soap_sprint_fault(char *buf, size_t len)
{	return ::soap_sprint_fault(this->soap, buf, len);
}
#endif

int SearchBindingProxy::send_GetServiceCapabilities(const char *soap_endpoint_url, const char *soap_action, _tse__GetServiceCapabilities *tse__GetServiceCapabilities)
{
	struct __tse__GetServiceCapabilities soap_tmp___tse__GetServiceCapabilities;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/GetServiceCapabilities";
	soap_tmp___tse__GetServiceCapabilities.tse__GetServiceCapabilities = tse__GetServiceCapabilities;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__GetServiceCapabilities(soap, &soap_tmp___tse__GetServiceCapabilities);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__GetServiceCapabilities(soap, &soap_tmp___tse__GetServiceCapabilities, "-tse:GetServiceCapabilities", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__GetServiceCapabilities(soap, &soap_tmp___tse__GetServiceCapabilities, "-tse:GetServiceCapabilities", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_GetServiceCapabilities(_tse__GetServiceCapabilitiesResponse &tse__GetServiceCapabilitiesResponse)
{
	tse__GetServiceCapabilitiesResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__GetServiceCapabilitiesResponse.soap_get(soap, "tse:GetServiceCapabilitiesResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_GetRecordingSummary(const char *soap_endpoint_url, const char *soap_action, _tse__GetRecordingSummary *tse__GetRecordingSummary)
{
	struct __tse__GetRecordingSummary soap_tmp___tse__GetRecordingSummary;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/GetRecordingSummary";
	soap_tmp___tse__GetRecordingSummary.tse__GetRecordingSummary = tse__GetRecordingSummary;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__GetRecordingSummary(soap, &soap_tmp___tse__GetRecordingSummary);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__GetRecordingSummary(soap, &soap_tmp___tse__GetRecordingSummary, "-tse:GetRecordingSummary", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__GetRecordingSummary(soap, &soap_tmp___tse__GetRecordingSummary, "-tse:GetRecordingSummary", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_GetRecordingSummary(_tse__GetRecordingSummaryResponse &tse__GetRecordingSummaryResponse)
{
	tse__GetRecordingSummaryResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__GetRecordingSummaryResponse.soap_get(soap, "tse:GetRecordingSummaryResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_GetRecordingInformation(const char *soap_endpoint_url, const char *soap_action, _tse__GetRecordingInformation *tse__GetRecordingInformation)
{
	struct __tse__GetRecordingInformation soap_tmp___tse__GetRecordingInformation;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/GetRecordingInformation";
	soap_tmp___tse__GetRecordingInformation.tse__GetRecordingInformation = tse__GetRecordingInformation;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__GetRecordingInformation(soap, &soap_tmp___tse__GetRecordingInformation);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__GetRecordingInformation(soap, &soap_tmp___tse__GetRecordingInformation, "-tse:GetRecordingInformation", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__GetRecordingInformation(soap, &soap_tmp___tse__GetRecordingInformation, "-tse:GetRecordingInformation", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_GetRecordingInformation(_tse__GetRecordingInformationResponse &tse__GetRecordingInformationResponse)
{
	tse__GetRecordingInformationResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__GetRecordingInformationResponse.soap_get(soap, "tse:GetRecordingInformationResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_GetMediaAttributes(const char *soap_endpoint_url, const char *soap_action, _tse__GetMediaAttributes *tse__GetMediaAttributes)
{
	struct __tse__GetMediaAttributes soap_tmp___tse__GetMediaAttributes;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/GetMediaAttributes";
	soap_tmp___tse__GetMediaAttributes.tse__GetMediaAttributes = tse__GetMediaAttributes;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__GetMediaAttributes(soap, &soap_tmp___tse__GetMediaAttributes);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__GetMediaAttributes(soap, &soap_tmp___tse__GetMediaAttributes, "-tse:GetMediaAttributes", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__GetMediaAttributes(soap, &soap_tmp___tse__GetMediaAttributes, "-tse:GetMediaAttributes", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_GetMediaAttributes(_tse__GetMediaAttributesResponse &tse__GetMediaAttributesResponse)
{
	tse__GetMediaAttributesResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__GetMediaAttributesResponse.soap_get(soap, "tse:GetMediaAttributesResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_FindRecordings(const char *soap_endpoint_url, const char *soap_action, _tse__FindRecordings *tse__FindRecordings)
{
	struct __tse__FindRecordings soap_tmp___tse__FindRecordings;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/FindRecordings";
	soap_tmp___tse__FindRecordings.tse__FindRecordings = tse__FindRecordings;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__FindRecordings(soap, &soap_tmp___tse__FindRecordings);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__FindRecordings(soap, &soap_tmp___tse__FindRecordings, "-tse:FindRecordings", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__FindRecordings(soap, &soap_tmp___tse__FindRecordings, "-tse:FindRecordings", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_FindRecordings(_tse__FindRecordingsResponse &tse__FindRecordingsResponse)
{
	tse__FindRecordingsResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__FindRecordingsResponse.soap_get(soap, "tse:FindRecordingsResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_GetRecordingSearchResults(const char *soap_endpoint_url, const char *soap_action, _tse__GetRecordingSearchResults *tse__GetRecordingSearchResults)
{
	struct __tse__GetRecordingSearchResults soap_tmp___tse__GetRecordingSearchResults;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/GetRecordingSearchResults";
	soap_tmp___tse__GetRecordingSearchResults.tse__GetRecordingSearchResults = tse__GetRecordingSearchResults;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__GetRecordingSearchResults(soap, &soap_tmp___tse__GetRecordingSearchResults);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__GetRecordingSearchResults(soap, &soap_tmp___tse__GetRecordingSearchResults, "-tse:GetRecordingSearchResults", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__GetRecordingSearchResults(soap, &soap_tmp___tse__GetRecordingSearchResults, "-tse:GetRecordingSearchResults", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_GetRecordingSearchResults(_tse__GetRecordingSearchResultsResponse &tse__GetRecordingSearchResultsResponse)
{
	tse__GetRecordingSearchResultsResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__GetRecordingSearchResultsResponse.soap_get(soap, "tse:GetRecordingSearchResultsResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_FindEvents(const char *soap_endpoint_url, const char *soap_action, _tse__FindEvents *tse__FindEvents)
{
	struct __tse__FindEvents soap_tmp___tse__FindEvents;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/FindEvents";
	soap_tmp___tse__FindEvents.tse__FindEvents = tse__FindEvents;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__FindEvents(soap, &soap_tmp___tse__FindEvents);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__FindEvents(soap, &soap_tmp___tse__FindEvents, "-tse:FindEvents", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__FindEvents(soap, &soap_tmp___tse__FindEvents, "-tse:FindEvents", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_FindEvents(_tse__FindEventsResponse &tse__FindEventsResponse)
{
	tse__FindEventsResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__FindEventsResponse.soap_get(soap, "tse:FindEventsResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_GetEventSearchResults(const char *soap_endpoint_url, const char *soap_action, _tse__GetEventSearchResults *tse__GetEventSearchResults)
{
	struct __tse__GetEventSearchResults soap_tmp___tse__GetEventSearchResults;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/GetEventSearchResults";
	soap_tmp___tse__GetEventSearchResults.tse__GetEventSearchResults = tse__GetEventSearchResults;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__GetEventSearchResults(soap, &soap_tmp___tse__GetEventSearchResults);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__GetEventSearchResults(soap, &soap_tmp___tse__GetEventSearchResults, "-tse:GetEventSearchResults", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__GetEventSearchResults(soap, &soap_tmp___tse__GetEventSearchResults, "-tse:GetEventSearchResults", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_GetEventSearchResults(_tse__GetEventSearchResultsResponse &tse__GetEventSearchResultsResponse)
{
	tse__GetEventSearchResultsResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__GetEventSearchResultsResponse.soap_get(soap, "tse:GetEventSearchResultsResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_FindPTZPosition(const char *soap_endpoint_url, const char *soap_action, _tse__FindPTZPosition *tse__FindPTZPosition)
{
	struct __tse__FindPTZPosition soap_tmp___tse__FindPTZPosition;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/FindPTZPosition";
	soap_tmp___tse__FindPTZPosition.tse__FindPTZPosition = tse__FindPTZPosition;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__FindPTZPosition(soap, &soap_tmp___tse__FindPTZPosition);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__FindPTZPosition(soap, &soap_tmp___tse__FindPTZPosition, "-tse:FindPTZPosition", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__FindPTZPosition(soap, &soap_tmp___tse__FindPTZPosition, "-tse:FindPTZPosition", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_FindPTZPosition(_tse__FindPTZPositionResponse &tse__FindPTZPositionResponse)
{
	tse__FindPTZPositionResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__FindPTZPositionResponse.soap_get(soap, "tse:FindPTZPositionResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_GetPTZPositionSearchResults(const char *soap_endpoint_url, const char *soap_action, _tse__GetPTZPositionSearchResults *tse__GetPTZPositionSearchResults)
{
	struct __tse__GetPTZPositionSearchResults soap_tmp___tse__GetPTZPositionSearchResults;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/GetPTZPositionSearchResults";
	soap_tmp___tse__GetPTZPositionSearchResults.tse__GetPTZPositionSearchResults = tse__GetPTZPositionSearchResults;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__GetPTZPositionSearchResults(soap, &soap_tmp___tse__GetPTZPositionSearchResults);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__GetPTZPositionSearchResults(soap, &soap_tmp___tse__GetPTZPositionSearchResults, "-tse:GetPTZPositionSearchResults", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__GetPTZPositionSearchResults(soap, &soap_tmp___tse__GetPTZPositionSearchResults, "-tse:GetPTZPositionSearchResults", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_GetPTZPositionSearchResults(_tse__GetPTZPositionSearchResultsResponse &tse__GetPTZPositionSearchResultsResponse)
{
	tse__GetPTZPositionSearchResultsResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__GetPTZPositionSearchResultsResponse.soap_get(soap, "tse:GetPTZPositionSearchResultsResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_GetSearchState(const char *soap_endpoint_url, const char *soap_action, _tse__GetSearchState *tse__GetSearchState)
{
	struct __tse__GetSearchState soap_tmp___tse__GetSearchState;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/GetSearchState";
	soap_tmp___tse__GetSearchState.tse__GetSearchState = tse__GetSearchState;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__GetSearchState(soap, &soap_tmp___tse__GetSearchState);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__GetSearchState(soap, &soap_tmp___tse__GetSearchState, "-tse:GetSearchState", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__GetSearchState(soap, &soap_tmp___tse__GetSearchState, "-tse:GetSearchState", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_GetSearchState(_tse__GetSearchStateResponse &tse__GetSearchStateResponse)
{
	tse__GetSearchStateResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__GetSearchStateResponse.soap_get(soap, "tse:GetSearchStateResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_EndSearch(const char *soap_endpoint_url, const char *soap_action, _tse__EndSearch *tse__EndSearch)
{
	struct __tse__EndSearch soap_tmp___tse__EndSearch;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/EndSearch";
	soap_tmp___tse__EndSearch.tse__EndSearch = tse__EndSearch;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__EndSearch(soap, &soap_tmp___tse__EndSearch);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__EndSearch(soap, &soap_tmp___tse__EndSearch, "-tse:EndSearch", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__EndSearch(soap, &soap_tmp___tse__EndSearch, "-tse:EndSearch", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_EndSearch(_tse__EndSearchResponse &tse__EndSearchResponse)
{
	tse__EndSearchResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__EndSearchResponse.soap_get(soap, "tse:EndSearchResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_FindMetadata(const char *soap_endpoint_url, const char *soap_action, _tse__FindMetadata *tse__FindMetadata)
{
	struct __tse__FindMetadata soap_tmp___tse__FindMetadata;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/FindMetadata";
	soap_tmp___tse__FindMetadata.tse__FindMetadata = tse__FindMetadata;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__FindMetadata(soap, &soap_tmp___tse__FindMetadata);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__FindMetadata(soap, &soap_tmp___tse__FindMetadata, "-tse:FindMetadata", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__FindMetadata(soap, &soap_tmp___tse__FindMetadata, "-tse:FindMetadata", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_FindMetadata(_tse__FindMetadataResponse &tse__FindMetadataResponse)
{
	tse__FindMetadataResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__FindMetadataResponse.soap_get(soap, "tse:FindMetadataResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

int SearchBindingProxy::send_GetMetadataSearchResults(const char *soap_endpoint_url, const char *soap_action, _tse__GetMetadataSearchResults *tse__GetMetadataSearchResults)
{
	struct __tse__GetMetadataSearchResults soap_tmp___tse__GetMetadataSearchResults;
	if (soap_endpoint_url != NULL)
		soap_endpoint = soap_endpoint_url;
	if (soap_action == NULL)
		soap_action = "http://www.onvif.org/ver10/search/wsdl/GetMetadataSearchResults";
	soap_tmp___tse__GetMetadataSearchResults.tse__GetMetadataSearchResults = tse__GetMetadataSearchResults;
	soap_begin(soap);
	soap_set_version(soap, 2); /* use SOAP1.2 */
	soap->encodingStyle = NULL; /* use SOAP literal style */
	soap_serializeheader(soap);
	soap_serialize___tse__GetMetadataSearchResults(soap, &soap_tmp___tse__GetMetadataSearchResults);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put___tse__GetMetadataSearchResults(soap, &soap_tmp___tse__GetMetadataSearchResults, "-tse:GetMetadataSearchResults", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put___tse__GetMetadataSearchResults(soap, &soap_tmp___tse__GetMetadataSearchResults, "-tse:GetMetadataSearchResults", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

int SearchBindingProxy::recv_GetMetadataSearchResults(_tse__GetMetadataSearchResultsResponse &tse__GetMetadataSearchResultsResponse)
{
	tse__GetMetadataSearchResultsResponse.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	tse__GetMetadataSearchResultsResponse.soap_get(soap, "tse:GetMetadataSearchResultsResponse", NULL);
	if (soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}
/* End of client proxy code */
