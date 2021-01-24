
#include "MediaController.hpp"
#include "AppComponent.hpp"
#include <fstream>
#include "oatpp/network/Server.hpp"
#include "DVRLite.h"

std::shared_ptr<oatpp::data::stream::IOStream> MediaController::authconnection;

std::shared_ptr<MediaController::OutgoingResponse> MediaController::getStaticFileResponse(const oatpp::String& filename,
                                                                                          const oatpp::String& rangeHeader) const
{

  auto file = staticFileManager->getFile(filename);

  OATPP_ASSERT_HTTP(file.get() != nullptr, Status::CODE_404, "File not found");

  std::shared_ptr<OutgoingResponse> response;

  if(!rangeHeader) {
    response = getFullFileResponse(file);
  } else {
    response = getRangeResponse(rangeHeader, file);
  }

  response->putHeader("Accept-Ranges", "bytes");
  response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);
  auto mimeType = staticFileManager->guessMimeType(filename);
  if(mimeType) {
    response->putHeader(Header::CONTENT_TYPE, mimeType);
  } else {
    OATPP_LOGD("Server", "Unknown Mime-Type. Header not set");
  }

  return response;

}

std::shared_ptr<MediaController::OutgoingResponse> MediaController::getFullFileResponse(const oatpp::String& file) const
{
  return createResponse(Status::CODE_200, file);
}

std::shared_ptr<MediaController::OutgoingResponse> MediaController::getRangeResponse(const oatpp::String& rangeStr,
                                                                                     const oatpp::String& file) const
{

  auto range = oatpp::web::protocol::http::Range::parse(rangeStr.getPtr());

  if(range.end == 0) {
    range.end = file->getSize() - 1;
  }

  OATPP_ASSERT_HTTP(range.isValid() &&
                    range.start < file->getSize() &&
                    range.end > range.start &&
                    range.end < file->getSize(), Status::CODE_416, "Range is invalid");

  auto chunk = oatpp::String((const char*)&file->getData()[range.start], (v_int32)(range.end - range.start + 1), false);

  auto response = createResponse(Status::CODE_206, chunk);

  oatpp::web::protocol::http::ContentRange contentRange(oatpp::web::protocol::http::ContentRange::UNIT_BYTES,
                                                        range.start, range.end, file->getSize(), true);

  OATPP_LOGD("Server", "range=%s", contentRange.toString()->c_str());

  response->putHeader(Header::CONTENT_RANGE, contentRange.toString());
  return response;

}

std::string MediaController::CreateHeader(const std::string &pageTitle) const
{
    return templates["headertop"].asString() +
    ApplyTemplate("headertitle", pageTitle) +
    templates["headermenu"].asString();
}

std::string MediaController::CreateSourceList() const
{
    std::string sourcelist;
    sourcelist += templates["sourceheader"].asString();
    for (const Source& source : dvrlite->GetSources())
    {
        std::string sourcedata;
        std::vector<std::string> sourceLinkParameters{ std::string((const char*)u8"📹") + source.GetName(), source.GetName() };
        sourcedata += ApplyTemplate("sourcelink", sourceLinkParameters);
        sourcedata += ApplyTemplate("sourcedata", source.GetOnvifAddress());
        sourcedata += ApplyTemplate("sourcedata", source.GetVideoAddress());
        sourcedata += ApplyTemplate("sourcedata", std::to_string(source.GetDuration()));
        sourcedata += ApplyTemplate("sourcedata", std::to_string(source.GetQuota()));
        std::string triggers;
        for (const std::string& trigger : source.GetTriggers())
            triggers += trigger + "</br>";
        sourcedata += ApplyTemplate("sourcedata", triggers);
        sourcedata += ApplyTemplate("sourcedeletebutton", source.GetName());

        sourcelist += ApplyTemplate("sourcerecord", sourcedata);
    }
    sourcelist = ApplyTemplate("sourcetable", sourcelist);
    return sourcelist;
}

