#pragma once

#include "PullPointSubscription.h"
#include <memory>

class DVRLite;

class Onvif
{
public:
    Onvif();
    Onvif(DVRLite *dvrlite);

    void Add(const Source &source);
    void Remove(const std::string& source);
    //TODO mediacontroller needs to be able to add and remove pullpoints while it's running
private:
    DVRLite const* dvrlite;
    std::vector<std::unique_ptr<PullPointSubscription>> subscriptions;
};