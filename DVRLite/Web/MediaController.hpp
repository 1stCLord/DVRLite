#pragma once

#include "Utils.hpp"
#include "Source.h"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include <filesystem>

#include <unordered_map>
#include <filesystem>
#include <iostream>

#include "DVRLite.h"

class MediaController : public oatpp::web::server::api::ApiController
{
public:
  typedef MediaController __ControllerType;
private:
  OATPP_COMPONENT(std::shared_ptr<StaticFilesManager>, staticFileManager);
  DVRLite* dvrlite;
private:
  std::shared_ptr<OutgoingResponse> getStaticFileResponse(const oatpp::String& filename, const oatpp::String& rangeHeader) const;
  std::shared_ptr<OutgoingResponse> getFullFileResponse(const oatpp::String& file) const;
  std::shared_ptr<OutgoingResponse> getRangeResponse(const oatpp::String& rangeStr, const oatpp::String& file) const;

  std::string CreateHeader(const std::string &pageTitle) const;
  std::string CreateSourceList() const;
  std::string CreateVideoList(const Source &source) const;
  std::string CreateSourceCheckboxes() const;

  void ApplyTemplates(const std::string& pageTitle, std::string& content, const Source& currentSource) const;

  //static std::string ReplaceTag(const std::string &sourceContent, const std::string &tag, const std::string &replacement);

public:
  MediaController(const std::shared_ptr<ObjectMapper>& objectMapper)
    : oatpp::web::server::api::ApiController(objectMapper)
  {}

  void setDVR(DVRLite* dvrlite) { this->dvrlite = dvrlite; }
public:

  /**
   *  Inject @objectMapper component here as default parameter
   */
  static std::shared_ptr<MediaController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)){
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
          controller->ApplyTemplates("Sources", content, Source());
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

  ENDPOINT_ASYNC("GET", "configure", Configure)
  {

      ENDPOINT_ASYNC_INIT(Configure)

      Action act() override
      {
          QueryParams queryParams = request->getQueryParameters();
          std::string recordPath = queryParams.get("recordPath")->std_str();
          DVRLite::Config &config = controller->dvrlite->GetConfig();
          config.SetRecordPath(recordPath);
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

      auto filename = request->getPathTail();
      OATPP_ASSERT_HTTP(filename, Status::CODE_400, "Filename is empty");

      auto range = request->getHeader(Header::RANGE);
      
      return _return(controller->getStaticFileResponse(filename, range));

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
          return _return(controller->createResponse(Status::CODE_200,"ok"));
      }

  };

  ENDPOINT_ASYNC("GET", "*", Files)
  {
      ENDPOINT_ASYNC_INIT(Files)

          Action act() override
      {
          std::string webPath = controller->dvrlite->GetConfig().GetWebPath();
          std::string filename = request->getPathTail()->std_str();
          std::string filepath = webPath + filename;
          if (std::filesystem::is_regular_file(filepath))
          {
              std::filesystem::path file(filename);
              file.replace_extension("");
              std::string content = loadFromFile(filepath.c_str())->std_str();
              controller->ApplyTemplates(file.string(), content, Source());
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

  static void run(DVRLite *dvrlite);
  
};