#include "NekoNode.hpp"

#include <random>
using namespace geode::prelude;

NekoNode* NekoNode::create(NekoBounds* bounds) {
    auto res = new NekoNode();

    if (res && res->init(bounds)) {
        res->autorelease();
        return res;
    }
    CC_SAFE_DELETE(res);
    return nullptr;
}

bool NekoNode::init(NekoBounds* bounds) {
    if (!CCNode::init()) return false;

    auto& scale = this->m_scale;
    float const scaledContentSize = 280 * scale;

    if (bounds->getContentHeight() == 0) {
        log::error("Neko's bounds are 0 pixels big!");
    }

    this->setID("neko"_spr);
    this->setContentSize({scaledContentSize, scaledContentSize});
    this->setAnchorPoint(ccp(0.5f, 0.5f));

    // Set random position inside bounds
    static std::mt19937 mt{};
    int width = bounds->getContentWidth();
    int height = bounds->getContentHeight();
    float x = mt() % width;
    float y = mt() % height;
    CCPoint pos{x, y};
    this->setPosition(pos);

    auto nekoSprite = CCSprite::createWithSpriteFrameName("idle_0_0.png"_spr);

    auto scaleFactor = 70.f / nekoSprite->getContentHeight();
    float finalScale = scale * scaleFactor;

    nekoSprite->setID("neko-sprite"_spr);
    this->setScale(finalScale);

    nekoSprite->setPosition(this->getContentSize() / 2);
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    touchPos = this->convertToWorldSpace(nekoSprite->getPosition());
#endif

    auto opacitySettingValue = Mod::get()->getSettingValue<int64_t>("opacity");
    nekoSprite->setOpacity(opacitySettingValue / 100.f * 255);

    this->m_nekoSprite = nekoSprite;
    this->m_nekoBounds = bounds;
    this->m_nekoSize = nekoSprite->getContentSize() * this->getScale() / 2;

    this->addChild(nekoSprite);
    scheduleUpdate();

    return true;
}

bool NekoNode::isHittingWall() {
    return this->isHittingWall([](Direction direction) {});
}

bool NekoNode::isHittingWall(std::function<void(Direction)> action) {
    auto& nekoSprite = this->m_nekoSprite;
    auto& futurePos = this->m_futurePos;
    auto& bounds = this->m_nekoBounds;
    auto boundsRect = CCRect(ccp(0, 0), bounds->getContentSize());
    auto& nekoSize = this->m_nekoSize;

    bool ret = false;

    if (futurePos.x < nekoSize.width) {
        action(Direction::LEFT);
        ret = true;
    } else if (futurePos.x > boundsRect.size.width - nekoSize.width) {
        action(Direction::RIGHT);
        ret = true;
    }

    if (futurePos.y < nekoSize.height) {
        action(Direction::DOWN);
        ret = true;
    } else if (futurePos.y > boundsRect.size.height - nekoSize.height) {
        action(Direction::UP);
        ret = true;
    }
    return ret;
}

#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)

CCPoint touchPos = {0, 0};

void NekoTouchDispatcher::touches(CCSet* touches, CCEvent* event, unsigned int uIndex) {
    auto touch = static_cast<CCTouch*>(touches->anyObject());

    touchPos = touch->getLocation();
    CCTouchDispatcher::touches(touches, event, uIndex);
}

#endif

void NekoNode::update(float dt) {
    auto& bounds = this->m_nekoBounds;
    auto& state = this->m_state;
    auto& rect = bounds->m_rect;
#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_MACOS)
    auto const mousePos = bounds->convertToNodeSpace(geode::cocos::getMousePos());
#else
    auto const mousePos = bounds->convertToNodeSpace(touchPos);
#endif
    auto const vec = mousePos - this->getPosition();
    auto const normVec = vec.normalize();
    auto const pos = this->getPosition();
    CCPoint futurePos = pos + (normVec * this->m_speed) * dt;

    float distance = mousePos.getDistance(futurePos);

    this->m_futurePos = futurePos;
    this->m_mousePos = mousePos;

    if (rect.has_value()) {
        bounds->setContentSize(rect->size);
        bounds->setPosition(rect->origin);
    } else {
        auto& parent = bounds->m_parent;
        auto const contentSize = parent->getContentSize();
        bounds->setContentSize(contentSize);
        bounds->setPosition(contentSize * parent->getAnchorPoint());
    }

    this->handleStates(dt);
    this->updateSprite(vec);
}

void NekoNode::updateSprite(CCPoint const vec) {
    auto const frameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    auto& frameNumber = this->m_frame;
    auto& direction = this->m_direction;
    std::string frameString;
    std::string_view stateString = this->getStateString();

    if (!this->m_directionLock) {
        direction = this->getFrameDirection(vec);
    }

    frameString = fmt::format("{}_{}_{}.png"_spr, stateString, (int)direction, frameNumber);

    if (this->m_state == NekoState::IDLE && (direction != Direction::UP || frameNumber != 0)) {
        log::error("The idle texture is messing up again: {}", frameString);
    }

    auto frame = frameCache->spriteFrameByName(frameString.c_str());
    this->m_nekoSprite->setDisplayFrame(frame);
}

Direction NekoNode::getFrameDirection(CCPoint vec) {
    float const angleRad = vec.getAngle();
    float angle = angleRad * (180 / M_PI);
    float sectorAngle;
    float offset;

    auto updateVarsForDirections = [&sectorAngle, &offset](float const directions) {
        sectorAngle = 360 / directions;
        offset = sectorAngle / 2;
    };

    updateVarsForDirections(8);

    switch (this->m_state) {
        case NekoState::RUNNING:
            updateVarsForDirections(8);
            if (angle >= 0) {  // upwards
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
            } else {
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
        case NekoState::SLEEPING:
            [[fallthrough]];
        case NekoState::TIRED:
            [[fallthrough]];
        case NekoState::SHOCKED:
            [[fallthrough]];
        case NekoState::IDLE:
            return Direction::UP;
        case NekoState::BORDER: {
            updateVarsForDirections(4);

            if (angle >= 0) {  // upwards
                if (angle <= offset)
                    return Direction::RIGHT;
                else if (angle <= sectorAngle + offset)
                    return Direction::UP;
                else
                    return Direction::LEFT;
            } else {
                if (angle <= -180 + offset)
                    return Direction::LEFT;
                else if (angle <= -180 + sectorAngle + offset)
                    return Direction::DOWN;
                else
                    return Direction::RIGHT;
            }
        }
        default:
            log::error("Couldn't get the direction Neko is supposed to look at! State: {}", (int)this->m_state);
            return Direction::UP;
    }
}