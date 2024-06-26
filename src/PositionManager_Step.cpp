#include "PositionManager_Step.h"
#include <random>

using namespace std;

PositionManager_Step::PositionManager_Step(
    Position finalTarget,
    int boardMinX,
    int boardMaxX,
    int boardMinY,
    int boardMaxY)
:   _finalTarget{finalTarget},
    _boardMinX{boardMinX},
    _boardMaxX{boardMaxX},
    _boardMinY{boardMinY},
    _boardMaxY{boardMaxY}
{}

vector<Position> PositionManager_Step::getFuturePositions(Position position)
{
    if (!isValid(position))
    {
       throw invalid_argument(invalidPositionErrorString(position));
    }    

    if (atEnd(position))
    {
        return vector<Position>{};
    }

    setCurrentTarget(position);
    
    int pX = position.getX();
    int pY = position.getY();

    // Populate pairsOfPositionsAndDistSq with Positions that are adjacent to @position.
    // The distance is the distance from _curTarget to @position.
    vector<pair<double, Position>> pairsOfPositionsAndDistSq{};
    Position n  = Position{pX, pY-1};
    Position nw = Position{pX + 1, pY - 1};
    Position w  = Position{pX + 1, pY};
    Position sw = Position{pX + 1, pY + 1};
    Position s  = Position{pX, pY + 1};
    Position se = Position{pX - 1, pY + 1};
    Position e  = Position{pX - 1, pY};
    Position ne = Position{pX - 1, pY - 1};
    Position target = _curTarget;
    pairsOfPositionsAndDistSq.push_back({getDistSquared(n, target), n}); 
    pairsOfPositionsAndDistSq.push_back({getDistSquared(nw, target), nw}); 
    pairsOfPositionsAndDistSq.push_back({getDistSquared(w, target), w}); 
    pairsOfPositionsAndDistSq.push_back({getDistSquared(sw, target), sw}); 
    pairsOfPositionsAndDistSq.push_back({getDistSquared(s, target), s}); 
    pairsOfPositionsAndDistSq.push_back({getDistSquared(se, target), se}); 
    pairsOfPositionsAndDistSq.push_back({getDistSquared(e, target), e}); 
    pairsOfPositionsAndDistSq.push_back({getDistSquared(ne, target), ne}); 

    // Sort the pairs of positions and distances.
    sort(pairsOfPositionsAndDistSq.begin(), pairsOfPositionsAndDistSq.end(), [](const pair<double, Position>& a, const pair<double, Position>& b){return a.first < b.first;});

    // Remove and Positions that are invalid
    vector<Position> netPositions{};
    for (const auto& p: pairsOfPositionsAndDistSq)
    {
        Position pos = p.second;
        if(isValid(pos))
        {
            netPositions.push_back(pos);
        }
    }

    // Shuffle the Positions after index 2.
    random_device rd;
    mt19937 g(rd());
    shuffle(netPositions.begin()+3, netPositions.end(), g);

    return netPositions;
}

bool PositionManager_Step::atEnd(Position curPosition) const
{
    return curPosition == _finalTarget;
}

Rectangle PositionManager_Step::getEndRect() const
{
    return Rectangle{_finalTarget, _finalTarget};
}

Rectangle PositionManager_Step::getTargetRect() const
{
    return Rectangle{_finalTarget, _finalTarget};
}

void PositionManager_Step::setCurrentTarget(Position curPosition)
{
    // Set a new target if the _curTarget hasn't been set, or 
    // if the box has reached the _curTarget.
    if (_curTarget == Position{-1, -1} || curPosition == _curTarget)
    {
        int deltaX = _finalTarget.getX() - curPosition.getX();
        int deltaY = _finalTarget.getY() - curPosition.getY();

        // If the _curTarget is close to the _finalTarget then set the _curTarget to _finalTarget.
        // Or if target has the same X or Y value as curPosition.
        if( ( ( (deltaX*deltaX)+(deltaY*deltaY) ) < 100 ) ||
            deltaX == 0 ||
            deltaY == 0 )
        {
            _curTarget = _finalTarget;
        }
        // Else set the _currentTarget to the first Position on the line connecting the curPosition and the _finalTarget.
        else
        {  
            // Move one point over on the x axis and find the corresponding point on the y axis.
            if(std::abs(deltaX) <= std::abs(deltaY))
            {
                int smallDeltaX = (deltaX > 0) ? 1 : -1; 
                int x = curPosition.getX() + smallDeltaX;
                double tempY = curPosition.getY() + static_cast<double>(smallDeltaX) * deltaY/deltaX;
                _curTarget = Position(x, static_cast<int>(round(tempY)));
            }
            // Move one point over on the y axis and find the corresponding point on the x axis.
            else
            {
                int smallDeltaY = (deltaY > 0) ? 1 : -1; 
                int y = curPosition.getY() + smallDeltaY;
                double tempX = curPosition.getX() + static_cast<double>(smallDeltaY) * deltaX/deltaY;
                _curTarget = Position(static_cast<int>(round(tempX)), y);
            }
        }
    }
}

double PositionManager_Step::getDistSquared(Position a, Position b) const
{
    double x = std::abs(static_cast<double>(a.getX()) - b.getX());
    double y = std::abs(static_cast<double>(a.getY()) - b.getY());

    return (x * x) + (y * y);
}

bool PositionManager_Step::isValid(Position& p) const
{
    return  (p.getX() >= _boardMinX &&
             p.getX() <= _boardMaxX &&
             p.getY() >= _boardMinY &&
             p.getY() <= _boardMaxY);
}

string PositionManager_Step::invalidPositionErrorString(Position p) const
{
    return  p.toString() + " is an invalid Position.";
}
