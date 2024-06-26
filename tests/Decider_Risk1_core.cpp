#include "catch.hpp"
#include "../src/Decider_Risk1.h"

using namespace std;

TEST_CASE("Decider_Risk1_core::")
{
    // Decider_Risk1
    Decider_Risk1 decider{};

    // Vector of Boxes 
    vector<Box> boxes{
        Box{0, 0, 10, 10},  // boxId = 0
        Box{1, 1, 10, 10},  // boxId = 1
        Box{2, 2, 10, 10}}; // boxId = 2

    // Positions with different MoveTypes.
    Position posTypeToArrive{0, 0};
    Position posTypeArrive{1, 1};
    Position posTypeToLeave{2, 2};
    Position posTypeLeft{3, 3};

    // Board
    Board board{10, 10, std::move(boxes)};

    // Position{0, 0} has MoveType::to_arrive and Box0. 
    board.changeSpot(posTypeToArrive, BoardNote{0, MoveType::to_arrive}, true);

    // Position{1, 1} has MoveType::arrive and Box1.
    board.changeSpot(posTypeArrive, BoardNote{1, MoveType::to_arrive}, true);
    board.changeSpot(posTypeArrive, BoardNote{1, MoveType::arrive}, true);
    
    // Position{2, 2} has SpotTYpe::to_leave and Box2.
    board.changeSpot(posTypeToLeave, BoardNote{2, MoveType::to_arrive}, true);
    board.changeSpot(posTypeToLeave, BoardNote{2, MoveType::arrive}, true);
    board.changeSpot(posTypeToLeave, BoardNote{2, MoveType::to_leave}, true);

    // There is no Box at posTypeLeft.

    SECTION("Box3 is deciding to move to posTypeToArrive which has a MoveType::to_arrive.")
    {
        SECTION("verify suggestMoveTo() returns false")
        {
            REQUIRE_FALSE(decider.suggestMoveTo(posTypeToArrive, board));
        }
        SECTION("verify getNext() returns the first possiblePositions that is MoveType::to_leave or MoveType::left")
        {
            vector<Position> possiblePositions = {posTypeToArrive, posTypeLeft, posTypeToLeave};
            pair<Position, int> next = decider.getNext(possiblePositions, board);
            REQUIRE(posTypeLeft == next.first);
            REQUIRE(0 == next.second);

            possiblePositions = {posTypeToArrive, posTypeToLeave, posTypeLeft};
            next = decider.getNext(possiblePositions, board);
            REQUIRE(posTypeToLeave == next.first);
            REQUIRE(7 == next.second);
        }
    }
    SECTION("Box3 is deciding to move to posTypeArrive which has a MoveType::arrive.")
    {
        SECTION("verify suggestMoveTo() returns false")
        {
            REQUIRE_FALSE(decider.suggestMoveTo(posTypeArrive, board));
        }
        SECTION("verify getNext() returns the first possiblePositions that is MoveType::to_leave or MoveType::left")
        {
            vector<Position> possiblePositions = {posTypeArrive, posTypeLeft, posTypeToLeave};
            pair<Position, int> next = decider.getNext(possiblePositions, board);
            REQUIRE(posTypeLeft == next.first);
            REQUIRE(0 == next.second);
        }
    }
    SECTION("Box3 is deciding to move to posTypeToLeave which has a MoveType::toLeave.")
    {
        SECTION("verify suggestMoveTo() returns true")
        {
            REQUIRE(decider.suggestMoveTo(posTypeToLeave, board));
        }
        SECTION("possiblePositions starts with posTypeToLeave. It is the first possiblePosition that is MoveType::to_leave or MoveType::left, verify it is returned.")
        {
            vector<Position> possiblePositions = {posTypeToLeave, posTypeLeft, posTypeToLeave};
            pair<Position, int> next = decider.getNext(possiblePositions, board);
            REQUIRE(posTypeToLeave == next.first);
            REQUIRE(7 == next.second);
        }
    }
    SECTION("Box3 is deciding to move to posTypeLeft which has a MoveType::Left.")
    {
        SECTION("verify suggestMoveTo() returns true")
        {
            REQUIRE(decider.suggestMoveTo(posTypeLeft, board));
        }
        SECTION("possiblePositions starts with posTypeLeft. It is the first possiblePosition that is SpoteType::to_leave or MoveType::left, verifty it is returnd.")
        {
            vector<Position> possiblePositions = {posTypeLeft, posTypeLeft, posTypeToLeave};
            pair<Position, int> next = decider.getNext(possiblePositions, board);
            REQUIRE(posTypeLeft == next.first);
            REQUIRE(0 == next.second);
        }
    }
}
