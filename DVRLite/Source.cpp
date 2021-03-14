#include "Source.h"
#include <json/json.h>
#include <iostream>
#include <fstream>
#include <functional>
#include "Web/Utils.hpp"
#include "DVRLite.h"
namespace DVRLite
{
    using namespace Logger;

    Source::Source(const QueryParams& queryParams)
    {
        using String = oatpp::data::mapping::type::String;
        using StringKeyLabel = oatpp::data::share::StringKeyLabel;
        using MatchMapPair = std::pair<StringKeyLabel, StringKeyLabel>;

        name = queryParams.get("name")->std_str();
        onvifAddress = queryParams.get("onvifAddress")->std_str();
        onvifAddress = createFullOnvifPath(unescapeUrl(onvifAddress));
        customVideoAddress = queryParams.get("customVideoAddress")->std_str();
        customVideoAddress = unescapeUrl(customVideoAddress);

        username = queryParams.get("username")->std_str();
        password = queryParams.get("password")->std_str();

        std::string variable = std::string("self_trigger");
        String value = queryParams.get(variable.c_str());
        if (value && value->std_str() == "on")
            triggers.insert(name);
        for (const MatchMapPair& queryParam : queryParams.getAll())
        {
            std::string start = "trigger_";
            std::string query = queryParam.first.std_str();
            std::string value = queryParam.second.std_str();
            if (query.substr(0, start.size()) == start)
            {
                if (value == "on")
                    triggers.insert(query.substr(start.size()));
            }
        }

        eventFilter = queryParams.get("eventFilter")->std_str();
        std::string durationString = queryParams.get("duration")->std_str();
        duration = durationString.empty() ? 0 : std::stoi(durationString);
        std::string quotaString = queryParams.get("quota")->std_str();
        quota = quotaString.empty() ? 0 : std::stoi(quotaString);
        String recordAudioString = queryParams.get("recordAudio");
        recordAudio = recordAudioString == "on";
        String recordVideoString = queryParams.get("recordVideo");
        recordVideo = recordVideoString == "on";

        std::stringstream jsonString;
        Log(filter, "Source Created:\n");
    }

    void Source::Save(const std::filesystem::path& path) const
    {
        Json::Value source;
        {
            std::lock_guard lock(sourceMutex);
            source["name"] = name;
            source["onvifAddress"] = onvifAddress;
            source["customVideoAddress"] = customVideoAddress;
            source["username"] = username;
            source["password"] = password;
            source["triggers"] = Json::Value(Json::arrayValue);
            for (std::string trigger : triggers)
                source["triggers"].append(trigger);
            source["eventFilter"] = eventFilter;
            source["duration"] = duration;
            source["quota"] = quota;
            source["recordAudio"] = recordAudio;
            source["recordVideo"] = recordVideo;
        }

        std::filesystem::path fullpath(path);
        std::filesystem::create_directories(fullpath.parent_path());
        std::ofstream file(path);
        file << source;

        std::stringstream jsonString;
        jsonString << source;
        Log(filter, "Source Saved:\n " + jsonString.str());
    }

    void Source::Load(const std::filesystem::path& path)
    {
        std::ifstream file(path);
        if (file.is_open())
        {
            Json::Value source;
            file >> source;

            std::lock_guard lock(sourceMutex);
            name = source["name"].asString();
            onvifAddress = source["onvifAddress"].asString();
            customVideoAddress = source["customVideoAddress"].asString();
            username = source["username"].asString();
            password = source["password"].asString();
            for (Json::Value& triggerValue : source["triggers"])
                triggers.insert(triggerValue.asString());
            eventFilter = source["eventFilter"].asString();
            duration = source["duration"].asUInt();
            quota = source["quota"].asUInt();
            recordAudio = source["recordAudio"].asBool();
            recordVideo = source["recordVideo"].asBool();

            std::stringstream jsonString;
            jsonString << source;
            Log(filter, "Config Loaded:\n " + jsonString.str());
        }
    }

    std::string Source::GetName() const
    {
        std::lock_guard lock(sourceMutex);
        return name;
    }

    std::string Source::GetOnvifAddress()const
    {
        std::lock_guard lock(sourceMutex);
        return onvifAddress;
    }

    std::string Source::GetVideoAddress() const
    {
        std::lock_guard lock(sourceMutex);
        return customVideoAddress.empty() ? onvifVideoAddress : customVideoAddress;
    }

    std::string Source::GetSnapshotAddress() const
    {
        std::lock_guard lock(sourceMutex);
        return onvifSnapshotAddress;
    }

    std::string Source::GetAuthSnapshotAddress() const
    {
        std::lock_guard lock(sourceMutex);
        return replace_substring(onvifSnapshotAddress, "://", "://" + username + ':' + password + '@');
    }

    std::string Source::GetUsername()const
    {
        std::lock_guard lock(sourceMutex);
        return username;
    }

    std::string Source::GetPassword()const
    {
        std::lock_guard lock(sourceMutex);
        return password;
    }

    std::string Source::GetEventFilter()const
    {
        std::lock_guard lock(sourceMutex);
        return eventFilter;
    }

    uint16_t Source::GetDuration()const
    {
        std::lock_guard lock(sourceMutex);
        return duration;
    }

    uint16_t Source::GetQuota()const
    {
        std::lock_guard lock(sourceMutex);
        return quota;
    }

    bool Source::GetRecordVideo() const
    {
        std::lock_guard lock(sourceMutex);
        return recordVideo;
    }

    bool Source::GetRecordAudio() const
    {
        std::lock_guard lock(sourceMutex);
        return recordAudio;
    }

    void Source::SetOnvifVideoAddress(std::string& address)
    {
        std::lock_guard lock(sourceMutex);
        onvifVideoAddress = address;
    }

    void Source::SetOnvifSnapshotAddress(std::string& address)
    {
        std::lock_guard lock(sourceMutex);
        onvifSnapshotAddress = address;
    }

    std::unordered_set<std::string> Source::GetTriggers() const
    {
        std::lock_guard lock(sourceMutex);
        return triggers;
    }

    size_t Source::Hash::operator() (const Source& source) const
    {
        return std::hash<std::string>{}(source.GetName());
    }
}