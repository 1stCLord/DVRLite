#pragma once

#include "Utils.hpp"
#include "Source.h"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/Server.hpp"
#include <filesystem>
#include <json/json.h>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <cstdlib>

#include "DVRLite.h"

namespace DVRLite
{
    class MediaController : public oatpp::web::server::api::ApiController
    {
    public:
        typedef MediaController __ControllerType;
    private:
        OATPP_COMPONENT(std::shared_ptr<StaticFilesManager>, staticFileManager);
        DVRLite* dvrlite;
        oatpp::network::Server* server = nullptr;
        static const Logger::LogFilter filter = Logger::LogFilter::Web;
    private:
        std::shared_ptr<OutgoingResponse> getStaticFileResponse(const oatpp::String& filename, const oatpp::String& rangeHeader) const;
        std::shared_ptr<OutgoingResponse> getFullFileResponse(const oatpp::String& file) const;
        std::shared_ptr<OutgoingResponse> getRangeResponse(const oatpp::String& rangeStr, const oatpp::String& file) const;

        std::string CreateHTMLHeader(const std::string& pageTitle) const;
        std::string CreateHeader(const std::string& pageTitle) const;
        std::string CreateSourceList() const;
        std::string CreateVideoList(const Source& source) const;
        std::string CreateVideoSnapshot(const Source& source) const;
        std::string CreateDatePicker(const std::string& label, std::chrono::system_clock::time_point date, const std::string& id) const;
        std::string CreateVideoTimeline(const Source& source, std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to) const;
        std::string CreateSourceCheckboxes(const Source& source) const;
        std::string CreateConfigList() const;
        std::string CreateAddSourceTable(const Source& source) const;
        std::string CreateLog() const; 
        std::string CreateSnapshot(const Source& source) const;

        uint32_t NumberVideosBetweenDates(const Source& source, std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to) const;
        std::vector<Json::Value*> VideosBetweenDates(const Source& source, std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to) const;

        void ApplyTemplates(const std::string& pageTitle, std::string& content, const Source& currentSource, std::chrono::system_clock::time_point startTime, std::chrono::system_clock::time_point endTime) const;
        std::string ApplyTemplate(const std::string& templatename, const std::string& value) const;
        std::string ApplyTemplate(const std::string& templatename, const std::vector<std::string>& value) const;

        Json::Value templates;

    public:
        MediaController(const std::shared_ptr<ObjectMapper>& objectMapper);

        void setDVR(DVRLite* dvrlite) { this->dvrlite = dvrlite; }
        void LoadTemplates();

        static void GetTimes(oatpp::String inStartTime, oatpp::String inEndTime, std::chrono::system_clock::time_point& startTime, std::chrono::system_clock::time_point& endTime);
        static Logger::LogFilter GetLogFilter(const QueryParams& queryParams);
    public:

        /**
         *  Inject @objectMapper component here as default parameter
         */
        static std::shared_ptr<MediaController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) {
            return std::shared_ptr<MediaController>(std::make_shared<MediaController>(objectMapper));
        }

        static std::shared_ptr<oatpp::data::stream::IOStream> authconnection;

        /**
         *  Begin ENDPOINTs generation ('ApiController' codegen)
         */
#include OATPP_CODEGEN_BEGIN(ApiController)


         /**
          *  index.html endpoint
          */

        ENDPOINT_ASYNC("GET", "/", Root)
        {

            ENDPOINT_ASYNC_INIT(Root)

                Action act() override
            {
                std::string webPath = controller->dvrlite->GetConfig().GetWebPath();
                std::string filepath = webPath + "sourcelist.html";
                std::string content = loadFromFile(filepath.c_str())->std_str();
                std::chrono::system_clock::time_point startTime, endTime;
                controller->ApplyTemplates("Sources", content, Source(), startTime, endTime);
                //replace_substring(content, "#sourcelist#", controller->CreateSourceList(), content);

                return _return(controller->createResponse(Status::CODE_200, content.c_str()));
            }

        };
        /*ENDPOINT_ASYNC("GET", "/", Root)
        {

          ENDPOINT_ASYNC_INIT(Root)

          Action act() override
          {
              std::string filepath = std::string(ROOT_PATH) + "videolist.html";
              std::string content = loadFromFile(filepath.c_str())->std_str();
              size_t position = content.find("#videolist#");
              std::string front = content.substr(0, position);
              std::string back = content.substr(position + sizeof(VIDEOLIST_TAG), content.size() - (position + sizeof(VIDEOLIST_TAG)));

              std::string videolist = controller->CreateVideoList("");
              content = front + videolist + back;

              return _return(controller->createResponse(Status::CODE_200, content.c_str()));
          }

        };*/

        ENDPOINT_ASYNC("GET", "add_source", AddSource)
        {

            ENDPOINT_ASYNC_INIT(AddSource)

                Action act() override
            {
                controller->dvrlite->AddSource(Source(request->getQueryParameters()));
                auto response = controller->createResponse(Status::CODE_302, "");
                response->putHeader("Location", "/");
                return _return(response);
            }
        };

