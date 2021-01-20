#include "PullPointSubscription.h"
#include <memory>

class DVRLite;

class Onvif
{
public:
    Onvif(DVRLite const *dvrlite);

    //TODO mediacontroller needs to be able to add and remove pullpoints while it's running
private:
    DVRLite const* dvrlite;
    std::vector<std::unique_ptr<PullPointSubscription>> subscriptions;
};