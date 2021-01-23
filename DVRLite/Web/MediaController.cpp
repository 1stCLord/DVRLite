
#include "MediaController.hpp"
#include "AppComponent.hpp"

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
    return "<a href=\"/\"><h1>DVRLite</h1></a><hr>" + pageTitle + "<div style=\"width: 30%;float: right;\"><a href=\"add.html\">Add</a><a href=\"config.html\">config</a></div><hr>";
}

std::string MediaController::CreateSourceList() const
{
    std::string sourcelist;
    sourcelist += "<table style=\"width: 100%;\">";
    for (const Source& source : dvrlite->GetSources())
    {
        sourcelist += "<tr>";
        sourcelist += "<td>" + source.GetName() + "</td>";
        sourcelist += "<td>" + source.GetOnvifAddress() + "</td>";
        sourcelist += "<td>" + source.GetVideoAddress() + "</td>";
        sourcelist += "<td>" + std::to_string(source.GetDuration()) + "</td>";
        sourcelist += "<td>" + std::to_string(source.GetQuota()) + "</td>";
        sourcelist += "<td>";
        for (const std::string& trigger : source.GetTriggers())
            sourcelist += trigger + "</br>";
        sourcelist += "</td>";
        sourcelist += "<td><a href=\"delete_source?source=" + source.GetName() + "\">delete</a></td>";
        sourcelist += "</tr>\n";
    }
    sourcelist += "</table>";
    return sourcelist;
}

std::string MediaController::CreateVideoList(const Source& source) const
{
    std::string videolist;
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("videos/" + source.GetName()))
    {
        if (entry.path().extension() == ".mp4")
        {
            videolist += "<a href='videos/";
            videolist += entry.path().filename().string();
            videolist += "'>";
            videolist += entry.path().filename().string();
            videolist += std::string("</a><br>");
        }
    }
    return videolist;
}

std::string MediaController::CreateSourceCheckboxes() const
{
    std::string sourcecheckboxes;
    for (const Source& source : dvrlite->GetSources())
        sourcecheckboxes += "<input id=\"trigger_" + source.GetName() + "\" name=\"trigger_" + source.GetName() + "\" type=\"checkbox\">" + source.GetName() + "</br>";
    return sourcecheckboxes;
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
}

void MediaController::run(DVRLite *dvrlite)
{
    oatpp::base::Environment::init();

    AppComponent components; // Create scope Environment components

    /* create ApiControllers and add endpoints to router */

    auto router = components.httpRouter.getObject();

    auto mediaController = MediaController::createShared();
    mediaController->setDVR(dvrlite);
    mediaController->addEndpointsToRouter(router);

    /* create server */

    oatpp::network::Server server(components.serverConnectionProvider.getObject(),
        components.serverConnectionHandler.getObject());

    OATPP_LOGI("Server", "Running on port %s...", components.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());

    server.run();

    oatpp::base::Environment::destroy();
}
