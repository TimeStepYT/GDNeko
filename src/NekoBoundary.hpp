#ifndef NEKO_BOUNDARY
#define NEKO_BOUNDARY

#include <Geode/Geode.hpp>

class NekoNode;

class NekoBoundary : public cocos2d::CCNode {
protected:
    NekoNode* m_nekoNode = nullptr; // unused but i think this may be handy later on
    
public:
    static NekoBoundary* create(cocos2d::CCNode*);
    static void placeWithRect(cocos2d::CCNode*, cocos2d::CCRect);
    static void place(cocos2d::CCNode*);
    
    bool init(CCNode*);

};

#endif