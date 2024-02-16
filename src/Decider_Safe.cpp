#include "Decider_Safe.h"

using namespace std;

Position Decider_Safe::getNextPosition(
    vector<Position> possiblePositions,
    const Board& board
    )
{
    // Take each position in possiblePositions
    for (const Position& position : possiblePositions)
    {
        cout << "Decider_Safe position: " << position << endl;
        // BoardNote at position will tell us what box (if any) is currently at that postion. 
        BoardNote boardNote = board.getNoteAt(position);

        // if the BoardNote's type is empty, then return current position. 
        if (boardNote.getType() == -1)
        {
            return position;
        }       
    }
    
    return Position{-1, -1};
} 

