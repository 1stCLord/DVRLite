#pragma once

#include <string>
#include <chrono>
#include <filesystem>
#include <thread>
#include <mutex>
#include <atomic>

struct AVStream;
struct AVFormatContext;
struct AVOutputFormat;
class Source;

struct StreamMap
{
	AVStream* in = nullptr;
	AVStream* out = nullptr;
	int streamIndex = -1;
};

struct RecordThreadContext
{
	AVFormatContext* ifmt_ctx = nullptr;
	AVFormatContext* ofmt_ctx = nullptr;
	AVOutputFormat* ofmt = nullptr;
	StreamMap video_stream_map;
	StreamMap audio_stream_map;
};


class FFmpeg
{
public:
	FFmpeg(const Source &source);
	FFmpeg(const FFmpeg&) = delete;
	FFmpeg(FFmpeg&&) = delete;
	FFmpeg& operator=(const FFmpeg&) = delete;
	FFmpeg& operator=(FFmpeg&&) = delete;
	~FFmpeg();
	void Record(std::chrono::seconds duration, const std::filesystem::path &path);
	bool IsRecording() const;
private:
	const Source& source;

	std::thread recordThread;
	std::mutex endMutex;
	std::atomic_bool isRecording;
	std::chrono::time_point<std::chrono::system_clock> endTime;

	bool InitialiseRecordThread(const char * in_filename, const char* out_filename, RecordThreadContext& context) const;
	void EndRecordThread(RecordThreadContext& context) const;
	const StreamMap *GetStreamMap(const RecordThreadContext& context, int stream_index) const;
	static bool MapInput(RecordThreadContext& context, StreamMap& streamMap, AVStream* stream);
	void RecordThread(const std::filesystem::path& path);
};