        ENDPOINT_ASYNC("GET", "delete_source", DeleteSource)
        {

            ENDPOINT_ASYNC_INIT(DeleteSource)

                Action act() override
            {
                std::string source = request->getQueryParameter("source")->std_str();
                controller->dvrlite->RemoveSource(source);
                auto response = controller->createResponse(Status::CODE_302, "");
                response->putHeader("Location", "/");
                return _return(response);
            }
        };

        ENDPOINT_ASYNC("GET", "configure", Configure)
        {

            ENDPOINT_ASYNC_INIT(Configure)

                Action act() override
            {
                QueryParams queryParams = request->getQueryParameters();
                std::string recordPath = queryParams.get("recordPath")->std_str();
                uint16_t port = std::stoul(queryParams.get("port")->std_str());
                Logger::LogFilter logFilter = MediaController::GetLogFilter(queryParams);
                DVRLite::Config& config = controller->dvrlite->GetConfig();
                config.SetRecordPath(unescapeUrl(recordPath));
                config.SetPort(port);
                config.SetLogFilter(logFilter);
                config.Save();
                auto response = controller->createResponse(Status::CODE_302, "");
                response->putHeader("Location", "/");
                return _return(response);
            }

        };

        /**
         *  Media files download endpoint with support for Range requests
         */
        ENDPOINT_ASYNC("GET", "videos/*", Videos)
        {

            ENDPOINT_ASYNC_INIT(Videos)

                Action act() override
            {
                /*if (request->getConnection() != authconnection)
                    return _return(controller->createResponse(Status::CODE_307, "login"));*/

                std::filesystem::path recordPath = controller->dvrlite->GetConfig().GetRecordPath();
                std::filesystem::path filepath = request->getPathTail()->std_str();
                //std::string filename = filepath.filename().string();
                OATPP_ASSERT_HTTP(filepath.string().c_str(), Status::CODE_400, "Filename is empty");

                auto range = request->getHeader(Header::RANGE);

                return _return(controller->getStaticFileResponse((recordPath / filepath).string().c_str(), range));

            }

        };

        /**
       *  Media files download endpoint with support for Range requests
       */
        ENDPOINT_ASYNC("GET", "login", Login)
        {

            ENDPOINT_ASYNC_INIT(Login)

                Action act() override
            {
                return _return(controller->createResponse(Status::CODE_200, "ok"));
            }

        };

        ENDPOINT_ASYNC("GET", "restart", Restart)
        {

            ENDPOINT_ASYNC_INIT(Restart)

                Action act() override
            {
                controller->server->stop();
#ifdef _WIN32
                system("sc start \"DVRLite\"");
#else
                system("sudo systemctrl restart DVRLite");
#endif
                return _return(controller->createResponse(Status::CODE_200, "ok"));
            }

        };

        ENDPOINT_ASYNC("GET", "shutdown", Shutdown)
        {

            ENDPOINT_ASYNC_INIT(Shutdown)

                Action act() override
            {
                controller->server->stop();

                return _return(controller->createResponse(Status::CODE_200, "ok"));
            }

        };

        ENDPOINT_ASYNC("GET", "favicon.png", Favicon)
        {

            ENDPOINT_ASYNC_INIT(Favicon)

                Action act() override
            {
                auto range = request->getHeader(Header::RANGE);
                std::string webPath = controller->dvrlite->GetConfig().GetWebPath() + "favicon.png";
                return _return(controller->getStaticFileResponse(oatpp::String(webPath.c_str()), range));

            }

        };

        ENDPOINT_ASYNC("GET", "*", Files)
        {
            ENDPOINT_ASYNC_INIT(Files)

                Action act() override
            {
                std::string webPath = controller->dvrlite->GetConfig().GetWebPath();
                std::string filename = request->getPathTail()->std_str();
                split_string(filename, '?', &filename, nullptr);
                std::string filepath = webPath + filename;
                if (std::filesystem::is_regular_file(filepath))
                {
                    std::filesystem::path file(filename);
                    file.replace_extension("");

                    std::chrono::system_clock::time_point startTime, endTime;
                    MediaController::GetTimes(request->getQueryParameter("startTime"), request->getQueryParameter("endTime"), startTime, endTime);

                    oatpp::String sourcename = request->getQueryParameter("source");
                    std::string content = loadFromFile(filepath.c_str())->std_str();
                    if (sourcename.get() != nullptr)
                    {
                        const Source& source = controller->dvrlite->GetSource(sourcename->std_str());
                        controller->ApplyTemplates(file.string() + " " + sourcename->std_str(), content, source, startTime, endTime);
                    }
                    else
                        controller->ApplyTemplates(file.string(), content, Source(), startTime, endTime);
                    return _return(controller->createResponse(Status::CODE_200, content.c_str()));
                }
                else
                    return _return(controller->createResponse(Status::CODE_404, "File not found"));
            }
        };

        /**
         *  Finish ENDPOINTs generation ('ApiController' codegen)
         */
#include OATPP_CODEGEN_END(ApiController)

        static void run(DVRLite* dvrlite);

    };
}