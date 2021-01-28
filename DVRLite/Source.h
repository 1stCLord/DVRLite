#pragma once
#include <unordered_set>
#include <filesystem>
#include "oatpp/web/server/api/ApiController.hpp"

using QueryParams = oatpp::web::protocol::http::QueryParams;

class Source
{
private:
	std::string name;
	std::string onvifAddress;
	std::string onvifVideoAddress;
	std::string onvifSnapshotAddress;
	std::string customVideoAddress;
	std::string username;
	std::string password;
	uint16_t duration;
	uint16_t quota;
	bool recordAudio;
	bool recordVideo;
	std::unordered_set<std::string> triggers;

	mutable std::mutex sourceMutex;

public:
	Source() : duration(0), quota(0), recordAudio(false), recordVideo(false) {}
	Source(const std::string& name, const std::string& onvifAddress, uint16_t duration, uint16_t quota, const std::unordered_set<std::string>& triggers) :
		name(name),
		onvifAddress(onvifAddress),
		customVideoAddress(),
		duration(duration),
		quota(quota),
		recordAudio(true),
		recordVideo(true),
		triggers(triggers)
	{
	}
	Source(const std::filesystem::path& path) { Load(path); }
	Source(const QueryParams& queryParams);
	Source(const Source& other) :
		name(other.name),
		onvifAddress(other.onvifAddress),
		onvifVideoAddress(other.onvifVideoAddress),
		onvifSnapshotAddress(other.onvifSnapshotAddress),
		customVideoAddress(other.customVideoAddress),
		username(other.username),
		password(other.password),
		duration(other.duration),
		quota(other.quota),
		recordAudio(other.recordAudio),
		recordVideo(other.recordVideo),
		triggers(other.triggers)
	{}

	bool operator==(const Source& other) const { return name == other.name; }

	void Save(const std::filesystem::path& path) const;
	void Load(const std::filesystem::path& path);

	void SetOnvifVideoAddress(std::string& address);
	void SetOnvifSnapshotAddress(std::string& address);

	std::string GetName() const;
	std::string GetOnvifAddress()const;
	std::string GetVideoAddress()const;
	std::string GetSnapshotAddress()const;
	std::string GetAuthSnapshotAddress()const;
	std::string GetUsername()const;
	std::string GetPassword()const;
	uint16_t GetDuration()const;
	uint16_t GetQuota()const;
	bool GetRecordVideo() const;
	bool GetRecordAudio() const;
	std::unordered_set<std::string> GetTriggers() const;

	struct Hash { size_t operator() (const Source& source) const; };
};
