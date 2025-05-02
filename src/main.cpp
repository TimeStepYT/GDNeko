#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/loader/Dispatch.hpp>

#include "NekoBoundary.hpp"

using namespace geode::prelude;

using CreateNekoRectEvent = geode::DispatchEvent<CCLayer*, CCRect>;
using CreateNekoRectFilter = geode::DispatchFilter<CCLayer*, CCRect>;
using CreateNekoEvent = geode::DispatchEvent<CCLayer*>;
using CreateNekoFilter = geode::DispatchFilter<CCLayer*>;

$execute{
    // Layer needs to be specified because getting the scene from CCDirector gives me the exiting scene
    // Create Neko boundary with given CCRect
    new EventListener<CreateNekoRectFilter>(+[](CCLayer* layer, CCRect rect) {
        NekoBoundary::placeWithRect(layer, rect);
        return ListenerResult::Propagate;
    }, CreateNekoRectFilter("create-neko-rect"_spr));

    // Create Neko boundary for full screen
    new EventListener<CreateNekoFilter>(+[](CCLayer* layer) {
        NekoBoundary::place(layer);
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