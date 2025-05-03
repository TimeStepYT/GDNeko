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
    this->setContentSize({ scaledContentSize, scaledContentSize });
    this->setPositionX(boundary->getContentWidth() / 2);
    this->setPositionY(boundary->getContentHeight() / 2);
    this->setAnchorPoint(ccp(0.5f, 0.5f));

    // @geode-ignore(unknown-resource) geode extension is blind
    auto nekoSprite = CCSprite::createWithSpriteFrameName("idle_0_0.png"_spr);

    nekoSprite->setID("neko-sprite"_spr);
    nekoSprite->setScale(scale);
    nekoSprite->setPositionX(this->getContentWidth() / 2);
    nekoSprite->setPositionY(this->getContentHeight() / 2);

    m_nekoSprite = nekoSprite;
    m_nekoBoundary = boundary;

    this->addChild(nekoSprite);
    scheduleUpdate();

    return true;
}

void NekoNode::update(float dt) {
    auto const mousePos = geode::cocos::getMousePos();
    auto const vec = mousePos - this->getPosition();
    auto const normVec = vec.normalize();
    auto const pos = this->getPosition();
    CCPoint const futurePos = pos + normVec * this->m_speed * dt;
    int const maxFrames = 2;
    float const frameChangesPerSecond = 5.f;
    float const timeUntilFrameChange = 1.f / frameChangesPerSecond;
    auto& timer = this->m_animTimer;
    auto& state = this->m_state;
    auto& frameNumber = this->m_frame;
    auto frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    std::string stateString;
    std::string frameString;

    switch (state) {
        case NekoState::RUNNING:
            timer += dt;
            if (timer >= timeUntilFrameChange) {
                timer -= timeUntilFrameChange;
                frameNumber = (frameNumber + 1) % maxFrames;
            }
            stateString = "running";
            break;
        case NekoState::IDLE:
            stateString = "idle";
            frameNumber = 0;
            break;
        default:
            break;
    }

    int deadzone = 17;
    if (mousePos.getDistance(futurePos) < deadzone) {
        state = NekoState::IDLE;
    }
    else {
        state = NekoState::RUNNING;
        this->setPosition(futurePos);
    }

    Direction direction;
    if (state == NekoState::IDLE)
        direction = (Direction) 0;
    else
        direction = getFrameDirection(vec);

    frameString = fmt::format("{}_{}_{}.png"_spr, stateString, (int) direction, frameNumber);
    auto frame = frameCache->spriteFrameByName(frameString.c_str());
    this->m_nekoSprite->setDisplayFrame(frame);
}

Direction NekoNode::getFrameDirection(CCPoint vec) {
    float const angleRad = vec.getAngle();
    float angle = angleRad * (180 / M_PI);
    float const sectorAngle = 360 / 8.f;
    float const offset = sectorAngle / 2;

    if (angle >= 0) { // upwards
        if (angle <= offset)
            return Direction::RIGHT;
        else if (angle <= sectorAngle + offset)
            return Direction::UP_RIGHT;
        else if (angle <= 2 * sectorAngle + offset)
            return Direction::UP;
        else if (angle <= 3 * sectorAngle + offset)
            return Direction::UP_LEFT;
        else
            return Direction::LEFT;
    }
    else {
        if (angle <= -180 + offset)
            return Direction::LEFT;
        else if (angle <= -180 + sectorAngle + offset)
            return Direction::DOWN_LEFT;
        else if (angle <= -180 + 2 * sectorAngle + offset)
            return Direction::DOWN;
        else if (angle <= -180 + 3 * sectorAngle + offset)
            return Direction::DOWN_RIGHT;
        else
            return Direction::RIGHT;
    }
}