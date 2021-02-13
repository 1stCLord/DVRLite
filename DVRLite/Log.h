#pragma once	
#include <atomic>
#include <string>

namespace DVRLite::Logger
{
	enum class LogFilter
	{
		None = 0,
		DVRLite = 1 << 0,
		Onvif = 1 << 1,
		PullPointSubscription = 1 << 2,
		FFmpeg = 1 << 3,
		Web = 1 << 4
	};

	inline LogFilter operator&(LogFilter first, LogFilter second)
	{
		return static_cast<LogFilter>(static_cast<uint8_t>(first) & static_cast<uint8_t>(second));
	}
	
	inline LogFilter operator|(LogFilter first, LogFilter second)
	{
		return static_cast<LogFilter>(static_cast<uint8_t>(first) | static_cast<uint8_t>(second));
	}

	inline LogFilter& operator&=(LogFilter& first, LogFilter second) { first = first & second; return first; }
	inline LogFilter& operator|=(LogFilter& first, LogFilter second) { first = first | second; return first; }

	inline bool Has(LogFilter filter, LogFilter includes)
	{
		return (filter & includes) != LogFilter::None;
	}

	void Init(const std::string& path);
	void Filter(LogFilter filter);
	void Log(LogFilter filter, const std::string& logline);
}