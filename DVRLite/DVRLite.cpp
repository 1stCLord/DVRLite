// DVRLite.cpp : Defines the entry point for the application.
//

#include "DVRLite.h"
#include "Web/MediaController.hpp"
#include "json/json.h"
#include <filesystem>
#include <fstream>

uint16_t DVRLite::DVRLite::Config::Port = 8000;

int main(int argc, const char* argv[])
{
    std::string configPath = argc >= 2 ? argv[1] : "config.json";
#ifdef _DEBUG
    std::string webPath = argc >= 3 ? argv[2] : "../../../../web/";
#else
    std::string webPath = argc >= 3 ? argv[2] : "web/";
#endif
    uint16_t port = argc >= 4 ? std::stoi(argv[3]) : DVRLite::DVRLite::Config::Port;

    bool isService = argc >= 5 ? std::string("service") == argv[4] : false;

    DVRLite::DVRLite dvrlite = DVRLite::DVRLite(configPath, webPath, port, isService);
    DVRLite::MediaController::run(&dvrlite);

    return 0;
}

namespace DVRLite
{
    using namespace Logger;

    DVRLite::DVRLite(const std::string& configPath, const std::string& webPath, uint16_t port, bool isService) : config(configPath, webPath, port, isService), cache(100000)
    {
        Logger::Init(config.GetLogPath());
        config.Load();

        //load the sources
        if (std::filesystem::is_directory(config.GetSourcePath()))
        {
            for (std::filesystem::directory_entry directory : std::filesystem::directory_iterator(config.GetSourcePath()))
            {
                std::filesystem::path configPath = directory.path() / "config.json";
                if (directory.is_directory() && std::filesystem::is_regular_file(configPath))
                {
                    std::pair<std::unordered_set<Source>::iterator, bool> result = sources.insert(Source(configPath));
                    const Source& source = *(result.first);
                    ffmpegs[source.GetName()] = std::make_unique<FFmpeg>(source, this);
                }
            }
        }
        //get the onvif info
        onvif.Init(this);
    }

    void DVRLite::AddSource(const Source& source)
    {
        Log(LogFilter::DVRLite, "Adding source " + source.GetUsername());
        std::pair<SourceSet::iterator, bool> result = sources.insert(source);
        if (!result.second)
        {
            sources.erase(result.first);
            result = sources.insert(source);
        }

        const Source& ourSource = *(result.first);
        ffmpegs[source.GetName()] = std::make_unique<FFmpeg>(ourSource, this);
        onvif.Add(ourSource);
        ourSource.Save(std::filesystem::path(config.GetSourcePath()) / source.GetName() / std::string("config.json"));
    }

    void DVRLite::RemoveSource(const std::string& sourceName)
    {
        Log(LogFilter::DVRLite, "Removing source " + sourceName);
        ffmpegs[sourceName].reset();
        onvif.Remove(sourceName);
        sources.erase(std::find_if(sources.begin(), sources.end(), [&](const Source& source) {return source.GetName() == sourceName; }));
    }

    const Source& DVRLite::GetSource(const std::string& sourceName) const
    {
        return *std::find_if(sources.begin(), sources.end(), [&](const Source& source) {return source.GetName() == sourceName; });
    }

    JsonCache& DVRLite::GetCache()
    {
        return cache;
    }

    uintmax_t DVRLite::GetVideoDirectorySize()
    {
        std::filesystem::path videoDirectory = GetConfig().GetRecordPath();
        std::filesystem::file_time_type newVideoDirectoryWriteTime = std::filesystem::last_write_time(videoDirectory);
        if (newVideoDirectoryWriteTime > videoDirectoryWriteTime)
        {
            videoDirectoryWriteTime = newVideoDirectoryWriteTime;
            videoDirectorySize = 0;
            for (const std::filesystem::directory_entry& directory_entry : std::filesystem::recursive_directory_iterator(videoDirectory))
            {
                if (std::filesystem::is_regular_file(directory_entry))
                    videoDirectorySize += directory_entry.file_size();
            }
        }
        Log(LogFilter::DVRLite, "Video Directory Size: " + std::to_string(videoDirectorySize) + " Modified Time: " + std::to_string(videoDirectoryWriteTime.time_since_epoch().count()));
        return videoDirectorySize;
    }

