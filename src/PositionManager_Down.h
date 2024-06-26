#ifndef POSITION_MANAGER_DOWN__H
#define POSITION_MANAGER_DOWN__H

#include "PositionManager.h"

/*
PositionManager_Down's target is a horizontal line at finalY. If a Box's Position is at or below finalY, then it has reached its target.
*/
class PositionManager_Down : public PositionManager 
{
    public:

    PositionManager_Down(int finalY, int boardMinX, int boardMaxX, int boardMinY, int boardMaxY);
    PositionManager_Down() = delete;
    PositionManager_Down(const PositionManager_Down& o) = default;
    PositionManager_Down(PositionManager_Down&& o) noexcept = default;
    PositionManager_Down& operator=(const PositionManager_Down& o) = default;
    PositionManager_Down& operator=(PositionManager_Down&& o) noexcept = default;
    ~PositionManager_Down() = default;


    /*
    Returns a vector of three Positions. Say @position's x-value is x and y-value is y. Then the Positions will be {{Position{x, y+1}, Position{x-1, y}, Position{x+1, y}}. The first Position moves the Box closer to the end line. The second and third Positions move horizontally parallel to the end line. Positions that are not on the Board, are deleted from the returned vector. If @position's y value is larger than or equal to finalY, then returns an empty vector; It has reached its end target.
    */
    std::vector<Position> getFuturePositions(Position position) override;

    
    /*
    Returns true if @position's y value is greater than or equal to finalY.
    */
    bool atEnd(Position position) const override;


    /*
    Returns a Rectangle the with a y-value of finalY and width of the Board. Rectangle's top left corner would be {boardMinX, finalY}, and bottom right corner would be {boardMaxX, finalY}. Notice the Rectangle has a height of zero.
    */
    Rectangle getEndRect() const override;


    /*
    Returns a Rectangle the with a y-value of finalY and width of the Board. Rectangle's top left corner would be {boardMinX, finalY}, and bottom right corner would be {boardMaxX, finalY}. Notice the Rectangle has a height of zero.
    */
    Rectangle getTargetRect() const override;


    private:

    int _endY;

    // edges of the board
    int _boardMinX = 0;
    int _boardMaxX = 0;
    int _boardMinY = 0;
    int _boardMaxY = 0;

    /*
    Returns true if @p is on the Board.
    */
    bool isValid(Position& p) const;

    std::string invalidPositionErrorString(Position p) const;
};

#endif
    
