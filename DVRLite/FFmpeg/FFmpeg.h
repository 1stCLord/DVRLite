#pragma once

#include <string>
#include <chrono>
#include <filesystem>
#include <thread>
#include <mutex>
#include <atomic>
#include "Log.h"

struct AVStream;
struct AVFormatContext;
struct AVOutputFormat;

namespace DVRLite
{
	class Source;
	class DVRLite;

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
		FFmpeg(const Source& source, DVRLite* dvrlite);
		FFmpeg(const FFmpeg&) = delete;
		FFmpeg(FFmpeg&&) = delete;
		FFmpeg& operator=(const FFmpeg&) = delete;
		FFmpeg& operator=(FFmpeg&&) = delete;
		~FFmpeg();
		void Record(std::chrono::seconds duration, const std::filesystem::path& path);
		bool IsRecording() const;
	private:
		DVRLite* const dvrlite;
		const Source& source;

		std::thread recordThread;
		std::mutex endMutex;
		std::atomic_bool isRecording;
		std::chrono::system_clock::time_point endTime;

		static const Logger::LogFilter filter = Logger::LogFilter::FFmpeg;

		bool InitialiseRecordThread(const char* in_filename, const char* out_filename, RecordThreadContext& context) const;
		void EndRecordThread(RecordThreadContext& context) const;
		const StreamMap* GetStreamMap(const RecordThreadContext& context, int stream_index) const;
		static bool MapInput(RecordThreadContext& context, StreamMap& streamMap, AVStream* stream);
		void RecordThread(const std::filesystem::path& path);
		void WriteMetadata(const std::filesystem::path& path, std::chrono::system_clock::time_point startTime, std::chrono::system_clock::time_point endTime) const;
	};
}