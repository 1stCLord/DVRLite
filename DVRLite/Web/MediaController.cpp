
#include "MediaController.hpp"
#include "AppComponent.hpp"
#include <fstream>
#include "DVRLite.h"

namespace DVRLite
{
    using namespace Logger;

    std::shared_ptr<oatpp::data::stream::IOStream> MediaController::authconnection;

    std::shared_ptr<MediaController::OutgoingResponse> MediaController::getStaticFileResponse(const oatpp::String& filename,
        const oatpp::String& rangeHeader) const
    {

        auto file = staticFileManager->getFile(filename);

        OATPP_ASSERT_HTTP(file.get() != nullptr, Status::CODE_404, "File not found");

        std::shared_ptr<OutgoingResponse> response;

        if (!rangeHeader) {
            response = getFullFileResponse(file);
        }
        else {
            response = getRangeResponse(rangeHeader, file);
        }

        response->putHeader("Accept-Ranges", "bytes");
        response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);
        auto mimeType = staticFileManager->guessMimeType(filename);
        if (mimeType) {
            response->putHeader(Header::CONTENT_TYPE, mimeType);
        }
        else {
            OATPP_LOGD("Server", "Unknown Mime-Type. Header not set");
        }

        return response;

    }

    std::shared_ptr<MediaController::OutgoingResponse> MediaController::getFullFileResponse(const oatpp::String& file) const
    {
        return createResponse(Status::CODE_200, file);
    }

    std::shared_ptr<MediaController::OutgoingResponse> MediaController::getRangeResponse(const oatpp::String& rangeStr,
        const oatpp::String& file) const
    {

        auto range = oatpp::web::protocol::http::Range::parse(rangeStr.getPtr());

        if (range.end == 0) {
            range.end = file->getSize() - 1;
        }

        OATPP_ASSERT_HTTP(range.isValid() &&
            range.start < file->getSize() &&
            range.end > range.start &&
            range.end < file->getSize(), Status::CODE_416, "Range is invalid");

        auto chunk = oatpp::String((const char*)&file->getData()[range.start], (v_int32)(range.end - range.start + 1), false);

        auto response = createResponse(Status::CODE_206, chunk);

        oatpp::web::protocol::http::ContentRange contentRange(oatpp::web::protocol::http::ContentRange::UNIT_BYTES,
            range.start, range.end, file->getSize(), true);

        OATPP_LOGD("Server", "range=%s", contentRange.toString()->c_str());

        response->putHeader(Header::CONTENT_RANGE, contentRange.toString());
        return response;

    }

    std::string MediaController::CreateHTMLHeader(const std::string& pageTitle) const
    {
        std::string scripts = "<script type=\"text/javascript\" src=\"" + std::string("main.js")     + "\"></script>";
        if(pageTitle.starts_with("snapshot "))
                   scripts += "<script type=\"text/javascript\" src=\"" + std::string("snapshot.js") + "\"></script>";
        if (pageTitle.starts_with("videolist "))
            scripts += "<script type=\"text/javascript\" src=\"https://unpkg.com/vis-timeline@latest/standalone/umd/vis-timeline-graph2d.min.js\"></script><link href=\"https://unpkg.com/vis-timeline@latest/styles/vis-timeline-graph2d.min.css\" rel=\"stylesheet\" type=\"text/css\" />";
        return ApplyTemplate("htmlheader", scripts);
    }

    std::string MediaController::CreateHeader(const std::string& pageTitle) const
    {
        std::string addString = ApplyTemplate("headermenuitem", { "Add", "add.html", (const char*)u8"➕" });
        std::string configString = ApplyTemplate("headerdropdownitem", { "Config", "config.html", (const char*)u8"⚙️" });
        std::string logString = ApplyTemplate("headerdropdownitem", { "Log", "log.html", (const char*)u8"📋" });
        std::string shutdownString = ApplyTemplate("headerdropdownitem", { "Shutdown", "shutdown", (const char*)u8"🛑" });
        std::string restartString = ApplyTemplate("headerdropdownitem", { "Restart", "restart", (const char*)u8"♻️" });

        std::string headerDropdown = ApplyTemplate("headerdropdown", { addString, configString, logString, shutdownString, dvrlite->GetConfig().IsService() ? restartString : "" });
        return templates["headertop"].asString() + ApplyTemplate("headertitle", { pageTitle, headerDropdown });
    }

    std::string MediaController::CreateSourceList() const
    {
        std::string sourcelist;
        sourcelist += templates["sourceheader"].asString();
        for (const Source& source : dvrlite->GetSources())
        {
            std::string sourcedata;
            sourcedata += ApplyTemplate("sourcesnapshotlink", source.GetName());
            sourcedata += ApplyTemplate("sourcedata", source.GetOnvifAddress());
            /*sourcedata += ApplyTemplate("sourcedata", source.GetVideoAddress());
            sourcedata += ApplyTemplate("sourcedata", std::to_string(source.GetDuration()));
            sourcedata += ApplyTemplate("sourcedata", std::to_string(source.GetQuota()));*/
            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::chrono::system_clock::time_point hour = now - std::chrono::hours(1);
            std::chrono::system_clock::time_point day = now - std::chrono::hours(24);
            std::chrono::system_clock::time_point month = now - std::chrono::hours(24 * 30);
            std::vector<std::string> hourSourceLinkParameters{ std::to_string(VideosBetweenDates(source, hour, now)), source.GetName(), to_string(hour, DATESTRINGFORMATQUOTES), to_string(now, DATESTRINGFORMATQUOTES) };
            sourcedata += ApplyTemplate("sourcelink", hourSourceLinkParameters);
            std::vector<std::string> daySourceLinkParameters{ std::to_string(VideosBetweenDates(source, day, now)), source.GetName(), to_string(day, DATESTRINGFORMATQUOTES), to_string(now, DATESTRINGFORMATQUOTES) };
            sourcedata += ApplyTemplate("sourcelink", daySourceLinkParameters);
            std::vector<std::string> monthSourceLinkParameters{ std::to_string(VideosBetweenDates(source, month, now)), source.GetName(), to_string(month, DATESTRINGFORMATQUOTES), to_string(now, DATESTRINGFORMATQUOTES) };
            sourcedata += ApplyTemplate("sourcelink", monthSourceLinkParameters);
            std::string triggers;
            for (const std::string& trigger : source.GetTriggers())
                triggers += trigger + "</br>";
            sourcedata += ApplyTemplate("sourcedata", triggers);
            sourcedata += ApplyTemplate("sourcetoolelement", source.GetName());

            sourcelist += ApplyTemplate("sourcerecord", sourcedata);
        }
        sourcelist = ApplyTemplate("sourcetable", sourcelist);
        return sourcelist;
    }

    std::string MediaController::CreateVideoList(const Source& source) const
    {
        std::string videolist;
        std::filesystem::path videoDirectory = dvrlite->GetConfig().GetRecordPath();
        videoDirectory = videoDirectory / source.GetName();
        if (std::filesystem::is_directory(videoDirectory))
        {
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(videoDirectory))
            {
                if (entry.path().extension() == ".mp4")
                {
                    std::vector<std::string> videoParameters{ entry.path().filename().string(), (std::filesystem::path("videos") / source.GetName() / entry.path().filename()).string() };
                    videolist += ApplyTemplate("videoelement", videoParameters);
                }
            }
        }
        return videolist;
    }

    std::string MediaController::CreateVideoSnapshot(const Source& source) const
    {
        return ApplyTemplate("videosnapshot", source.GetAuthSnapshotAddress());
    }

    uint32_t MediaController::VideosBetweenDates(const Source& source, std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to) const
    {
        std::filesystem::path videoDirectory = dvrlite->GetConfig().GetRecordPath();
        videoDirectory = videoDirectory / source.GetName();
        JsonCache& cache = dvrlite->GetCache();
        cache.Preload(videoDirectory);

        //DVRLite::Log("VideosBetweenDates - source " + source.GetName() + " listing videos between " + to_string(from, DATESTRINGFORMATQUOTES) + " & " + to_string(to, DATESTRINGFORMATQUOTES));
        Log(filter, "VideosBetweenDates - source " + source.GetName() + " listing videos between " + to_string(from, DATESTRINGFORMATQUOTES) + std::to_string(from.time_since_epoch().count()) + " & " + to_string(to, DATESTRINGFORMATQUOTES) + std::to_string(to.time_since_epoch().count()));
        int i = 0;
        if (std::filesystem::is_directory(videoDirectory))
        {
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(videoDirectory))
            {
                if (entry.path().extension() == ".json")
                {
                    Json::Value* jsonptr = cache.Get(entry.path().string());
                    if (jsonptr != nullptr)
                    {
                        Json::Value& json = *jsonptr;
                        std::chrono::system_clock::time_point startTimepoint = to_timepoint(json["startTime"].asString(), DATESTRINGFORMAT);
                        std::chrono::system_clock::time_point endTimepoint = to_timepoint(json["endTime"].asString(), DATESTRINGFORMAT);
                        //DVRLite::Log("VideosBetweenDates - source " + source.GetName() + " compare " + json["startTime"].asString() + " & " + json["endTime"].asString());
                        if (endTimepoint > from && startTimepoint < to)
                        {
                            ++i;
                            Log(filter, "    VideosBetweenDates - source " + source.GetName() + " matched " + json["startTime"].asString() + std::to_string(startTimepoint.time_since_epoch().count()) + " & " + json["endTime"].asString() + "(" + std::to_string(endTimepoint.time_since_epoch().count()) + ")");
                        }
                        else
                            Log(filter, "    VideosBetweenDates - source " + source.GetName() + " didn't match " + json["startTime"].asString() + std::to_string(startTimepoint.time_since_epoch().count()) + " & " + json["endTime"].asString() + "(" + std::to_string(endTimepoint.time_since_epoch().count()) + ")");
                    }
                }
            }
        }
        Log(filter, "VideosBetweenDates - source " + source.GetName() + " found " + std::to_string(i));
        return i;
    }

    std::string MediaController::CreateDatePicker(const std::string& label, std::chrono::system_clock::time_point date, const std::string &id) const
    {
        return ApplyTemplate("videotimelinedatepicker", { label, to_string(date, DATEPICKERSTRINGFORMAT), id });
    }

    std::string MediaController::CreateVideoTimeline(const Source& source, std::chrono::system_clock::time_point from, std::chrono::system_clock::time_point to) const
    {
        std::string videotimeline;
        std::filesystem::path videoDirectory = dvrlite->GetConfig().GetRecordPath();
        videoDirectory = videoDirectory / source.GetName();
        int groupIndex = 0;

        JsonCache& cache = dvrlite->GetCache();
        cache.Preload(videoDirectory);

        std::string pickers = CreateDatePicker("From: ", from, "fromDatePicker");
        pickers += CreateDatePicker("To: ", to, "toDatePicker");

        if (std::filesystem::is_directory(videoDirectory))
        {
            int i = 1;
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(videoDirectory))
            {
                if (entry.path().extension() == ".json")
                {
                    Json::Value* jsonptr = cache.Get(entry.path().string());
                    if (jsonptr != nullptr)
                    {
                        Json::Value& json = *jsonptr;
                        std::chrono::system_clock::time_point startTimepoint = to_timepoint(json["startTime"].asString(), DATESTRINGFORMAT);
                        std::chrono::system_clock::time_point endTimepoint = to_timepoint(json["endTime"].asString(), DATESTRINGFORMAT);
                        if (endTimepoint > from && startTimepoint < to)
                        {
                            if (i > 1)
                                videotimeline += ",";

                            std::filesystem::path videofile = entry.path().filename().replace_extension(".mp4");
                            std::vector<std::string> videoParameters{ std::to_string(i++), '\'' + videofile.string() + '\'', std::to_string(groupIndex), '\'' + source.GetName() + '\'', '\'' + json["startTime"].asString() + '\'', '\'' + json["endTime"].asString() + '\'' };
                            videotimeline += ApplyTemplate("videotimelineelement", videoParameters);
                        }
                    }
                }
            }
        }
        std::vector<std::string> groupParameters{ std::to_string(groupIndex), '\'' + source.GetName() + '\'' };
        std::string videoGroups = ApplyTemplate("videotimelinegroup", groupParameters);
        std::vector<std::string> timelineParameters{ pickers, videotimeline, videoGroups };
        return ApplyTemplate("videotimeline", timelineParameters);
    }

    const std::string checkedOn = "checked";
    const std::string checkedOff = "";

    std::string MediaController::CreateSourceCheckboxes(const Source &source) const
    {
        std::unordered_set<std::string> triggers = source.GetTriggers();
        
        std::string sourcecheckboxes = ApplyTemplate("sourcecheckboxes", {"self_trigger", "self", triggers.contains(source.GetName()) ? checkedOn : checkedOff } );
        for (const Source& source : dvrlite->GetSources())
            sourcecheckboxes += ApplyTemplate("sourcecheckboxes", { "trigger_" + source.GetName(), source.GetName(), triggers.contains(source.GetName()) ? checkedOn : checkedOff });
        return sourcecheckboxes;
    }

    std::string MediaController::CreateConfigList() const
    {
        std::string configlist;
        std::vector<std::string> recordParameters{ "\"recordPath\"", "Record Path ", dvrlite->GetConfig().GetRecordPath(), "text", "required" };
        configlist = ApplyTemplate("typedrecord", recordParameters);
        std::vector<std::string> portParameters{ "\"port\"", "Port ", std::to_string(dvrlite->GetConfig().GetPort()), "number", "" };
        configlist += ApplyTemplate("typedrecord", portParameters);
        
        std::string loglist = ApplyTemplate("checkboxrecordelement", { "\"logDVRLite\"", "DVRLite ", Has(dvrlite->GetConfig().GetLogFilter(), LogFilter::DVRLite)?checkedOn:checkedOff, "" });
        loglist += ApplyTemplate("checkboxrecordelement", { "\"logOnvif\"", "Onvif ", Has(dvrlite->GetConfig().GetLogFilter(), LogFilter::Onvif) ? checkedOn : checkedOff, "" });
        loglist += ApplyTemplate("checkboxrecordelement", { "\"logPullPointSubscription\"", "PullPointSubscription ", Has(dvrlite->GetConfig().GetLogFilter(), LogFilter::PullPointSubscription) ? checkedOn : checkedOff, "" });
        loglist += ApplyTemplate("checkboxrecordelement", { "\"logFFmpeg\"", "FFmpeg ", Has(dvrlite->GetConfig().GetLogFilter(), LogFilter::FFmpeg) ? checkedOn : checkedOff, "" });
        loglist += ApplyTemplate("checkboxrecordelement", { "\"logWeb\"", "Web ", Has(dvrlite->GetConfig().GetLogFilter(), LogFilter::Web) ? checkedOn : checkedOff, "" });
        configlist += ApplyTemplate("customrecord", { "Log", loglist });

        configlist += templates["submitrecord"].asString();
        return ApplyTemplate("form", { "\"configure\"", configlist });
    }

    std::string MediaController::CreateLog() const
    {
        std::ifstream logfile = std::ifstream(dvrlite->GetConfig().GetLogPath());
        if (logfile.is_open())
        {
            std::string log;
            for (std::string line; std::getline(logfile, line); )
            {
                log += line;
                log += "<br>";
            }
            return log;
        }
        return "";
    }

    std::string MediaController::CreateSnapshot(const Source& source) const
    {
        return ApplyTemplate("sourcesnapshot", source.GetAuthSnapshotAddress());
    }

    std::string MediaController::ApplyTemplate(const std::string& templatename, const std::string& value) const
    {
        Json::Value jsonvalue = templates[templatename];
        std::string result = templates[templatename].asString();
        bool found = true;
        while (found)
            found = replace_substring(result, "{}", value, result);
        return result;
    }

    std::string MediaController::CreateAddSourceTable(const Source &source) const
    {
        std::string addsourcetable;
        std::vector<std::string> nameParameters{ "\"name\"", "Name ", source.GetName(), "text", "required" };
        addsourcetable += ApplyTemplate("typedrecord", nameParameters);
        std::vector<std::string> addressParameters{ "\"onvifAddress\"", "Address ", source.GetOnvifAddress(), "url", "required" };
        addsourcetable += ApplyTemplate("typedrecord", addressParameters);
        std::vector<std::string> videoAddressParameters{ "\"customVideoAddress\"", "Custom Video Address (optional) ", source.GetVideoAddress(), "url", "" };
        addsourcetable += ApplyTemplate("typedrecord", videoAddressParameters);
        std::vector<std::string> usernameParameters{ "\"username\"", "Username ", source.GetUsername(), "text", "required" };
        addsourcetable += ApplyTemplate("typedrecord", usernameParameters);
        std::vector<std::string> passwordParameters{ "\"password\"", "Password ", source.GetPassword(), "password", "required" };
        addsourcetable += ApplyTemplate("typedrecord", passwordParameters);

        addsourcetable += ApplyTemplate("customrecord", { "Sources to trigger ", CreateSourceCheckboxes(source) });

        std::vector<std::string> durationParameters{ "\"duration\"", "Record duration (seconds) ", std::to_string(source.GetDuration()), "number", "" };
        addsourcetable += ApplyTemplate("typedrecord", durationParameters);
        std::vector<std::string> quotaParameters{ "\"quota\"", "Disk quota (mb) ", std::to_string(source.GetQuota()), "number", "" };
        addsourcetable += ApplyTemplate("typedrecord", quotaParameters);

        std::vector<std::string> recordAudioParameters{ "\"recordAudio\"", "Record Audio ", "", "checkbox", source.GetRecordAudio() ? checkedOn : checkedOff };
        addsourcetable += ApplyTemplate("typedrecord", recordAudioParameters);
        std::vector<std::string> recordVideoParameters{ "\"recordVideo\"", "Record Video ", "", "checkbox",  source.GetRecordVideo() ? checkedOn : checkedOff };
        addsourcetable += ApplyTemplate("typedrecord", recordVideoParameters);

        addsourcetable += templates["submitrecord"].asString();
        return ApplyTemplate("form", {"\"add_source\"", addsourcetable});
    }

    std::string MediaController::ApplyTemplate(const std::string& templatename, const std::vector<std::string>& values) const
    {
        Json::Value jsonvalue = templates[templatename];
        std::string result = templates[templatename].asString();
        for (int i = 0; i < values.size(); ++i)
        {
            std::string tag = '{' + std::to_string(i) + '}';
            bool found = true;
            while (found)
            {
                std::string value = values[i];
                if (value.empty())value = "\"\"";
                found = replace_substring(result, tag, value, result);
            }
        }
        return result;
    }

    void MediaController::ApplyTemplates(const std::string& pageTitle, std::string& content, const Source& currentSource, std::chrono::system_clock::time_point startTime, std::chrono::system_clock::time_point endTime) const
    {
        if (content.find("#header#") != std::string::npos)
            replace_substring(content, "#header#", CreateHeader(pageTitle), content);
        if (content.find("#sourcelist#") != std::string::npos)
            replace_substring(content, "#sourcelist#", CreateSourceList(), content);
        if (content.find("#videolist#") != std::string::npos)
            replace_substring(content, "#videolist#", CreateVideoList(currentSource), content);
        if (content.find("#videosnapshot#") != std::string::npos)
            replace_substring(content, "#videosnapshot#", CreateVideoSnapshot(currentSource), content);
        if (content.find("#videotimeline#") != std::string::npos)
            replace_substring(content, "#videotimeline#", CreateVideoTimeline(currentSource, startTime, endTime), content);
        if (content.find("#sourcecheckboxes#") != std::string::npos)
            replace_substring(content, "#addsourceform#", CreateAddSourceTable(currentSource), content);
        if (content.find("#configform#") != std::string::npos)
            replace_substring(content, "#configform#", CreateConfigList(), content);
        if (content.find("#htmlheader#") != std::string::npos)
            replace_substring(content, "#htmlheader#", CreateHTMLHeader(pageTitle), content);
        if (content.find("#log#") != std::string::npos)
            replace_substring(content, "#log#", CreateLog(), content);
        if (content.find("#snapshot#") != std::string::npos)
            replace_substring(content, "#snapshot#", CreateSnapshot(currentSource), content);
    }

    MediaController::MediaController(const std::shared_ptr<ObjectMapper>& objectMapper) :
        oatpp::web::server::api::ApiController(objectMapper)
    {
    }

    void MediaController::LoadTemplates()
    {
        std::string webPath = dvrlite->GetConfig().GetWebPath();
        std::ifstream file(std::filesystem::path(webPath) / "templates.json");
        if (file.is_open())
        {
            file >> templates;
        }
    }

    void MediaController::GetTimes(oatpp::String inStartTime, oatpp::String inEndTime, std::chrono::system_clock::time_point& startTime, std::chrono::system_clock::time_point& endTime)
    {
        bool inStartTimeEmpty = inStartTime.get() == nullptr || inStartTime.get()->getSize() == 0;
        bool inEndTimeEmpty = inEndTime.get() == nullptr || inEndTime.get()->getSize() == 0;

        if (inStartTimeEmpty && inEndTimeEmpty)
        {
            startTime = std::chrono::system_clock::now() - std::chrono::hours(24 * 365 * 10);
            endTime = std::chrono::system_clock::now();
        }
        else if (inStartTimeEmpty)
        {
            endTime = to_timepoint(strip_quotes(unescapeUrl(inEndTime->std_str()), DATEREFSTRING), DATESTRINGFORMAT);
            startTime = endTime - std::chrono::hours(24);
        }
        else if (inEndTimeEmpty)
        {
            startTime = to_timepoint(strip_quotes(unescapeUrl(inStartTime->std_str()), DATEREFSTRING), DATESTRINGFORMAT);
            endTime = startTime + std::chrono::hours(24);
        }
        else
        {
            startTime = to_timepoint(strip_quotes(unescapeUrl(inStartTime->std_str()), DATEREFSTRING), DATESTRINGFORMAT);
            endTime = to_timepoint(strip_quotes(unescapeUrl(inEndTime->std_str()), DATEREFSTRING), DATESTRINGFORMAT);
        }
    }

    Logger::LogFilter MediaController::GetLogFilter(const QueryParams& queryParams)
    {
        LogFilter result = LogFilter::None;
        result |= (queryParams.get("logDVRLite") == "on") ? LogFilter::DVRLite : LogFilter::None;
        result |= (queryParams.get("logOnvif") == "on") ? LogFilter::Onvif : LogFilter::None;
        result |= (queryParams.get("logPullPointSubscription") == "on") ? LogFilter::PullPointSubscription : LogFilter::None;
        result |= (queryParams.get("logFFmpeg") == "on") ? LogFilter::FFmpeg : LogFilter::None;
        result |= (queryParams.get("logWeb") == "on") ? LogFilter::Web : LogFilter::None;
        return result;
    }

    void MediaController::run(DVRLite* dvrlite)
    {
        oatpp::base::Environment::init();

        AppComponent components; // Create scope Environment components

        /* create ApiControllers and add endpoints to router */

        auto router = components.httpRouter.getObject();

        auto mediaController = MediaController::createShared();
        mediaController->setDVR(dvrlite);
        mediaController->LoadTemplates();
        mediaController->addEndpointsToRouter(router);

        /* create server */

        oatpp::network::Server server(components.serverConnectionProvider.getObject(),
            components.serverConnectionHandler.getObject());
        mediaController->server = &server;

        //OATPP_LOGI("Server", "Running on port %s...", components.serverConnectionProvider.getObject()->getProperty("port").toString()->c_str());
        Log(LogFilter::Web, "Running on port " + components.serverConnectionProvider.getObject()->getProperty("port").toString()->std_str());

        server.run();

       //oatpp::base::Environment::destroy();
    }
}