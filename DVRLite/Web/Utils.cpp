
#include "Utils.hpp"

#include <fstream>
#include <cstring>
#include <stdarg.h>
#include <iomanip>
#include <sstream>

namespace DVRLite
{

    oatpp::String StaticFilesManager::getExtension(const oatpp::String& filename)
    {
        v_int32 dotPos = 0;
        for (v_int32 i = filename->getSize() - 1; i > 0; i--) {
            if (filename->getData()[i] == '.') {
                dotPos = i;
                break;
            }
        }
        if (dotPos != 0 && dotPos < filename->getSize() - 1) {
            return oatpp::String((const char*)&filename->getData()[dotPos + 1], filename->getSize() - dotPos - 1);
        }
        return nullptr;
    }

    oatpp::String StaticFilesManager::getFile(const oatpp::String& path)
    {
        if (!path) {
            return nullptr;
        }
        std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
        auto& file = m_cache[path];
        if (file) {
            return file;
        }
        oatpp::String filename = path;
        file = loadFromFile(filename->c_str());
        return file;
    }

    oatpp::String StaticFilesManager::guessMimeType(const oatpp::String& filename)
    {
        auto extension = getExtension(filename);
        if (extension) {

            if (extension->equals("m3u8")) {
                return "application/x-mpegURL";
            }
            else if (extension->equals("mp4")) {
                return "video/mp4";
            }
            else if (extension->equals("ts")) {
                return "video/MP2T";
            }
            else if (extension->equals("mp3")) {
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
        va_start(args, text);
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

    std::string stdLoadFile(const std::filesystem::path& filePath)
    {
        std::ifstream fileStream = std::ifstream(filePath);

        std::string contents;
        if (fileStream.is_open())
        {
            fileStream.seekg(0, std::ios::end);
            size_t length = fileStream.tellg();
            fileStream.seekg(0, std::ios::beg);
            contents = std::string(length, 0);
            fileStream.read(contents.data(), length);
        }
        return contents;
    }

    std::string escapeUrl(const std::string& url)
    {
        static_assert("TODO");
        return std::string();
    }

    std::string unescapeUrl(const std::string& url)
    {
        std::string resultString;
        size_t end = 0, position = 0;
        while ((position = url.find('%', end)) != std::string::npos)
        {
            resultString += url.substr(end, position - end);
            //end = url.find(' ');
            end = position + 3;
            std::string hexstring = "0x" + url.substr(position + 1, 2);
            char c = stoi(hexstring, 0, 16);
            resultString += c;
        }
        resultString += url.substr(end);

        return resultString;
    }

    std::string replace_substring(const std::string& source, const std::string& tag, const std::string& replacement)
    {
        std::string result;
        replace_substring(source, tag, replacement, result);
        return result;
    }

    void replace_all_substrings(const std::string& source, const std::string& tag, const std::string& replacement, std::string& result)
    {
        bool found = true;
        while (found)
        {
            found = replace_substring(result, tag, replacement, result);
        }
    }

    bool replace_substring(const std::string& source, const std::string& tag, const std::string& replacement, std::string& result)
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

    bool split_string(const std::string& source, char token, std::string* first, std::string* second)
    {
        size_t position = source.find(token);
        if (position == std::string::npos)return false;
        if (first)*first = source.substr(0, position);
        if (second)*second = source.substr(position);
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
        if (portEnd == std::string::npos || portEnd == withoutDomainOrIp.size() - 1)
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

    std::string to_string(std::chrono::system_clock::time_point time, const std::string& format, const std::chrono::time_zone& timezone)
    {
        std::chrono::zoned_time zoned_time(&timezone, time);
        std::chrono::local_time<std::chrono::system_clock::duration> local_time = zoned_time.get_local_time();

        return std::format(format, zoned_time);
    }
#ifdef _WIN32
#define timegm _mkgmtime
#endif
    std::chrono::system_clock::time_point to_timepoint(const std::string& timeString, const std::string& format, const std::chrono::time_zone& timezone, bool fixup)
    {
        //strip the lib fmt tokens
        std::string tmpFmt = format.substr(2, format.size() - 3);
        std::string fixedTimeString;
        if (fixup)
            fixedTimeString = replace_substring(timeString, "+", " ");
        else
            fixedTimeString = timeString;

        std::chrono::local_time<std::chrono::system_clock::duration> local_time;
        //std::chrono::system_clock::time_point time;
        std::stringstream stream(fixedTimeString);
        std::chrono::from_stream(stream, tmpFmt.c_str(), local_time);
        std::chrono::zoned_time zoned_time(&timezone, local_time);
        return zoned_time.get_sys_time();
        //return time;
    }


    bool is_year_in_date_range(const std::string &yearString, const std::chrono::time_zone& timezone, std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to)
    {
        int32_t year;
        try
        {
            year = std::stoi(yearString);
        }
        catch (...) { return false; }

        std::chrono::zoned_time from_zoned_time(&timezone, from);
        std::chrono::zoned_time to_zoned_time(&timezone, to);

        std::chrono::local_days from_local_days = std::chrono::floor<std::chrono::days>(from_zoned_time.get_local_time());
        std::chrono::year_month_day from_ymd(from_local_days);

        std::chrono::local_days to_local_days = std::chrono::floor<std::chrono::days>(to_zoned_time.get_local_time());
        std::chrono::year_month_day to_ymd(to_local_days);

        return year >= (int)from_ymd.year() && year <= (int)to_ymd.year();
    }

    bool is_day_in_date_range(const std::string& dayString, const std::chrono::time_zone& timezone, const std::string& formatString, std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to)
    {

        std::chrono::zoned_time from_zoned_time(&timezone, from);
        std::chrono::zoned_time to_zoned_time(&timezone, to);

        std::chrono::local_days from_local_days = std::chrono::floor<std::chrono::days>(from_zoned_time.get_local_time());
        std::chrono::year_month_day from_ymd(from_local_days);

        std::chrono::local_days to_local_days = std::chrono::floor<std::chrono::days>(to_zoned_time.get_local_time());
        std::chrono::year_month_day to_ymd(to_local_days);

        std::chrono::local_time<std::chrono::system_clock::duration> local_time;
        std::stringstream stream(dayString);
        std::chrono::from_stream(stream, formatString.c_str(), local_time);
        std::chrono::zoned_time zoned_time(&timezone, local_time);
        std::chrono::local_days local_days = std::chrono::floor<std::chrono::days>(zoned_time.get_local_time());
        std::chrono::year_month_day ymd(local_days);

        return ymd >= from_ymd && ymd <= to_ymd;
    }

    std::pair<int, int> calculate_year_range(std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to, const std::chrono::time_zone& timezone)
    {
        std::chrono::zoned_time from_zoned_time(&timezone, from);
        std::chrono::zoned_time to_zoned_time(&timezone, to);

        std::chrono::local_days from_local_days = std::chrono::floor<std::chrono::days>(from_zoned_time.get_local_time());
        std::chrono::year_month_day from_ymd(from_local_days);

        std::chrono::local_days to_local_days = std::chrono::floor<std::chrono::days>(to_zoned_time.get_local_time());
        std::chrono::year_month_day to_ymd(to_local_days);
        return { (int)from_ymd.year(), (int)to_ymd.year() };
    }

    std::pair<int, int> calculate_month_range(std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to, int currentYear, const std::chrono::time_zone& timezone)
    {
        std::chrono::zoned_time from_zoned_time(&timezone, from);
        std::chrono::zoned_time to_zoned_time(&timezone, to);

        std::chrono::local_days from_local_days = std::chrono::floor<std::chrono::days>(from_zoned_time.get_local_time());
        std::chrono::year_month_day from_ymd(from_local_days);

        std::chrono::local_days to_local_days = std::chrono::floor<std::chrono::days>(to_zoned_time.get_local_time());
        std::chrono::year_month_day to_ymd(to_local_days);

        std::pair<int, int> result;
        result.first = 1;
        if (currentYear == (int)from_ymd.year())
            result.first = (uint32_t)from_ymd.month();
        result.second = 12;
        if (currentYear == (int)to_ymd.year())
            result.second = (uint32_t)to_ymd.month();
        return result;
    }

    std::pair<int, int> calculate_day_range(std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to, int currentYear, int currentMonth, const std::chrono::time_zone& timezone)
    {
        std::chrono::zoned_time from_zoned_time(&timezone, from);
        std::chrono::zoned_time to_zoned_time(&timezone, to);

        std::chrono::local_days from_local_days = std::chrono::floor<std::chrono::days>(from_zoned_time.get_local_time());
        std::chrono::year_month_day from_ymd(from_local_days);

        std::chrono::local_days to_local_days = std::chrono::floor<std::chrono::days>(to_zoned_time.get_local_time());
        std::chrono::year_month_day to_ymd(to_local_days);

        std::pair<int, int> result;
        result.first = 1;
        if ((currentYear == (int)from_ymd.year()) && (currentMonth == (uint32_t)from_ymd.month()))
            result.first = (uint32_t)from_ymd.day();
        result.second = 31;
        if ((currentYear == (int)to_ymd.year()) && (currentMonth == (uint32_t)to_ymd.month()))
            result.second = (uint32_t)to_ymd.day();
        return result;
    }

    //video filename format yyyymmdd-hh-mm-ss
    std::string filename_to_datestring(const std::string& filename)
    {
        std::string year = filename.substr(0, 4);
        std::string month = filename.substr(4, 2);
        std::string day = filename.substr(6, 2);
        std::string hour = filename.substr(9, 2);
        std::string minute = filename.substr(12, 2);
        std::string second = filename.substr(15, 2);
        return year + '-' + month + '-' + day + ' ' + hour + ':' + minute + ':' + second;
    }

    std::string strip_quotes(const std::string& quotedString, const std::string& refString)
    {
        if (quotedString.size() > refString.size())
            return quotedString.substr(1, quotedString.size() - 2);
        else return quotedString;
    }

    std::string bytes_to_string(uintmax_t bytes)
    {
        const std::vector<std::string> suffixes{ "B", "KB", "MB", "GB", "TB", "PB" };

        int magnitude = 0;
        while (bytes > 1024 && magnitude < suffixes.size())
        {
            bytes /= 1024;
            ++magnitude;
        }
        return std::to_string(bytes) + suffixes[magnitude];
    }

}