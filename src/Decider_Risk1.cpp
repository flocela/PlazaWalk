#include "Decider_Risk1.h"

#include "MoveType.h"
#include "Util.h"

using namespace std;

bool Decider_Risk1::suggestMoveTo(Position position, const Board& board)
{
    BoardNote note = board.getNoteAt(position);
    return ((note.getType() == MoveType::left) ||
           (note.getType() == MoveType::to_leave));
}

pair<Position, int> Decider_Risk1::getNext(
    const vector<Position> possiblePositions,
    const Board& board
    )
{
    // Take each position in possiblePositions
    for (const Position& position : possiblePositions)
    {
        // BoardNote at position will tell us what box (if any) is currently at that Position. 
        BoardNote boardNote = board.getNoteAt(position);

        if (boardNote.getType() == MoveType::left)
        {
            return {position, 0};
        }
        if (boardNote.getType() == MoveType::to_leave)
        {
            return {position, 7};
        }
    }

    return {Position{-1, -1}, -1};
} 