    DVRLite::Config& DVRLite::GetConfig()
    {
        return config;
    }

    const DVRLite::Config& DVRLite::GetConfig() const
    {
        return config;
    }

    DVRLite::Config::Config(const std::string& configPath, const std::string& webPath, uint16_t port, bool isService) :
        configPath(configPath),
        webPath(webPath),
        isService(isService)
    {
        Port = port;
    }

    void DVRLite::Config::Load()
    {
        if (std::filesystem::is_regular_file(configPath))
        {
            std::ifstream file(configPath);
            if (file.is_open())
            {
                Json::Value config;
                file >> config;

                std::lock_guard lock(configMutex);
                std::string newRecordPath = config["recordPath"].asString();
                if (!newRecordPath.empty())
                    recordPath = newRecordPath;
                uint16_t port = config["port"].asUInt();
                if (port != 0)
                    Port = port;
                std::string newTheme = config["theme"].asString();
                if (newTheme.empty())
                    theme = "default";
                else theme = newTheme;
                logFilter = static_cast<LogFilter>(config["logFilter"].asUInt());
                Filter(logFilter);

                std::stringstream jsonString;
                jsonString << config;
                Log(LogFilter::DVRLite, "Config Loaded:\n " + jsonString.str());
            }
        }
    }

    void DVRLite::Config::Save() const
    {
        Json::Value source;
        {
            std::lock_guard lock(configMutex);
            source["recordPath"] = recordPath;
            source["theme"] = theme;
            source["port"] = Port;
            source["logFilter"] = static_cast<uint16_t>(logFilter);
        }
        std::filesystem::path fullpath(configPath);
        std::filesystem::create_directories(fullpath.parent_path());
        std::ofstream file(configPath);
        file << source;

        std::stringstream jsonString;
        jsonString << source;
        Log(LogFilter::DVRLite, "Config Saved:\n " + jsonString.str());
    }

    void DVRLite::Config::SetRecordPath(const std::string& recordPath)
    {
        std::lock_guard lock(configMutex);
        this->recordPath = recordPath;
    }

    std::string DVRLite::Config::GetRecordPath() const
    {
        std::lock_guard lock(configMutex);
        return recordPath;
    }

    std::string DVRLite::Config::GetWebPath() const
    {
        std::lock_guard lock(configMutex);
        return webPath;
    }

    std::string DVRLite::Config::GetSourcePath() const
    {
        std::lock_guard lock(configMutex);
        return (std::filesystem::path(configPath).parent_path() / "sources").string();
    }

    std::string DVRLite::Config::GetLogPath() const
    {
        std::lock_guard lock(configMutex);
        return (std::filesystem::path(configPath).parent_path() / "log.txt").string();
    }

    std::string DVRLite::Config::GetTheme() const
    {
        std::lock_guard lock(configMutex);
        return theme;
    }

    Logger::LogFilter DVRLite::Config::GetLogFilter() const
    {
        std::lock_guard lock(configMutex);
        return logFilter;
    }

    uint16_t DVRLite::Config::GetPort() const
    {
        std::lock_guard lock(configMutex);
        return Port;
    }

    bool DVRLite::Config::IsService()const
    {
        return isService;
    }

    void DVRLite::Config::SetTheme(const std::string &theme)
    {
        std::lock_guard lock(configMutex);
        this->theme = theme;
    }

    void DVRLite::Config::SetPort(uint16_t port)
    {
        std::lock_guard lock(configMutex);
        this->Port = port;
    }

    void DVRLite::Config::SetLogFilter(Logger::LogFilter filter)
    {
        std::lock_guard lock(configMutex);
        this->logFilter = filter;
        Filter(filter);
    }
}