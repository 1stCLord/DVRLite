#include "FFmpeg.h"
#include "Source.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
}
#include "DVRLite.h"
#include <json/json.h>
#include <fstream>
#include "Web/Utils.hpp"

FFmpeg::FFmpeg(const Source &source) : 
    source(source),
    isRecording(false)
{

}

FFmpeg::~FFmpeg()
{
    //while (isRecording)std::this_thread::sleep_until(endTime);
    if (recordThread.joinable())
        recordThread.join();
}

void FFmpeg::Record(std::chrono::seconds duration, const std::filesystem::path& path)
{
    std::lock_guard lock(endMutex);
    if (!isRecording)
    {
        DVRLite::Log("Begin Record " + source.GetName());
        if(recordThread.joinable())
            recordThread.join();
        isRecording = true;
        endTime = std::chrono::system_clock::now() + duration;
        recordThread = std::thread(&FFmpeg::RecordThread, this, path);
    }
    else
    {
        DVRLite::Log("Extend Record " + source.GetName());
        endTime = std::chrono::system_clock::now() + duration;
    }
}

bool FFmpeg::IsRecording() const
{
    return isRecording;
}

bool FFmpeg::InitialiseRecordThread(const char* in_filename, const char* out_filename, RecordThreadContext &context) const
{
    avformat_open_input(&context.ifmt_ctx, in_filename, NULL, NULL);
    if (avformat_find_stream_info(context.ifmt_ctx, 0) < 0)
    {
        DVRLite::Log("Retrieve input stream info failed.");
        return false;
    }

    av_dump_format(context.ifmt_ctx, 0, in_filename, 0);

    // Output
    avformat_alloc_output_context2(&context.ofmt_ctx, NULL, NULL, out_filename);
    if (!context.ofmt_ctx)
    {
        DVRLite::Log("Create output context failed.");
        return false;
    }

    context.ofmt = context.ofmt_ctx->oformat;

    uint16_t stream_index = 0;
    for (uint32_t i = 0; i < context.ifmt_ctx->nb_streams; ++i)
    {
        // Create output AVStream according to input AVStream
        if (context.ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && context.video_stream_map.out == NULL && source.GetRecordVideo())
        {
            if (!MapInput(context, context.video_stream_map, context.ifmt_ctx->streams[i]))
            {
                return false;
            }
            context.video_stream_map.streamIndex = stream_index++;
        }
        else if (context.ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && context.audio_stream_map.out == NULL && source.GetRecordAudio())
        {
            if (!MapInput(context, context.audio_stream_map, context.ifmt_ctx->streams[i]))
            {
                return false;
            }
            context.audio_stream_map.streamIndex = stream_index++;
        }
        else context.ifmt_ctx->streams[i]->discard = AVDISCARD_ALL;
        /*if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
        }*/
    }

    // Output format
    av_dump_format(context.ofmt_ctx, 0, out_filename, 1);

    // Open output file
    if (!(context.ofmt->flags & AVFMT_NOFILE))
    {
        if (avio_open(&context.ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE) < 0)
        {
            DVRLite::Log(std::string("Open output file ") + out_filename + " failed.");
            return false;
        }
    }

    // Write file header
    if (avformat_write_header(context.ofmt_ctx, NULL) < 0)
    {
        DVRLite::Log("Error occurred when opening output file.");
        return false;
    }
    return true;
}

void FFmpeg::WriteMetadata(const std::filesystem::path &path, std::chrono::system_clock::time_point startTime, std::chrono::system_clock::time_point endTime) const
{
    Json::Value json;
    json["startTime"] = to_string(startTime, "%Y-%m-%d %H:%M:%S");
    json["endTime"] = to_string(endTime, "%Y-%m-%d %H:%M:%S");
    std::ofstream file(path);
    if(file.is_open())
        file << json;
}

bool FFmpeg::MapInput(RecordThreadContext& context, StreamMap& streamMap, AVStream *stream)
{
    streamMap.in = stream;
    streamMap.out = avformat_new_stream(context.ofmt_ctx, streamMap.in->codec->codec);

    if (!streamMap.out)
    {
        DVRLite::Log("Allocate output stream failed.");
        return false;
    }

    // Copy the settings of AVCodecContext
    if (avcodec_copy_context(streamMap.out->codec, streamMap.in->codec) < 0)
    {
        DVRLite::Log("Failed to copy context from input to output stream codec context.");
        return false;
    }

    streamMap.out->codecpar->codec_tag = 0;
    return true;
}

const StreamMap *FFmpeg::GetStreamMap(const RecordThreadContext &context, int stream_index) const
{
    if (context.ifmt_ctx->streams[stream_index] == context.video_stream_map.in && source.GetRecordVideo())
    {
        return &context.video_stream_map;
    }
    else if (context.ifmt_ctx->streams[stream_index] == context.audio_stream_map.in && source.GetRecordAudio())
    {
        return &context.audio_stream_map;
    }
    return nullptr;
}

void FFmpeg::RecordThread(const std::filesystem::path& path)
{
    DVRLite::Log("RecordThread Begin " + source.GetName());
    std::string out_path = path.string();
    const char* out_filename = out_path.c_str();
    std::string videoAddress = source.GetVideoAddress();
    const char* in_filename = videoAddress.c_str();

    RecordThreadContext context;
    if (!InitialiseRecordThread(in_filename, out_filename, context))
    {
        DVRLite::Log("Initialise Record Failed " + source.GetName());
        isRecording = false;
        return;
    }

    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point actualEndTime;
    int frame_index = 0;
    AVPacket pkt;
    while (true) 
    {
        // Get an AVPacket
        if (av_read_frame(context.ifmt_ctx, &pkt) < 0) 
        {
            DVRLite::Log("Record Bad Read");
            //isRecording = false;
            continue;
        }

        const StreamMap *current_stream_map = GetStreamMap(context, pkt.stream_index);
        if (current_stream_map != nullptr)
        {
            pkt.stream_index = current_stream_map->streamIndex;

            /* copy packet */
            // Convert PTS / DTS
            pkt.pts = av_rescale_q_rnd(pkt.pts, current_stream_map->in->time_base, current_stream_map->out->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.dts = av_rescale_q_rnd(pkt.dts, current_stream_map->in->time_base, current_stream_map->out->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
            pkt.duration = av_rescale_q(pkt.duration, current_stream_map->in->time_base, current_stream_map->out->time_base);
            pkt.pos = -1;

            // Write
            if (av_interleaved_write_frame(context.ofmt_ctx, &pkt) < 0)
            {
                DVRLite::Log("Error muxing packet.");
                continue;
            }
            //DVRLite::Log(std::string("Write ") + std::to_string(frame_index) + " frames to output file\n");
            av_free_packet(&pkt);
            frame_index++;
        }

        std::lock_guard lock(endMutex);
        if (std::chrono::system_clock::now() >= endTime)
        {
            actualEndTime = endTime;
            isRecording = false;
            break;
        }
    }
    EndRecordThread(context);
    WriteMetadata(std::filesystem::path(path).replace_extension(".json"), startTime, actualEndTime);

    return;
}

void FFmpeg::EndRecordThread(RecordThreadContext& context) const
{
    DVRLite::Log("End record" + source.GetName());
    // Write file trailer
    av_write_trailer(context.ofmt_ctx);

    avformat_close_input(&context.ifmt_ctx);

    /* close output */
    if (context.ofmt_ctx && !(context.ofmt->flags & AVFMT_NOFILE))
    {
        avio_close(context.ofmt_ctx->pb);
    }

    avformat_free_context(context.ofmt_ctx);


}