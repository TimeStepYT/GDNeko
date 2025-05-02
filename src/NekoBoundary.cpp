#include "NekoBoundary.hpp"
#include "NekoNode.hpp"

using namespace geode::prelude;

NekoBoundary* NekoBoundary::create() {
    auto res = new NekoBoundary();

    if (res && res->init()) {
        res->autorelease();
        return res;
    }

    CC_SAFE_DELETE(res);
    return nullptr;
}

void NekoBoundary::placeWithRect(CCLayer* layer, CCRect rect) {
    auto director = CCDirector::get();
    auto nekoBoundary = NekoBoundary::create();

    nekoBoundary->setPosition(rect.origin);
    nekoBoundary->setContentSize(rect.size);

    layer->addChild(nekoBoundary);
}

void NekoBoundary::place(CCLayer* layer) {
    auto director = CCDirector::get();
    auto nekoBoundary = NekoBoundary::create();

    layer->addChild(nekoBoundary);
}

bool NekoBoundary::init() {
    if (!CCLayer::init()) return false;

    this->setID("neko-boundary"_spr);

    auto nekoNode = NekoNode::create(this);
    this->addChild(nekoNode);
    return true;
}