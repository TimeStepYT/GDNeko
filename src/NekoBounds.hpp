#ifndef NEKO_BOUNDS
#define NEKO_BOUNDS

#include <Geode/Geode.hpp>

class NekoNode;

class NekoBounds : public cocos2d::CCNode {
protected:
    NekoNode* m_nekoNode = nullptr; // unused but i think this may be handy later on
    
public:
    static NekoBounds* create(cocos2d::CCNode*);
    static NekoBounds* create(cocos2d::CCNode*, cocos2d::CCRect);
    static void placeWithRect(cocos2d::CCNode*, cocos2d::CCRect);
    static void place(cocos2d::CCNode*);
    
    bool init(CCNode*, cocos2d::CCRect);

};

#endif