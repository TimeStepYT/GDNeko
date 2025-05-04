#include "NekoNode.hpp"

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

    auto scaleFactor = CCDirector::get()->getContentScaleFactor();
    auto& scale = this->m_scale;
    float const scaledContentSize = 280 * scale / scaleFactor;

    this->setID("neko"_spr);
    this->setContentSize({ scaledContentSize, scaledContentSize });
    this->setAnchorPoint(ccp(0.5f, 0.5f));
    this->setPositionX(bounds->getContentWidth() / 2);
    this->setPositionY(bounds->getContentHeight() / 2);
    auto nekoSprite = CCSprite::createWithSpriteFrameName("idle_0_0.png"_spr);
    
    nekoSprite->setID("neko-sprite"_spr);
    nekoSprite->setScale(scale);
    nekoSprite->setPositionX(this->getContentWidth() / 2);
    nekoSprite->setPositionY(this->getContentHeight() / 2);
    
    this->m_nekoSprite = nekoSprite;
    this->m_nekoBounds = bounds;
    this->m_nekoSize = nekoSprite->getContentSize() * this->m_scale / 2;

    this->addChild(nekoSprite);
    scheduleUpdate();

    return true;
}

bool NekoNode::isHittingWall() {
    return this->isHittingWall([](auto) {});
}

bool NekoNode::isHittingWall(auto action) {
    auto& nekoSprite = this->m_nekoSprite;
    auto& futurePos = this->m_futurePos;
    auto& bounds = this->m_nekoBounds;
    auto boundsRect = CCRect(ccp(0, 0), bounds->getContentSize());
    auto& nekoSize = this->m_nekoSize;

    if (futurePos.x < nekoSize.width) {
        action(Direction::LEFT);
        return true;
    }
    else if (futurePos.x > boundsRect.size.width - nekoSize.width) {
        action(Direction::RIGHT);
        return true;
    }
    
    if (futurePos.y < nekoSize.height) {
        action(Direction::DOWN);
        return true;
    }
    else if (futurePos.y > boundsRect.size.height - nekoSize.height) {
        action(Direction::UP);
        return true;
    }
    return false;
}

void NekoNode::handleStates(float dt) {
    auto& state = this->m_state;
    auto& bounds = this->m_nekoBounds;
    auto& mousePos = this->m_mousePos;
    auto boundsRect = CCRect(ccp(0, 0), bounds->getContentSize());

    switch (state) {
        case NekoState::IDLE: [[fallthrough]];
        case NekoState::RUNNING: {
            auto& futurePos = this->m_futurePos;
            auto& nekoSize = this->m_nekoSize;
            this->m_directionLock = false;
            state = NekoState::RUNNING;

            auto action = [&futurePos, nekoSize, boundsRect](Direction direction) {
                switch (direction) {
                    case Direction::LEFT:
                        futurePos.x = nekoSize.width;
                        break;
                    case Direction::RIGHT:
                        futurePos.x = boundsRect.size.width - nekoSize.width;
                        break;
                    case Direction::DOWN:
                        futurePos.y = nekoSize.height;
                        break;
                    case Direction::UP:
                        futurePos.y = boundsRect.size.height - nekoSize.height;
                        break;
                    default: break;
                }
                };

            if (this->isHittingWall(action) && !boundsRect.containsPoint(mousePos)) {
                state = NekoState::BORDER;
            }

            this->setPosition(futurePos);
            break;
        }
        case NekoState::BORDER: {
            bool const mouseReachable = boundsRect.containsPoint(mousePos);
            auto& direction = this->m_direction;
            auto action = [&direction](Direction newDirection) {
                direction = newDirection;
            };
            bool hittingWall = isHittingWall(action);
            this->m_directionLock = true;

            if (mouseReachable || !hittingWall) {
                state = NekoState::RUNNING;
                log::info("{}, {}", mouseReachable, !hittingWall);
            }

            break;
        }
        default:
            break;
    }
    int const maxFrames = 2;
    float const frameChangesPerSecond = this->m_speed / 10;
    float const timeUntilFrameChange = 1.f / frameChangesPerSecond;
    auto& timer = this->m_animTimer;
    auto& frameNumber = this->m_frame;

    if (state != NekoState::IDLE) {
        timer += dt;
        if (timer >= timeUntilFrameChange) {
            timer -= timeUntilFrameChange;
            frameNumber = (frameNumber + 1) % maxFrames;
        }
    }
}

void NekoNode::update(float dt) {
    auto const mousePos = this->m_nekoBounds->convertToNodeSpace(geode::cocos::getMousePos());
    auto const vec = mousePos - this->getPosition();
    auto const normVec = vec.normalize();
    auto const pos = this->getPosition();
    CCPoint futurePos = pos + normVec * this->m_speed * dt;
    auto& state = this->m_state;

    float distance = mousePos.getDistance(futurePos);
    int const deadzone = 17;
    bool const inExitRadius = state == NekoState::IDLE && distance < deadzone + 10; // less eratic small mouse movement

    this->m_futurePos = futurePos;
    this->m_mousePos = mousePos;

    if (distance < deadzone || inExitRadius) {
        state = NekoState::IDLE;
        this->m_frame = 0;
        this->m_directionLock = false;
    }
    else
        this->handleStates(dt);

    this->updateSprite(vec);
}

std::string_view NekoNode::getStateString() {
    switch (this->m_state) {
        case NekoState::RUNNING:
            return "running";
            break;
        case NekoState::IDLE:
            return "idle";
            break;
        case NekoState::BORDER:
            return "border";
        default:
            return "";
    }
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

    frameString = fmt::format("{}_{}_{}.png"_spr, stateString, (int) direction, frameNumber);

    if (this->m_state == NekoState::IDLE && (direction != Direction::UP || frameNumber != 0)) {
        log::info("The idle texture is messing up again: {}", frameString);
    }

    auto frame = frameCache->spriteFrameByName(frameString.c_str());
    this->m_nekoSprite->setDisplayFrame(frame);
}

Direction NekoNode::getFrameDirection(CCPoint vec) {
    float const angleRad = vec.getAngle();
    float angle = angleRad * (180 / M_PI);
    float sectorAngle;
    float offset;

    auto updateVars = [&sectorAngle, &offset](float const directions) {
        sectorAngle = 360 / directions;
        offset = sectorAngle / 2;
        };

    switch (this->m_state) {
        case NekoState::RUNNING:
            updateVars(8);
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
        case NekoState::IDLE:
            return Direction::UP;
        case NekoState::BORDER:
            updateVars(4);
            if (angle >= 0) { // upwards
                if (angle <= offset)
                    return Direction::RIGHT;
                else if (angle <= sectorAngle + offset)
                    return Direction::UP;
                else
                    return Direction::LEFT;
            }
            else {
                if (angle <= -180 + offset)
                    return Direction::LEFT;
                else if (angle <= -180 + sectorAngle + offset)
                    return Direction::DOWN;
                else
                    return Direction::RIGHT;
            }
        default:
            return Direction::UP;
    }
}