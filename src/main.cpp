#include <Geode/Geode.hpp>
#include <Geode/loader/Dispatch.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "NekoBounds.hpp"

using namespace geode::prelude;

using CreateNekoRectEvent = geode::DispatchEvent<CCNode*, CCRect>;
using CreateNekoRectFilter = geode::DispatchFilter<CCNode*, CCRect>;
using CreateNekoEvent = geode::DispatchEvent<CCNode*>;
using CreateNekoFilter = geode::DispatchFilter<CCNode*>;

$execute {
    // Parent needs to be specified because getting the scene from CCDirector gives me the exiting scene
    // Create Neko bounds with given CCRect
    new EventListener<CreateNekoRectFilter>(+[](CCNode* parent, CCRect rect) {
        NekoBounds::placeWithRect(parent, rect);
        return ListenerResult::Propagate; }, CreateNekoRectFilter("create-neko-rect"_spr));

    // Create Neko bounds for full screen
    new EventListener<CreateNekoFilter>(+[](CCNode* parent) {
        NekoBounds::place(parent);
        return ListenerResult::Propagate; }, CreateNekoFilter("create-neko"_spr));
};

#define settingCheckVoid(key)   if (!Mod::get()->getSettingValue<bool>(key)) return
#define settingCheckBool(key)   settingCheckVoid(key) true

// Now we just hook every layer we want to have the silly little goober in!

class $modify(NekoMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        settingCheckBool("menulayer");

        CreateNekoEvent("create-neko"_spr, this).post();

        /*
        This is the same as:

        geode::DispatchEvent<CCNode*>("timestepyt.gdneko/create-neko", this).post();

        You should be able to copy-paste that line directly in your layer's / node's init method!
        */

        return true;
    }
};

#include <Geode/modify/LevelBrowserLayer.hpp>
class $modify(NekoBrowserLayer, LevelBrowserLayer) {
    bool init(GJSearchObject* p0) {
        if (!LevelBrowserLayer::init(p0)) return false;

        settingCheckBool("levelbrowserlayer");

        auto layer = this->getChildByType<GJListLayer>(0);
        auto size = layer->getContentSize();
        auto pos = layer->getPosition() + size / 2;
        auto rect = CCRect(pos, size);

        CreateNekoRectEvent("create-neko-rect"_spr, this, rect).post();

        return true;
    }
};

#include <Geode/modify/SecretRewardsLayer.hpp>
class $modify(NekoRewardsLayer, SecretRewardsLayer) {
    bool init(bool p0) {
        if (!SecretRewardsLayer::init(p0)) return false;

        settingCheckBool("secretrewardslayer");

        CreateNekoEvent("create-neko"_spr, this).post();

        return true;
    }
};

#include <Geode/modify/PauseLayer.hpp>
class $modify(NekoPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        settingCheckVoid("pauselayer");

        CreateNekoEvent("create-neko"_spr, this).post();
    }
};

#include <Geode/modify/GJGarageLayer.hpp>
class $modify(NekoGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init()) return false;

        settingCheckBool("gjgaragelayer");

        CreateNekoEvent("create-neko"_spr, this).post();
        return true;
    }
};

#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(NekoSelectLayer, LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;

        settingCheckBool("levelselectlayer");

        CreateNekoEvent("create-neko"_spr, this).post();
        return true;
    }
};

#include <Geode/modify/LevelSearchLayer.hpp>
class $modify(NekoSearchLayer, LevelSearchLayer) {
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;

        settingCheckBool("levelsearchlayer");

        CreateNekoEvent("create-neko"_spr, this).post();
        return true;
    }
};

#include <Geode/modify/CreatorLayer.hpp>
class $modify(NekoCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;

        settingCheckBool("creatorlayer");

        CreateNekoEvent("create-neko"_spr, this).post();
        return true;
    }
};

#ifdef CHAOS_MODE
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
class $modify(NekoMenuItemSpriteExtra, CCMenuItemSpriteExtra) {
    bool init(CCNode* sprite, CCNode* disabledSprite, CCObject* target, SEL_MenuHandler callback) {
        if (!CCMenuItemSpriteExtra::init(sprite, disabledSprite, target, callback)) return false;

        CreateNekoEvent("create-neko"_spr, this).post();

        return true;
    }
};
#endif