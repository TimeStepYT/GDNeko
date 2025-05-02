#include "NekoNode.hpp"

using namespace geode::prelude;

NekoNode* NekoNode::create(NekoBoundary* boundary) {
    auto res = new NekoNode();

    if (res && res->init(boundary)) {
        res->autorelease();
        return res;
    }
    CC_SAFE_DELETE(res);
    return nullptr;
}

bool NekoNode::init(NekoBoundary* boundary) {
    if (!CCNode::init()) return false;

    auto scaleFactor = CCDirector::get()->getContentScaleFactor();
    auto& scale = this->m_scale;
    float const scaledContentSize = 280 * scale / scaleFactor;

    this->setID("neko"_spr);
    this->setContentSize({scaledContentSize, scaledContentSize});
    this->setPositionX(boundary->getContentWidth() / 2);
    this->setPositionY(boundary->getContentHeight() / 2);
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    
    // @geode-ignore(unknown-resource) geode is blind
    auto nekoSprite = CCSprite::createWithSpriteFrameName("idle.png"_spr);
    
    nekoSprite->setID("neko-sprite"_spr);
    nekoSprite->setScale(scale);
    nekoSprite->setPositionX(this->getContentWidth() / 2);
    nekoSprite->setPositionY(this->getContentHeight() / 2);
    
    m_nekoSprite = nekoSprite;
    m_nekoBoundary = boundary;

    this->addChild(nekoSprite);

    return true;
}