// DVRLite.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <unordered_set>
#include <unordered_map>
#include "Log.h"
#include "Onvif/Onvif.h"
#include "Source.h"
#include "FFmpeg/FFmpeg.h"
#include "Web/JsonCache.h"

namespace DVRLite
{

	class DVRLite
	{
	public:
		class Config
		{
		private:
			mutable std::mutex configMutex;
			const std::string configPath;
			std::string recordPath;
			const std::string webPath;
			std::string theme;
			Logger::LogFilter logFilter;
			const bool isService;

		public:
			Config(const std::string& configPath, const std::string& webPath, uint16_t port, bool isService);

			void Load();
			void Save() const;

			void SetRecordPath(const std::string& recordPath);
			std::string GetRecordPath() const;
			std::string GetWebPath() const;
			std::string GetSourcePath() const;
			std::string GetLogPath() const;
			std::string GetTheme() const;
			Logger::LogFilter GetLogFilter() const;
			void SetPort(uint16_t port);
			void SetTheme(const std::string &theme);
			void SetLogFilter(Logger::LogFilter filter);
			uint16_t GetPort()const;
			bool IsService()const;

			static uint16_t Port;
		};

		using SourceSet = std::unordered_set <Source, Source::Hash>;


		DVRLite(const std::string& configPath = "config.json", const std::string& webPath = "web/", uint16_t port = 8000, bool isService = false);

		void AddSource(const Source& source);
		void RemoveSource(const std::string& source);

		const Source& GetSource(const std::string& sourceName) const;
		SourceSet& GetSources() { return sources; }
		FFmpeg* GetFFmpeg(const Source& source) { return GetFFmpeg(source.GetName()); }
		FFmpeg* GetFFmpeg(const Source& source) const { return GetFFmpeg(source.GetName()); }
		FFmpeg* GetFFmpeg(const std::string& source) { return ffmpegs[source].get(); }
		FFmpeg* GetFFmpeg(const std::string& source) const { return (*ffmpegs.find(source)).second.get(); }

		JsonCache& GetCache();

		uintmax_t GetVideoDirectorySize();

		Config& GetConfig();
		const Config& GetConfig() const;
	private:

		SourceSet sources;
		std::unordered_map<std::string, std::unique_ptr<FFmpeg>> ffmpegs;

		Config config;
		Onvif onvif;
		JsonCache cache;

		uintmax_t videoDirectorySize;
		std::filesystem::file_time_type videoDirectoryWriteTime;
	};
}