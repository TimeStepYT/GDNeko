#ifndef NEKO_NODE
#define NEKO_NODE

#include <Geode/Geode.hpp>
#include "NekoBoundary.hpp"

class NekoNode : public cocos2d::CCNode {
protected:
    cocos2d::CCSprite* m_nekoSprite = nullptr;
    NekoBoundary* m_nekoBoundary = nullptr;
    float m_scale = 0.5f;

public:
    static NekoNode* create(NekoBoundary*);

    bool init(NekoBoundary*);
};

#endif