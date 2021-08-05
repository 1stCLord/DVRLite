#pragma once

#include "oatpp/core/concurrency/SpinLock.hpp"
#include "oatpp/core/Types.hpp"

#include <unordered_map>
#include <string>
#include <chrono>
#include <filesystem>

//#define VIDEOLIST_TAG "#videolist#"
//#define ROOT_PATH "F:/Projects/DVRLite/web/"
//#define VIDEO_PATH "F:/Projects/DVRLite/videos"
//#define PORT 8000

namespace DVRLite
{

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

	std::string stdLoadFile(const std::filesystem::path& filePath);

	std::string escapeUrl(const std::string& url);
	std::string unescapeUrl(const std::string& url);
	void replace_all_substrings(const std::string& source, const std::string& tag, const std::string& replacement, std::string& result);
	std::string replace_substring(const std::string& source, const std::string& tag, const std::string& replacement);
	bool replace_substring(const std::string& source, const std::string& tag, const std::string& replacement, std::string& result);
	bool split_string(const std::string& source, char token, std::string* first, std::string* second);

	std::string createFullOnvifPath(const std::string& url);

	std::string to_string(std::chrono::system_clock::time_point time, const std::string& format);
	std::chrono::system_clock::time_point to_timepoint(const std::string& timeString, const std::string& format, bool fixup = true);
	bool is_year_in_date_range(const std::string& yearString, std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to);
	bool is_day_in_date_range(const std::string& dayString, const std::string& formatString, std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to);
	std::string filename_to_datestring(const std::string& filename);

	std::pair<int, int> calculate_year_range(std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to);
	std::pair<int, int> calculate_month_range(std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to, int currentYear);
	std::pair<int, int> calculate_day_range(std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to, int currentYear, int currentMonth);

	std::string strip_quotes(const std::string& quotedString, const std::string& refString);

	std::string bytes_to_string(uintmax_t bytes);
}

#define DATESTRINGFORMAT "%Y-%m-%d %H:%M:%S"
#define DATESTRINGFORMATQUOTES "'%Y-%m-%d %H:%M:%S'"
#define DATEREFSTRING "YYYY-mm-dd HH:MM:SS"
#define DATEFOLDERFORMAT "%Y/%m-%d"
#define DATEFILEFORMAT "%H-%M-%S"
#define DATEPICKERSTRINGFORMAT "%Y-%m-%d"