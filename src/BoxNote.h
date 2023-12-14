#ifndef BOXNOTE__H
#define BOXNOTE__H

#include "Position.h"

class BoxNote
{

public:
    BoxNote(int type, Position toPosition, Position fromPosition);
    BoxNote() = delete;
    BoxNote(const BoxNote& o) = default;
    BoxNote(BoxNote&& o) noexcept = default;
    BoxNote& operator=(const BoxNote& o) = delete;
    BoxNote& operator=(BoxNote&& o) noexcept = delete;

    int getType() const;
    Position getToPosition() const;
    Position getFromPosition() const; 
    
private:
    const int _type = 0;
    const Position _toPosition;
    const Position _fromPosition;

};
#endif