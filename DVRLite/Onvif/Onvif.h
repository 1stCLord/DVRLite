#pragma once

#include "PullPointSubscription.h"
#include <memory>

namespace DVRLite
{
    class DVRLite;

    class Onvif
    {
    public:
        Onvif();
        Onvif(const Onvif&) = delete;
        Onvif(const Onvif&&) = delete;
        const Onvif& operator=(const Onvif&) = delete;
        const Onvif& operator=(Onvif&&) = delete;
        ~Onvif() = default;
        Onvif(DVRLite* dvrlite);

        void Init(DVRLite* dvrlite);

        void Add(const Source& source);
        void Remove(const std::string& source);

    private:
        DVRLite const* dvrlite;
        std::vector<std::unique_ptr<PullPointSubscription>> subscriptions;

        static const Logger::LogFilter filter = Logger::LogFilter::Onvif;
    };
}