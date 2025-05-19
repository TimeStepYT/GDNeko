#include "NekoNode.hpp"

using namespace geode::prelude;

void NekoNode::idle() {
    auto& timer = this->m_animTimer;
    auto& state = this->m_state;
    auto& mousePos = this->m_mousePos;
    auto& futurePos = this->m_futurePos;
    float const distance = mousePos.getDistance(futurePos);

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

    this->m_directionLock = false;
}

void NekoNode::shocked() {
    auto& timer = this->m_animTimer;
    auto& state = this->m_state;
    auto& mousePos = this->m_mousePos;
    auto& futurePos = this->m_futurePos;
    float const distance = mousePos.getDistance(futurePos);

    if (timer >= 0.5f) {
        timer = 0;
        state = NekoState::RUNNING;
    }
    if (distance < this->m_happyRadius)
        state = NekoState::IDLE;
}

void NekoNode::tired() {
    auto& timer = this->m_animTimer;

    if (timer < 1) return;

    timer = 0;
    this->m_state = NekoState::SLEEPING;
}

void NekoNode::sleeping() {
    auto& mousePos = this->m_mousePos;
    auto& futurePos = this->m_futurePos;
    float const distance = mousePos.getDistance(futurePos);

    if (distance < this->m_happyRadius) return;

    this->m_animTimer = 0;
    this->m_state = NekoState::SHOCKED;
}

void NekoNode::running() {
    auto& nekoSize = this->m_nekoSize;
    auto& state = this->m_state;
    auto& futurePos = this->m_futurePos;
    auto& bounds = this->m_nekoBounds;
    auto& mousePos = this->m_mousePos;
    auto boundsRect = CCRect(ccp(0, 0), bounds->getContentSize());
    float const distance = mousePos.getDistance(futurePos);

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

    auto hittingWall = this->isHittingWall(action);
    auto mouseOutOfBounds = !boundsRect.containsPoint(mousePos);
    if (hittingWall && mouseOutOfBounds) {
        state = NekoState::BORDER;
    }

    if (distance < this->m_happyRadius) {
        state = NekoState::IDLE;
    }

    this->setPosition(futurePos);
}

void NekoNode::border() {
    auto& mousePos = this->m_mousePos;
    auto& bounds = this->m_nekoBounds;
    auto boundsRect = CCRect(ccp(0, 0), bounds->getContentSize());
    bool const mouseReachable = boundsRect.containsPoint(mousePos);
    auto& direction = this->m_direction;

    auto action = [&direction](Direction newDirection) {
        direction = newDirection;
        };

    bool hittingWall = isHittingWall(action);
    this->m_directionLock = true;

    if (mouseReachable || !hittingWall) {
        this->m_state = NekoState::RUNNING;
    }
}

void NekoNode::handleStates(float dt) {
    auto& state = this->m_state;
    auto& timer = this->m_animTimer;

    switch (state) {
        case NekoState::IDLE:
            this->idle();
            break;
        case NekoState::SHOCKED:
            this->shocked();
            break;
        case NekoState::TIRED:
            this->tired();
            [[fallthrough]];
        case NekoState::SLEEPING:
            this->sleeping();
            break;
        case NekoState::RUNNING:
            this->running();
            break;
        case NekoState::BORDER:
            this->border();
            break;
        default:
            break;
    }
    int const maxFrames = 2;
    float const frameChangesPerSecond = this->m_speed / 15;
    float timeUntilFrameChange;
    auto& frameNumber = this->m_frame;

    if (dt > 1) // No exploding for really bad pcs with over 1 second long lags
        dt = 1;
    timer += dt;

    // Making sure it doesn't increase the animation frame if there aren't multiple frames to choose from
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