#include <Geode/Geode.hpp>
#include <Geode/loader/Dispatch.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include "NekoBounds.hpp"
#include "global.hpp"

using namespace geode::prelude;

#include <capeling.soggy-mod/include/Events.hpp>
//#include <timestepyt.heisbehindthetree/api/API.hpp>
//#include <timestepyt.deltarune_textboxes/api/API.hpp>

Dispatch<CCNode*, CCRect> createNekoRectEvent{"create-neko-rect"_spr};
Dispatch<CCNode*> createNekoEvent{"create-neko"_spr};

$on_mod(Loaded) {
    // Parent needs to be specified because getting the scene from CCDirector gives me the exiting scene
    
    // Create Neko bounds with given CCRect
    auto nekoCreateRectListener = createNekoRectEvent.listen(+[](CCNode *parent, CCRect rect) {
        NekoBounds::placeWithRect(parent, rect);
    });
    
    // Create Neko bounds for full screen
    auto nekoCreateListener = createNekoEvent.listen(+[](CCNode *parent) {
        NekoBounds::place(parent);
    });
    
    nekoCreateRectListener.leak();
    nekoCreateListener.leak();

    auto soggyListener = soggy_mod::OnSogLayer().listen([](auto layer) {
        createNekoEvent.send(layer);
    });
    soggyListener.leak();

    auto chronoNow = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(chronoNow);
    std::tm now = geode::localtime(t);

    if (now.tm_mon == 3 && now.tm_mday == 1) {
        neko_global::april_fools = true;
    }
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

        geode::Dispatch<CCNode*>("timestepyt.gdneko/create-neko").send(this);

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
    struct Fields {
        NekoBounds* bounds = nullptr;
    };

    bool init(GJGameLevel* level, bool p1) {
        if (!LevelEditorLayer::init(level, p1))
            return false;

        settingCheckBool("leveleditorlayer");

        createNekoEvent.send(this);

        auto* const bounds = this->getChildByID("neko-bounds"_spr);
        m_fields->bounds = static_cast<NekoBounds*>(bounds);
        
        return true;
    }
};

#include <Geode/modify/EditorUI.hpp>
class $modify(NekoEditorUI, EditorUI) {
    void setNekoVisible(bool visible) {
        settingCheckVoid("hideduringplaytest");

        NekoLevelEditorLayer* lel = static_cast<NekoLevelEditorLayer*>(this->m_editorLayer);
        auto* const bounds = lel->m_fields->bounds;

        if (!bounds)
            return;

        bounds->setVisible(visible);
    }

    bool isNekoVisible() {
        NekoLevelEditorLayer* lel = static_cast<NekoLevelEditorLayer*>(this->m_editorLayer);
        auto* const bounds = lel->m_fields->bounds;

        if (!bounds)
            return false;

        return bounds->isVisible();
    }

    void onPlaytest(CCObject* sender) {
        this->setNekoVisible(!this->isNekoVisible());

        EditorUI::onPlaytest(sender);
    }

    void playtestStopped() {
        this->setNekoVisible(true);
        
        EditorUI::playtestStopped();
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

#include <Geode/modify/LevelListLayer.hpp>
class $modify(NekoLevelListLayer, LevelListLayer) {
    bool init(GJLevelList* list) {
        if (!LevelListLayer::init(list))
            return false;
        
        settingCheckBool("levellistlayer");

        auto layer = this->getChildByType<GJListLayer>(0);

        CCSize size = layer->getContentSize();
        CCPoint pos = layer->getPosition() + size / 2;
        CCRect rect {pos, size};

        createNekoRectEvent.send(this, rect);
        
        return true;
    }
};
#include <Geode/modify/LeaderboardsLayer.hpp>
class $modify(NekoLeaderboardsLayer, LeaderboardsLayer) {
    bool init(LeaderboardType type, LeaderboardStat stat) {
        if (!LeaderboardsLayer::init(type, stat))
            return false;
        
        settingCheckBool("leaderboardslayer");

        auto layer = this->getChildByType<GJListLayer>(0);

        CCSize size = layer->getContentSize();
        CCPoint pos = layer->getPosition() + size / 2;
        CCRect rect {pos, size};

        createNekoRectEvent.send(this, rect);
        
        return true;
    }
};

#include <Geode/modify/GJShopLayer.hpp>
class $modify(NekoShopLayer, GJShopLayer) {
    bool init(ShopType type) {
        if (!GJShopLayer::init(type))
            return false;
        
        settingCheckBool("shoplayer");

        createNekoEvent.send(this);
        
        return true;
    }
};

#include <Geode/modify/GauntletSelectLayer.hpp>
class $modify(NekoGauntletSelectLayer, GauntletSelectLayer) {
    void setupGauntlets() {
        GauntletSelectLayer::setupGauntlets();
        
        settingCheckVoid("gauntletselectlayer");

        createNekoEvent.send(this);
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