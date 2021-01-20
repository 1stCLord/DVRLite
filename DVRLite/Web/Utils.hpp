#pragma once

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>
#include <string>

#define VIDEOLIST_TAG "#videolist#"
#define ROOT_PATH "F:/Projects/DVRLite/web/"
#define VIDEO_PATH "F:/Projects/DVRLite/videos"
#define PORT 8000

class StaticFilesManager
{
private:
  oatpp::String m_basePath;
  oatpp::concurrency::SpinLock m_lock;
  std::unordered_map<oatpp::String, oatpp::String> m_cache;
private:
  oatpp::String getExtension(const oatpp::String& filename);
public:
  
  StaticFilesManager(const oatpp::String& basePath)
    : m_basePath(basePath)
  {}
  
  oatpp::String getFile(const oatpp::String& path);
  
  oatpp::String guessMimeType(const oatpp::String& filename);
  
};

oatpp::String loadFromFile(const char* fileName);
oatpp::String formatText(const char* text, ...);
v_int64 getMillisTickCount();

std::string escapeUrl(std::string &url);
std::string unescapeUrl(std::string& url);
bool replace_substring(const std::string& source, const std::string& tag, const std::string& replacement, std::string& result);

std::string createFullOnvifPath(const std::string& url);