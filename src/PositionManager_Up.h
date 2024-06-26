#ifndef POSITION_MANAGER_UP__H
#define POSITION_MANAGER_UP__H

#include "PositionManager.h"

/*
PositionManager_Down's target is a horizontal line at finalY. If a Box's Position is at or above finalY, then it has reached its target.
*/
class PositionManager_Up : public PositionManager
{
    public:
    PositionManager_Up(int finalY, int boardMinX, int boardMaxX, int boardMinY, int boardMaxY);
    PositionManager_Up() = delete;
    PositionManager_Up(const PositionManager_Up& o) = default;
    PositionManager_Up(PositionManager_Up&& o) noexcept = default;
    PositionManager_Up& operator=(const PositionManager_Up& o) = default;
    PositionManager_Up& operator=(PositionManager_Up&& o) noexcept = default;
    ~PositionManager_Up() = default;

    /*
    Returns a vector of three Positions. Say @position's x-value is x and y-value is y. Then the Positions will be {{Position{x, y-1}, Position{x-1, y}, Position{x+1, y}}. The first Position moves the box closer to the end line. The second and third Positions move horizontally parallel to the end line. Positions that are not on the Board, are deleted from the returned vector. If @position's y value is less than or equal to finalY, then returns an empty vector.
    */
    std::vector<Position> getFuturePositions(Position position) override;
   
 
    /*
    Returns true if @position's y value is less than or equal to final Y.
    */
    bool atEnd(Position position) const override;


    /*
    Returns a Rectangle the with a y-value of finalY and the width of the Board. Rectangle's top left corner would be {boardMinX, finalY}, and bottom right corner would be {boardMaxX, finalY}. Notice the Rectangle has a height of zero.
    */
    Rectangle getEndRect() const override;


    /*
    Returns a Rectangle the with a y-value of finalY and the width of the Board. Rectangle's top left corner would be {boardMinX, finalY}, and bottom right corner would be {boardMaxX, finalY}. Notice the Rectangle has a height of zero.
    */
    Rectangle getTargetRect() const override;

private:
    int _endY;
    int _boardMinX;
    int _boardMaxX;
    int _boardMinY;
    int _boardMaxY;

    bool isValid(Position& p) const;
    std::string invalidPositionErrorString(Position p) const;
};

#endif
