#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/loader/Dispatch.hpp>

#include "NekoBoundary.hpp"

using namespace geode::prelude;

using CreateNekoRectEvent = geode::DispatchEvent<CCNode*, CCRect>;
using CreateNekoRectFilter = geode::DispatchFilter<CCNode*, CCRect>;
using CreateNekoEvent = geode::DispatchEvent<CCNode*>;
using CreateNekoFilter = geode::DispatchFilter<CCNode*>;

$execute{
    // Parent needs to be specified because getting the scene from CCDirector gives me the exiting scene
    // Create Neko boundary with given CCRect
    new EventListener<CreateNekoRectFilter>(+[](CCNode* parent, CCRect rect) {
        NekoBoundary::placeWithRect(parent, rect);
        return ListenerResult::Propagate;
    }, CreateNekoRectFilter("create-neko-rect"_spr));

    // Create Neko boundary for full screen
    new EventListener<CreateNekoFilter>(+[](CCNode* parent) {
        NekoBoundary::place(parent);
        return ListenerResult::Propagate;
    }, CreateNekoFilter("create-neko"_spr));
};

class $modify(NekoMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        CreateNekoEvent("create-neko"_spr, this).post();

        return true;
    }
};