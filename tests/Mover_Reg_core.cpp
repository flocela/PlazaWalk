#include "catch.hpp"
#include "../src/Mover_Reg.h"
#include "../src/NoteAccountant.h"

using namespace std;


/* 
TempListener is a BoardListener that saves the last Drops and Boxes that were received in the last call to receiveChanges().
When receiveChanges() is called, TempListener 1) receives and saves changes to Spots, 2) receives and saves the BoxInfos. Note, changes to Spots are in fact changes since the last time receivedChanges() was called. The BoxInfos received are the current state of the Boxes.
*/
class TempListener : public BoardListener 
{
    public: 

    void receiveChanges(
        unordered_set<Drop> drops,
        unordered_map<int, BoxInfo> boxes) override
    {
        _dropsPerPosition.clear();
        _boxes.clear();

        for(const auto& p: boxes)
        {
            _boxes.insert({p.second.getId(), p.second});
        }

        for(auto& drop : drops)
        {
            _dropsPerPosition.insert({drop.getPosition(), drop});
        }
    }
    
    unordered_map<Position, Drop> _dropsPerPosition{};
    unordered_map<int, BoxInfo> _boxes{};
};

TEST_CASE("Mover_Reg_core::")
{
    SECTION("In addBox() and moveBox(), Mover_Reg adds BoardNotes to Board in the correct order.")
    {
        // Register a NoteAccountant with Board for positions: startPosition and endPosition.
        Position startPosition = {5, 5};
        NoteAccountant startPositionAccountant{};
        Position endPosition = {5, 6};
        NoteAccountant endPositionAccountant{};
         
        vector<Box> boxes{Box{0, 0,  10, 10}};
        Board board{10, 10, std::move(boxes)};
        board.registerNoteSubscriber(startPosition, startPositionAccountant);
        board.registerNoteSubscriber(endPosition, endPositionAccountant);

        // Create Mover_Reg. Add a box to startPosition. Then move that box from startPosition to endPosition.
        Mover_Reg mover{boxes[0].getId(), &board};
        REQUIRE(true == mover.addBox(startPosition));
        REQUIRE(true == mover.moveBox(startPosition, endPosition));

        // Get BoardNotes from accountants per position.
        vector<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>, BoardNote>> startPosNotes = startPositionAccountant.getNotes();
        vector<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>, BoardNote>> endPosNotes = endPositionAccountant.getNotes();

        // Results:

        // startPosition should get 4 BoardNotes: MoveType::to_arrive, MoveType::arrive, MoveType::to_leave, MoveType::left.

        // endPosition should get 2 BoardNotes: MoveType::to_arrive and MoveType::arrive. 
        REQUIRE(4 == startPosNotes.size());

        REQUIRE(2 == endPosNotes.size());

        // From startPosNotes: Box 0 is to_arrive.
        REQUIRE(BoardNote{0,MoveType::to_arrive} == startPosNotes[0].second); 

        // From startPosNotes: Box 0 has arrived.
        REQUIRE(BoardNote{0,MoveType::arrive} == startPosNotes[1].second); 

        // From endPosNotes: Box 0 is to_arrive.
        REQUIRE(BoardNote{0, MoveType::to_arrive} == endPosNotes[0].second); 

        // From startPosNotes: Box 0 is to_leave.
        REQUIRE(BoardNote{0, MoveType::to_leave} == startPosNotes[2].second); 

        // From endPosNotes: Box 0 is arrive.
        REQUIRE(BoardNote{0, MoveType::arrive} == endPosNotes[1].second); 

        // From startPosNotes: Box 0 has left.
        REQUIRE(BoardNote{0, MoveType::left} == startPosNotes[3].second); 
        
        // The BoardNotes should be in order.
        
        // startPosNotes[0] is Box0 is to_arrive, statePosNotes[1] is Box0 arrive.
        REQUIRE(startPosNotes[1].first - startPosNotes[0].first > std::chrono::milliseconds(0) );

        // startPosNotes[1] is Box0 arrive. endPosNotes[0] is Box0 to_arrive.
        REQUIRE(endPosNotes[0].first - startPosNotes[1].first > std::chrono::milliseconds(0) );

        // endPosNotes[0] is Box0 to_arrive. startPosNotes[2] is Box0 to_leave.
        REQUIRE(startPosNotes[2].first - endPosNotes[0].first > std::chrono::milliseconds(0) );

        // startPosNotes[2] is Box0 to_leave. endPosNotes[1] is Box0 arrive.
        REQUIRE(endPosNotes[1].first - startPosNotes[2].first > std::chrono::milliseconds(0) );

        // endPosNotes[1] is Box0 arrive. startPosNotes[3] is Box0 left.
        REQUIRE(startPosNotes[3].first - endPosNotes[1].first > std::chrono::milliseconds(0) );
    }
    
    SECTION("Mover_Reg removes box from board.")
    {
        // Set up Board, Box, and Mover_Reg.
        vector<Box> boxes{Box{0, 0,  10, 10}};
        Board board{10, 10, std::move(boxes)};
        Mover_Reg mover{boxes[0].getId(), &board};

        // Register a NoteAccountant to receive changes from Board's positionsFrom.
        Position startPosition = {5, 5};
        NoteAccountant accountantForPosA{};

        // Add box to startPosition
        REQUIRE(true == mover.addBox(startPosition));
        REQUIRE(BoardNote{0, MoveType::arrive} ==  board.getNoteAt(startPosition));

        // Remove box from startPosition
        REQUIRE(true == mover.removeBox(startPosition));
        REQUIRE(BoardNote{-1, MoveType::left} ==  board.getNoteAt(startPosition));
    } 
    
    SECTION("Adding Box to a Position that already has a Box returns false and will not increase the Boxes' levels.")
    {
        Position position = {5, 5};
       
        // Two boxes. 
        vector<Box> boxes{Box{0, 0,  10, 10}, Box{1, 0, 10, 10}};
        Board board{10, 10, std::move(boxes)};
        TempListener listener{};
        board.registerListener(&listener);

        Mover_Reg mover0{boxes[0].getId(), &board};
        Mover_Reg mover1{boxes[1].getId(), &board};
        
        mover0.addBox(position);

        REQUIRE_FALSE(mover1.addBox(position));
        board.sendStateAndChanges();

        REQUIRE(0 == listener._boxes.at(0).getLevel());
        REQUIRE(0 == listener._boxes.at(1).getLevel());
        
    }
         
    SECTION("Moving Box to a Position that already has a Box returns false and will increase the Boxes' levels.")
    {
        Position positionA = {5, 5};
        Position positionB = {6, 6};
       
        // Two boxes. 
        vector<Box> boxes{Box{0, 0,  10, 10}, Box{1, 0, 10, 10}};
        Board board{10, 10, std::move(boxes)};
        TempListener listener{};
        board.registerListener(&listener);

        Mover_Reg mover0{boxes[0].getId(), &board};
        Mover_Reg mover1{boxes[1].getId(), &board};
        
        mover0.addBox(positionA);
        mover1.addBox(positionB);

        REQUIRE_FALSE(mover0.moveBox(positionA, positionB));

        board.sendStateAndChanges();

        REQUIRE(1 == listener._boxes.at(0).getLevel());
        REQUIRE(1 == listener._boxes.at(1).getLevel());
    }
}
    
