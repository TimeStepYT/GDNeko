#include "NekoBounds.hpp"
#include "NekoNode.hpp"

using namespace geode::prelude;

NekoBounds* NekoBounds::create(CCNode* parent) {
    auto res = new NekoBounds();
    if (res) {
        res->m_parent = parent;
        if (res && res->init()) {
            res->autorelease();
            return res;
        }
    }
    CC_SAFE_DELETE(res);
    return nullptr;
}

NekoBounds* NekoBounds::create(CCNode* parent, CCRect rect) {
    auto res = new NekoBounds();
    if (res) {
        res->m_rect = rect;
        res->m_parent = parent;
        if (res->init()) {
            res->autorelease();
            return res;
        }
    }

    CC_SAFE_DELETE(res);
    return nullptr;
}

void NekoBounds::placeWithRect(CCNode* parent, CCRect rect) {
    auto director = CCDirector::get();
    auto nekoBounds = NekoBounds::create(parent, rect);

    parent->addChild(nekoBounds);
}

void NekoBounds::place(CCNode* parent) {
    auto director = CCDirector::get();
    auto nekoBounds = NekoBounds::create(parent);

    parent->addChild(nekoBounds);
}

bool NekoBounds::init() {
    if (!CCNode::init()) return false;

    auto& rect = this->m_rect;
    auto& parent = this->m_parent;

    this->setID("neko-bounds"_spr);

    CCArrayExt<CCNode*> siblings = parent->getChildren();

    // We don't want our kitty to get buried below normal GD stuff
    int highestZ = 0;
    for (auto sibling : siblings) {
        int z = sibling->getZOrder();
        if (z > highestZ)
            highestZ = z;
    }
    this->setZOrder(highestZ + 1);
    this->setAnchorPoint(ccp(0.5f, 0.5f));

    if (rect.has_value()) {
        this->setPosition(rect->origin + rect->size * parent->getAnchorPoint());
        this->setContentSize(rect->size);
    }
    else {
        auto const contentSize = parent->getContentSize();
        this->setPosition(parent->getPosition() + contentSize * parent->getAnchorPoint());
        this->setContentSize(contentSize);
    }

    // Create the NekoNode AFTER setting the content size! GRRR
    auto nekoNode = NekoNode::create(this);
    
    this->addChild(nekoNode);
    return true;
}