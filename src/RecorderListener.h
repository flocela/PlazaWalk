#ifndef RECORDERLISTENER__H
#define RECORDERLISTENER__H

#include <unordered_map>
#include <unordered_set>
#include "BoxInfo.h"
#include "Drop.h"

class RecorderListener 
{
    public:
    
    /*
    Receives all the Drops that contain Boxes and all the Boxes, even Boxes that have not entered the Board or have been taken off the Board.
    */
    virtual void receiveAllDropsAllBoxes(std::unordered_set<Drop> drops,
                                         std::unordered_map<int, BoxInfo> boxes) = 0;
};

#endif

