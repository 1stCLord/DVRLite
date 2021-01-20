
#include "Utils.hpp"

#include <fstream>
#include <cstring>
#include <stdarg.h>

oatpp::String StaticFilesManager::getExtension(const oatpp::String& filename) 
{
  v_int32 dotPos = 0;
  for(v_int32 i = filename->getSize() - 1; i > 0; i--) {
    if(filename->getData()[i] == '.') {
      dotPos = i;
      break;
    }
  }
  if(dotPos != 0 && dotPos < filename->getSize() - 1) {
    return oatpp::String((const char*)&filename->getData()[dotPos + 1], filename->getSize() - dotPos - 1);
  }
  return nullptr;
}

oatpp::String StaticFilesManager::getFile(const oatpp::String& path) 
{
  if(!path) {
    return nullptr;
  }
  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  auto& file = m_cache [path];
  if(file) {
    return file;
  }
  oatpp::String filename = m_basePath + "/" + path;
  file = loadFromFile(filename->c_str());
  return file;
}

oatpp::String StaticFilesManager::guessMimeType(const oatpp::String& filename) 
{
  auto extension = getExtension(filename);
  if(extension) {
    
    if(extension->equals("m3u8")){
      return "application/x-mpegURL";
    } else if(extension->equals("mp4")){
      return "video/mp4";
    } else if(extension->equals("ts")){
      return "video/MP2T";
    } else if(extension->equals("mp3")){
      return "audio/mp3";
    }
    
  }
  return nullptr;
}

oatpp::String loadFromFile(const char* fileName)
{

    std::ifstream file(fileName, std::ios::in | std::ios::binary | std::ios::ate);

    if (file.is_open()) {

        auto result = oatpp::String((v_int32)file.tellg());
        file.seekg(0, std::ios::beg);
        file.read((char*)result->getData(), result->getSize());
        file.close();
        return result;

    }

    return nullptr;
}

oatpp::String formatText(const char* text, ...)
{
  char buffer[4097];
  va_list args;
  va_start (args, text);
  vsnprintf(buffer, 4096, text, args);
  va_end(args);
  return oatpp::String(buffer);
}

v_int64 getMillisTickCount()
{
  std::chrono::milliseconds millis = std::chrono::duration_cast<std::chrono::milliseconds>
  (std::chrono::system_clock::now().time_since_epoch());
  return millis.count();
}

std::string escapeUrl(std::string& url)
{
    //TODO
    return std::string();
}

std::string unescapeUrl(std::string& url)
{
    std::string resultString;
    size_t end = 0, position = 0;
    while ((position = url.find('%', end)) != std::string::npos)
    {
        resultString += url.substr(end, position-end);
        //end = url.find(' ');
        end = position + 3;
        std::string hexstring = "0x" + url.substr(position+1, 2);
        char c = stoi(hexstring,0,16);
        resultString += c;
    }
    resultString += url.substr(end);

    return resultString;
}

bool replace_substring(const std::string& source, const std::string& tag, const std::string& replacement, std::string &result)
{
    size_t position = source.find(tag);
    if (position == std::string::npos)
    {
        result = source;
        return false;
    }
    std::string front = source.substr(0, position);
    std::string back = source.substr(position + tag.size(), source.size() - (position + tag.size()));
    result = front + replacement + back;
    return true;
}

std::string createFullOnvifPath(const std::string& url)
{
    size_t schemeEnd = url.find("://");
    std::string scheme = url.substr(0, schemeEnd + 3);
    std::string withoutScheme = url.substr(schemeEnd + 3);
    std::string domainOrIp;

    //port
    size_t domainOrIpEnd = withoutScheme.find(':');
    std::string withoutDomainOrIp, port;
    if (domainOrIpEnd == std::string::npos)
    {
        withoutDomainOrIp = withoutScheme;
        domainOrIp = withoutDomainOrIp;
        port = ":8000";
    }
    else
    {
        withoutDomainOrIp = withoutScheme.substr(domainOrIpEnd);
        domainOrIp = withoutScheme.substr(0, domainOrIpEnd);
    }

    //endpoint
    size_t portEnd = withoutDomainOrIp.find('/');
    std::string endpoint;
    if (portEnd == std::string::npos || portEnd == withoutDomainOrIp.size()-1)
        endpoint = "/onvif/device_service";
    else
        endpoint = withoutDomainOrIp.substr(portEnd);

    if (port.empty())
    {
        if (portEnd == std::string::npos)
            port = withoutDomainOrIp;
        else
            port = withoutDomainOrIp.substr(0, portEnd);
    }

    return scheme + domainOrIp + port + endpoint;
}
