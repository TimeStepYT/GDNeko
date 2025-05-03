#ifndef NEKO_BOUNDARY
#define NEKO_BOUNDARY

#include <Geode/Geode.hpp>

class NekoNode;

class NekoBoundary : public cocos2d::CCLayer {
protected:
    NekoNode* m_nekoNode = nullptr; // unused but i think this may be handy later on
    
public:
    static NekoBoundary* create(cocos2d::CCLayer*);
    static void placeWithRect(cocos2d::CCLayer*, cocos2d::CCRect);
    static void place(cocos2d::CCLayer*);
    
    bool init(CCLayer*);

    CCLayer* m_layer = nullptr;
};

#endif