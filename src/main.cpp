#include <Geode/Geode.hpp>
#include <Geode/loader/Dispatch.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "NekoBounds.hpp"

using namespace geode::prelude;

Dispatch<CCNode*, CCRect> createNekoRectEvent{"create-neko-rect"_spr};
Dispatch<CCNode*> createNekoEvent{"create-neko"_spr};

$on_mod(Loaded) {
    // Parent needs to be specified because getting the scene from CCDirector gives me the exiting scene

    // Create Neko bounds with given CCRect
    auto nekoCreateRectListener = createNekoRectEvent.listen(+[](CCNode *parent, CCRect rect) {
        log::info("creating Neko with rect");
        NekoBounds::placeWithRect(parent, rect);
    });
    
    // Create Neko bounds for full screen
    auto nekoCreateListener = createNekoEvent.listen(+[](CCNode *parent) {
        log::info("creating Neko!");
        NekoBounds::place(parent);
    });

    nekoCreateRectListener.leak();
    nekoCreateListener.leak();
};

#define settingCheckVoid(key)                    \
    if (!Mod::get()->getSettingValue<bool>(key)) \
    return
#define settingCheckBool(key) settingCheckVoid(key) true

// Now we just hook every layer we want to have the silly little goober in!

class $modify(NekoMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        settingCheckBool("menulayer");

        createNekoEvent.send(this);

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
    bool init(GJSearchObject *p0) {
        if (!LevelBrowserLayer::init(p0))
            return false;

        settingCheckBool("levelbrowserlayer");

        GJListLayer* layer = this->getChildByType<GJListLayer>(0);
        CCSize const& size = layer->getContentSize();
        CCPoint const& pos = layer->getPosition() + size / 2;
        CCRect const& rect = CCRect(pos, size);

        createNekoRectEvent.send(this, rect);

        return true;
    }
};

#include <Geode/modify/SecretRewardsLayer.hpp>
class $modify(NekoRewardsLayer, SecretRewardsLayer) {
    bool init(bool p0) {
        if (!SecretRewardsLayer::init(p0))
            return false;

        settingCheckBool("secretrewardslayer");

        createNekoEvent.send(this);

        return true;
    }
};

#include <Geode/modify/PauseLayer.hpp>
class $modify(NekoPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        settingCheckVoid("pauselayer");

        createNekoEvent.send(this);
    }
};

#include <Geode/modify/GJGarageLayer.hpp>
class $modify(NekoGarageLayer, GJGarageLayer) {
    bool init() {
        if (!GJGarageLayer::init())
            return false;

        settingCheckBool("gjgaragelayer");

        createNekoEvent.send(this);
        return true;
    }
};

#include <Geode/modify/LevelSelectLayer.hpp>
class $modify(NekoSelectLayer, LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page))
            return false;

        settingCheckBool("levelselectlayer");

        createNekoEvent.send(this);
        return true;
    }
};

#include <Geode/modify/LevelSearchLayer.hpp>
class $modify(NekoSearchLayer, LevelSearchLayer) {
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0))
            return false;

        settingCheckBool("levelsearchlayer");

        createNekoEvent.send(this);
        return true;
    }
};

#include <Geode/modify/CreatorLayer.hpp>
class $modify(NekoCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init())
            return false;

        settingCheckBool("creatorlayer");

        createNekoEvent.send(this);

        return true;
    }
};

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(NekoLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge))
            return false;

        settingCheckBool("levelinfolayer");

        createNekoEvent.send(this);
        return true;
    }
};

#include <Geode/modify/LevelEditorLayer.hpp>
class $modify(NekoLevelEditorLayer, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool p1) {
        if (!LevelEditorLayer::init(level, p1))
            return false;

        settingCheckBool("leveleditorlayer");

        createNekoEvent.send(this);
        return true;
    }
};

#include <Geode/modify/EditLevelLayer.hpp>
class $modify(NekoEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* level) {
        if (!EditLevelLayer::init(level))
            return false;

        settingCheckBool("editlevellayer");

        createNekoEvent.send(this);
        return true;
    }
};

#ifdef CHAOS_MODE
#include <Geode/modify/CCMenuItemSpriteExtra.hpp>
class $modify(NekoMenuItemSpriteExtra, CCMenuItemSpriteExtra) {
    bool
    init(CCNode *sprite, CCNode *disabledSprite, CCObject *target, SEL_MenuHandler callback) {
        if (!CCMenuItemSpriteExtra::init(sprite, disabledSprite, target, callback))
            return false;

        createNekoEvent.send(this);

        return true;
    }
};
#endif