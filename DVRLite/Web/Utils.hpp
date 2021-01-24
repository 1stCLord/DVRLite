#pragma once

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>
#include <string>
#include <chrono>

//#define VIDEOLIST_TAG "#videolist#"
//#define ROOT_PATH "F:/Projects/DVRLite/web/"
//#define VIDEO_PATH "F:/Projects/DVRLite/videos"
//#define PORT 8000

class StaticFilesManager
{
private:
  oatpp::concurrency::SpinLock m_lock;
  std::unordered_map<oatpp::String, oatpp::String> m_cache;
private:
  oatpp::String getExtension(const oatpp::String& filename);
public:  
  oatpp::String getFile(const oatpp::String& path);
  
  oatpp::String guessMimeType(const oatpp::String& filename);
  
};

oatpp::String loadFromFile(const char* fileName);
oatpp::String formatText(const char* text, ...);
v_int64 getMillisTickCount();

std::string escapeUrl(std::string &url);
std::string unescapeUrl(std::string& url);
std::string replace_substring(const std::string& source, const std::string& tag, const std::string& replacement);
bool replace_substring(const std::string& source, const std::string& tag, const std::string& replacement, std::string& result);
bool split_string(const std::string& source, char token, std::string* first, std::string* second);

std::string createFullOnvifPath(const std::string& url);

std::string to_string(std::chrono::system_clock::time_point time, const std::string &format);