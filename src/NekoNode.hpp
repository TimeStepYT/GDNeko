#ifndef NEKO_NODE
#define NEKO_NODE

#include <Geode/Geode.hpp>
#include "NekoBounds.hpp"

typedef enum {
    IDLE,
    RUNNING,
    BORDER
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

class NekoNode : public cocos2d::CCNode {
protected:
    cocos2d::CCSprite* m_nekoSprite = nullptr;
    NekoBounds* m_nekoBounds = nullptr;
    float m_scale = 0.5f;
    float m_speed = 100.f;
    float m_animTimer = 0;
    NekoState m_state = NekoState::IDLE;
    int m_frame = 0;

public:
    static NekoNode* create(NekoBounds*);

    bool init(NekoBounds*);
    void update(float) override;
    Direction getFrameDirection(cocos2d::CCPoint);
};

#endif