std::string MediaController::CreateVideoList(const Source& source) const
{
    std::string videolist;
    std::filesystem::path videoDirectory = dvrlite->GetConfig().GetRecordPath();
    videoDirectory = videoDirectory / source.GetName();
    if (std::filesystem::is_directory(videoDirectory))
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(videoDirectory))
        {
            if (entry.path().extension() == ".mp4")
            {
                videolist += ApplyTemplate("videoelement", entry.path().filename().string());
            }
        }
    }
    return videolist;
}

std::string MediaController::CreateSourceCheckboxes() const
{
    std::string sourcecheckboxes;
    for (const Source& source : dvrlite->GetSources())
        sourcecheckboxes += ApplyTemplate("sourcecheckboxes", source.GetName());
    return sourcecheckboxes;
}

std::string MediaController::CreateConfigList() const
{
    std::string configlist;
    std::vector<std::string> recordParameters{ "\"recordPath\"", "Record Path ", dvrlite->GetConfig().GetRecordPath(), "required" };
    configlist = ApplyTemplate("textrecord", recordParameters);
    std::vector<std::string> portParameters{ "\"port\"", "Port ", std::to_string(dvrlite->GetConfig().GetPort()), "" };
    configlist += ApplyTemplate("numberrecord", portParameters);
    configlist += templates["submitrecord"].asString();
    return ApplyTemplate("configform", configlist);
}

std::string MediaController::ApplyTemplate(const std::string& templatename, const std::string& value) const
{
    Json::Value jsonvalue = templates[templatename];
    std::string result = templates[templatename].asString();
    bool found = true;
    while(found)
        found = replace_substring(result, "{}", value, result);
    return result;
}

std::string MediaController::ApplyTemplate(const std::string& templatename, const std::vector<std::string>& values) const
{
    Json::Value jsonvalue = templates[templatename];
    std::string result = templates[templatename].asString();
    for (int i = 0; i < values.size(); ++i)
    {
        std::string tag = '{' + std::to_string(i) + '}';
        bool found = true;
        while (found)
            found = replace_substring(result, tag, values[i], result);
    }
    return result;
}

void MediaController::ApplyTemplates(const std::string &pageTitle, std::string& content, const Source &currentSource) const
{
    if (content.find("#header#") != std::string::npos)
        replace_substring(content, "#header#", CreateHeader(pageTitle), content);
    if (content.find("#sourcelist#") != std::string::npos)
        replace_substring(content, "#sourcelist#", CreateSourceList(), content);
    if (content.find("#videolist#") != std::string::npos)
        replace_substring(content, "#videolist#", CreateVideoList(currentSource), content);
    if (content.find("#sourcecheckboxes#") != std::string::npos)
        replace_substring(content, "#sourcecheckboxes#", CreateSourceCheckboxes(), content);
    if (content.find("#configform#") != std::string::npos)
        replace_substring(content, "#configform#", CreateConfigList(), content);
    if (content.find("#htmlheader#") != std::string::npos)
        replace_substring(content, "#htmlheader#", templates["htmlheader"].asString(), content);
}

MediaController::MediaController(const std::shared_ptr<ObjectMapper>& objectMapper) :
    oatpp::web::server::api::ApiController(objectMapper)
{
}

void MediaController::LoadTemplates()
{
    std::string webPath = dvrlite->GetConfig().GetWebPath(); 
    std::ifstream file(std::filesystem::path(webPath) / "templates.json");
    if (file.is_open())
    {
        file >> templates;
    }
}

void MediaController::run(DVRLite *dvrlite)
{
    oatpp::base::Environment::init();

    AppComponent components; // Create scope Environment components

    /* create ApiControllers and add endpoints to router */

    auto router = components.httpRouter.getObject();

    auto mediaController = MediaController::createShared();
    mediaController->setDVR(dvrlite);
    mediaController->LoadTemplates();
    mediaController->addEndpointsToRouter(router);

    /* create server */

    oatpp::network::Server server(components.serverConnectionProvider.getObject(),
        components.serverConnectionHandler.getObject());

    OATPP_LOGI("Server", "Running on port %s...", components.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());

    server.run();

    oatpp::base::Environment::destroy();
}
