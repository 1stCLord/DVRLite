#include "FFmpeg.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
}
#define LOG(...) printf(__VA_ARGS__)

FFmpeg::FFmpeg(const std::string& url, bool recordAudio, bool recordVideo) : 
    url(url),
    recordAudio(recordAudio),
    recordVideo(recordVideo),
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
        if(recordThread.joinable())
            recordThread.join();
        isRecording = true;
        endTime = std::chrono::system_clock::now() + duration;
        recordThread = std::thread(&FFmpeg::RecordThread, this, path);
    }
    else
        endTime = std::chrono::system_clock::now() + duration;
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
        LOG("Retrieve input stream info failed.");
        return false;
    }

    av_dump_format(context.ifmt_ctx, 0, in_filename, 0);

    // Output
    avformat_alloc_output_context2(&context.ofmt_ctx, NULL, NULL, out_filename);
    if (!context.ofmt_ctx)
    {
        LOG("Create output context failed.");
        return false;
    }

    context.ofmt = context.ofmt_ctx->oformat;

    uint16_t stream_index = 0;
    for (uint32_t i = 0; i < context.ifmt_ctx->nb_streams; ++i)
    {
        // Create output AVStream according to input AVStream
        if (context.ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && context.video_stream_map.out == NULL && recordVideo)
        {
            if (!MapInput(context, context.video_stream_map, context.ifmt_ctx->streams[i]))
            {
                return false;
            }
            context.video_stream_map.streamIndex = stream_index++;
        }
        else if (context.ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO && context.audio_stream_map.out == NULL && recordAudio)
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
            LOG("Open output file '%s' failed.", out_filename);
            return false;
        }
    }

    // Write file header
    if (avformat_write_header(context.ofmt_ctx, NULL) < 0)
    {
        LOG("Error occurred when opening output file.");
        return false;
    }
    return true;
}

bool FFmpeg::MapInput(RecordThreadContext& context, StreamMap& streamMap, AVStream *stream)
{
    streamMap.in = stream;
    streamMap.out = avformat_new_stream(context.ofmt_ctx, streamMap.in->codec->codec);

    if (!streamMap.out)
    {
        LOG("Allocate output stream failed.");
        return false;
    }

    // Copy the settings of AVCodecContext
    if (avcodec_copy_context(streamMap.out->codec, streamMap.in->codec) < 0)
    {
        LOG("Failed to copy context from input to output stream codec context.");
        return false;
    }

    streamMap.out->codecpar->codec_tag = 0;
    return true;
}

const StreamMap *FFmpeg::GetStreamMap(const RecordThreadContext &context, int stream_index) const
{
    if (context.ifmt_ctx->streams[stream_index] == context.video_stream_map.in && recordVideo)
    {
        return &context.video_stream_map;
    }
    else if (context.ifmt_ctx->streams[stream_index] == context.audio_stream_map.in && recordAudio)
    {
        return &context.audio_stream_map;
    }
    return nullptr;
}

void FFmpeg::RecordThread(const std::filesystem::path& path)
{
    std::string out_path = path.string();
    const char* out_filename = out_path.c_str();
    const char* in_filename = url.c_str();

    RecordThreadContext context;
    if (!InitialiseRecordThread(in_filename, out_filename, context))
    {
        isRecording = false;
        return;
    }

    int frame_index = 0;
    AVPacket pkt;
    while (true) 
    {
        // Get an AVPacket
        if (av_read_frame(context.ifmt_ctx, &pkt) < 0) 
        {
            isRecording = false;
            break;
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
                LOG("Error muxing packet.");
                isRecording = false;
                break;
            }
            LOG("Write %8d frames to output file\n", frame_index);
            av_free_packet(&pkt);
            frame_index++;
        }

        std::lock_guard lock(endMutex);
        if (std::chrono::system_clock::now() >= endTime)
        {
            isRecording = false;
            break;
        }
    }
    EndRecordThread(context);

    return;
}

void FFmpeg::EndRecordThread(RecordThreadContext& context) const
{
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