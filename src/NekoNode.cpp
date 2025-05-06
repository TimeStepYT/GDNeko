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
    this->setPosition(bounds->getContentSize() / 2);
    auto nekoSprite = CCSprite::createWithSpriteFrameName("idle_0_0.png"_spr);

    nekoSprite->setID("neko-sprite"_spr);
    nekoSprite->setScale(scale);
    nekoSprite->setPosition(this->getContentSize() / 2);

#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    auto director = CCDirector::get();
    director->getTouchDispatcher()->addStandardDelegate(this, 999999);

#endif

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

    bool ret = false;

    if (futurePos.x < nekoSize.width) {
        action(Direction::LEFT);
        ret = true;
    }
    else if (futurePos.x > boundsRect.size.width - nekoSize.width) {
        action(Direction::RIGHT);
        ret = true;
    }

    if (futurePos.y < nekoSize.height) {
        action(Direction::DOWN);
        ret = true;
    }
    else if (futurePos.y > boundsRect.size.height - nekoSize.height) {
        action(Direction::UP);
        ret = true;
    }
    return ret;
}

void NekoNode::handleStates(float dt) {
    auto& state = this->m_state;
    auto& bounds = this->m_nekoBounds;
    auto& mousePos = this->m_mousePos;
    auto& futurePos = this->m_futurePos;
    auto& timer = this->m_animTimer;
    auto& directionLock = this->m_directionLock;
    auto boundsRect = CCRect(ccp(0, 0), bounds->getContentSize());
    float const distance = mousePos.getDistance(futurePos);

    switch (state) {
        case NekoState::IDLE:
            if (distance < this->m_happyRadius) {
                if (timer >= 7) {
                    timer = 0;
                    state = NekoState::TIRED;
                }
            }
            else {
                timer = 0;
                state = NekoState::SHOCKED;
            }

            directionLock = false;

            break;
        case NekoState::SHOCKED:
            if (timer >= 1) {
                timer = 0;
                state = NekoState::RUNNING;
            }
            if (distance < this->m_happyRadius)
                state = NekoState::IDLE;
            break;
        case NekoState::TIRED:
            if (timer >= 1) {
                timer = 0;
                state = NekoState::SLEEPING;
            }
            [[fallthrough]];
        case NekoState::SLEEPING:
            if (distance >= this->m_happyRadius) {
                timer = 0;
                state = NekoState::SHOCKED;
            }
            break;
        case NekoState::RUNNING: {
            auto& nekoSize = this->m_nekoSize;
            directionLock = false;
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

            if (distance < this->m_happyRadius) {
                state = NekoState::IDLE;
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
            }

            break;
        }
        default:
            break;
    }
    int const maxFrames = 2;
    float const frameChangesPerSecond = this->m_speed / 10;
    float timeUntilFrameChange;
    auto& frameNumber = this->m_frame;

    if (dt > 2) // I don't want it to explode when debugging or something
        dt = 2;
    timer += dt;

    // Making sure it doesn't increase the animation frame if there aren't multiple
    switch (state) {
        case NekoState::IDLE: [[fallthrough]];
        case NekoState::TIRED: [[fallthrough]];
        case NekoState::SHOCKED:
            frameNumber = 0;
            break;
        default:
            if (state == NekoState::SLEEPING)
                timeUntilFrameChange = 0.5;
            else
                timeUntilFrameChange = 1 / frameChangesPerSecond;
            while (timer >= timeUntilFrameChange) {
                timer -= timeUntilFrameChange;
                frameNumber = (frameNumber + 1) % maxFrames;
            }
            break;
    }
}

#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)

void NekoNode::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent) {
    log::info("{}", pTouch->getLocation());
    this->m_mousePos = pTouch->getLocation();
}

void NekoNode::onExit() {
    auto director = CCDirector::get();
    director->getTouchDispatcher()->removeDelegate(this);
    CCNode::onExit();
}

#endif

void NekoNode::update(float dt) {
    auto& bounds = this->m_nekoBounds;
    auto& state = this->m_state;
    auto& rect = bounds->m_rect;
#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_MACOS)
    auto const mousePos = bounds->convertToNodeSpace(geode::cocos::getMousePos());
#else
    auto& mousePos = this->m_mousePos;
#endif
    auto const vec = mousePos - this->getPosition();
    auto const normVec = vec.normalize();
    auto const pos = this->getPosition();
    CCPoint futurePos = pos + normVec * this->m_speed * dt;

    float distance = mousePos.getDistance(futurePos);

    this->m_futurePos = futurePos;
#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_MACOS)
    this->m_mousePos = mousePos;
#endif
    if (rect.has_value()) {
        bounds->setContentSize(rect->size);
        bounds->setPosition(rect->origin);
    }
    else {
        auto& parent = bounds->m_parent;
        auto const contentSize = parent->getContentSize();
        bounds->setContentSize(contentSize);
        bounds->setPosition(contentSize * parent->getAnchorPoint());
    }

    this->handleStates(dt);
    this->updateSprite(vec);
}

std::string_view NekoNode::getStateString() {
    switch (this->m_state) {
        case NekoState::RUNNING:
            return "running";
        case NekoState::IDLE:
            return "idle";
        case NekoState::BORDER:
            return "border";
        case NekoState::SLEEPING:
            return "sleeping";
        case NekoState::TIRED:
            return "tired";
        case NekoState::SHOCKED:
            return "shocked";
        default:
            log::error("Couldn't get Neko's state string: State {}", (int) this->m_state);
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
        case NekoState::SLEEPING: [[fallthrough]];
        case NekoState::TIRED: [[fallthrough]];
        case NekoState::SHOCKED: [[fallthrough]];
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
            log::error("Couldn't get the direction Neko is supposed to look at! State: {}", (int) this->m_state);
            return Direction::UP;
    }
}