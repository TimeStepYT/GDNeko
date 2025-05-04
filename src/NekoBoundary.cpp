#include "NekoBoundary.hpp"
#include "NekoNode.hpp"

using namespace geode::prelude;

NekoBoundary* NekoBoundary::create(CCNode* parent) {
    auto res = new NekoBoundary();

    if (res && res->init(parent)) {
        res->autorelease();
        return res;
    }

    CC_SAFE_DELETE(res);
    return nullptr;
}

void NekoBoundary::placeWithRect(CCNode* parent, CCRect rect) {
    auto director = CCDirector::get();
    auto nekoBoundary = NekoBoundary::create(parent);

    nekoBoundary->setPosition(rect.origin);
    nekoBoundary->setContentSize(rect.size);
    
    parent->addChild(nekoBoundary);
}

void NekoBoundary::place(CCNode* parent) {
    auto director = CCDirector::get();
    auto nekoBoundary = NekoBoundary::create(parent);

    parent->addChild(nekoBoundary);
}

bool NekoBoundary::init(CCNode* parent) {
    if (!CCNode::init()) return false;

    this->setID("neko-boundary"_spr);
    
    CCArrayExt<CCNode*> siblings = parent->getChildren();
    
    int highestZ = 0;
    for (auto sibling : siblings) {
        int z = sibling->getZOrder();
        if (z > highestZ)
            highestZ = z;
    }
    this->setZOrder(highestZ + 1);
    
    auto contentSize = parent->getContentSize();
    this->setContentSize(contentSize);
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    this->setPosition(contentSize / 2);
    
    auto nekoNode = NekoNode::create(this);

    this->addChild(nekoNode);
    return true;
}