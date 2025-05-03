#include "NekoBoundary.hpp"
#include "NekoNode.hpp"

using namespace geode::prelude;

NekoBoundary* NekoBoundary::create(CCLayer* layer) {
    auto res = new NekoBoundary();

    if (res && res->init(layer)) {
        res->autorelease();
        return res;
    }

    CC_SAFE_DELETE(res);
    return nullptr;
}

void NekoBoundary::placeWithRect(CCLayer* layer, CCRect rect) {
    auto director = CCDirector::get();
    auto nekoBoundary = NekoBoundary::create(layer);

    nekoBoundary->setPosition(rect.origin);
    nekoBoundary->setContentSize(rect.size);
    
    layer->addChild(nekoBoundary);
}

void NekoBoundary::place(CCLayer* layer) {
    auto director = CCDirector::get();
    auto nekoBoundary = NekoBoundary::create(layer);

    layer->addChild(nekoBoundary);
}

bool NekoBoundary::init(CCLayer* layer) {
    if (!CCLayer::init()) return false;

    this->setID("neko-boundary"_spr);
    
    CCArrayExt<CCNode*> siblings = layer->getChildren();
    
    int highestZ = 0;
    for (auto sibling : siblings) {
        int z = sibling->getZOrder();
        if (z > highestZ)
            highestZ = z;
    }
    this->setZOrder(highestZ + 1);
    
    auto nekoNode = NekoNode::create(this);
    this->addChild(nekoNode);
    return true;
}