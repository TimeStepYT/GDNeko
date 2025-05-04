#include "NekoBounds.hpp"
#include "NekoNode.hpp"

using namespace geode::prelude;

NekoBounds* NekoBounds::create(CCNode* parent) {
    auto res = new NekoBounds();
    CCRect rect = {ccp(0, 0), parent->getContentSize()}; 
    if (res && res->init(parent, rect)) {
        res->autorelease();
        return res;
    }

    CC_SAFE_DELETE(res);
    return nullptr;
}

NekoBounds* NekoBounds::create(CCNode* parent, CCRect rect) {
    auto res = new NekoBounds();
    if (res && res->init(parent, rect)) {
        res->autorelease();
        return res;
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

bool NekoBounds::init(CCNode* parent, CCRect rect) {
    if (!CCNode::init()) return false;

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
    this->setPosition(rect.origin + rect.size * parent->getAnchorPoint());
    this->setContentSize(rect.size);
    
    auto nekoNode = NekoNode::create(this);

    this->addChild(nekoNode);
    return true;
}