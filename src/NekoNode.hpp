#ifndef NEKO_NODE
#define NEKO_NODE

#include <Geode/Geode.hpp>
#include "NekoBounds.hpp"

typedef enum {
    IDLE,
    RUNNING,
    BORDER,
    SLEEPING,
    TIRED,
    SHOCKED
} NekoState;

typedef enum {
    UP,
    UP_RIGHT,
    RIGHT,
    DOWN_RIGHT,
    DOWN,
    DOWN_LEFT,
    LEFT,
    UP_LEFT
} Direction;

#if defined(GEODE_IS_WINDOWS) || defined(GEODE_IS_MACOS)
class NekoNode : public cocos2d::CCNode {
#else
class NekoNode : public cocos2d::CCNode, cocos2d::CCStandardTouchDelegate {
#endif
protected:
    cocos2d::CCSprite* m_nekoSprite = nullptr;
    cocos2d::CCPoint m_futurePos;
    cocos2d::CCPoint m_mousePos;
    cocos2d::CCSize m_nekoSize;
    NekoBounds* m_nekoBounds = nullptr;
    NekoState m_state = NekoState::RUNNING;
    Direction m_direction = Direction::UP;
    float m_happyRadius = 17.f;
    float m_scale = 0.5f;
    float m_speed = 100.f;
    float m_animTimer = 0;
    bool m_directionLock = false;
    int m_frame = 0;

public:
    static NekoNode* create(NekoBounds*);

    void update(float) override;
    void updateSprite(cocos2d::CCPoint const);
    void handleStates(float);
    bool init(NekoBounds*);
    bool isHittingWall(auto);
    bool isHittingWall();
    Direction getFrameDirection(cocos2d::CCPoint);
    std::string_view getStateString();

    #if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    void ccTouchMoved(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
    void onExit() override;
    #endif
};

#endif