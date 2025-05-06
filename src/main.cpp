#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/loader/Dispatch.hpp>

#include "NekoBounds.hpp"

using namespace geode::prelude;

using CreateNekoRectEvent = geode::DispatchEvent<CCNode*, CCRect>;
using CreateNekoRectFilter = geode::DispatchFilter<CCNode*, CCRect>;
using CreateNekoEvent = geode::DispatchEvent<CCNode*>;
using CreateNekoFilter = geode::DispatchFilter<CCNode*>;

$execute{
    // Parent needs to be specified because getting the scene from CCDirector gives me the exiting scene
    // Create Neko bounds with given CCRect
    new EventListener<CreateNekoRectFilter>(+[](CCNode* parent, CCRect rect) {
        NekoBounds::placeWithRect(parent, rect);
        return ListenerResult::Propagate;
    }, CreateNekoRectFilter("create-neko-rect"_spr));

    // Create Neko bounds for full screen
    new EventListener<CreateNekoFilter>(+[](CCNode* parent) {
        NekoBounds::place(parent);
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

#include <Geode/modify/GJListLayer.hpp>
class $modify(NekoListLayer, GJListLayer) {
    bool init(BoomListView * p0, char const* p1, ccColor4B p2, float p3, float p4, int p5) {
        if (!GJListLayer::init(p0, p1, p2, p3, p4, p5)) return false;

        CreateNekoEvent("create-neko"_spr, this).post();

        return true;
    }
};

#ifdef CHAOS_MODE
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
class $modify(NekoMenuItemSpriteExtra, CCMenuItemSpriteExtra) {
    bool init(CCNode * sprite, CCNode * disabledSprite, CCObject * target, SEL_MenuHandler callback) {
        if (!CCMenuItemSpriteExtra::init(sprite, disabledSprite, target, callback)) return false;

        CreateNekoEvent("create-neko"_spr, this).post();

        return true;
    }
};
#endif