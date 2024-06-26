#include "catch.hpp"
#include "../src/Board.h"
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

TEST_CASE("Board_core::")
{
    // BoxId's 0, 1, and 2.    
    int boxId_0 = 0;
    int boxId_1 = 1;
    int boxId_2 = 2;

    // Create Boxes with id=0, id=1, and id=2.
    vector<Box> boxes{
        Box{boxId_0, 0, 5, 5},
        Box{boxId_1, 1, 5, 5},
        Box{boxId_2, 2, 5, 5}};

    // Create Board.
    Board board{20, 20, std::move(boxes)};

    // Add Listener.
    TempListener listener{};
    board.registerListener(&listener);
    
    // Positions.    
    Position posA{Position{5, 5}};
    Position posB{Position{6, 6}};
    Position posC{Position{7, 7}};
    
    SECTION("Insert BoardNotes with different types at posA. Verifty the getNoteAt() method returns the new BoardNote after each insertion.")
    {  
        board.changeSpot(posA, BoardNote{boxId_0, MoveType::to_arrive}, true);
        REQUIRE(board.getNoteAt(posA) == BoardNote{boxId_0, MoveType::to_arrive});

        board.changeSpot(posA, BoardNote{boxId_0, MoveType::arrive}, true);
        REQUIRE(board.getNoteAt(posA) == BoardNote{boxId_0, MoveType::arrive});

        board.changeSpot(posA, BoardNote{boxId_0, MoveType::to_leave}, true);
        REQUIRE(board.getNoteAt(posA) == BoardNote{boxId_0, MoveType::to_leave});

        board.changeSpot(posA, BoardNote{boxId_0, MoveType::left}, true);
        REQUIRE(board.getNoteAt(posA) == BoardNote{-1, MoveType::left});

    }


    SECTION("Verify changes to Board's Spots are sent to the BoardListener when board.sendStateAndChanges() is called.")
    {

        board.changeSpot(posA, BoardNote{boxId_0, MoveType::to_arrive}, true);
        board.changeSpot(posB, BoardNote{boxId_1, MoveType::to_arrive}, true);

        board.sendStateAndChanges();
        
        REQUIRE(listener._dropsPerPosition.size() == 2);

        Drop drop0 = listener._dropsPerPosition.at(posA);
        Drop drop1 = listener._dropsPerPosition.at(posB);

        REQUIRE(MoveType::to_arrive == drop0.getMoveType());
        REQUIRE(boxId_0 == drop0.getBoxId());

        REQUIRE(MoveType::to_arrive == drop1.getMoveType());
        REQUIRE(boxId_1 == drop1.getBoxId());

        // Again,
        // add BoardNotes to Board, request Board send changes. Verify BoardListener received changes.
        board.changeSpot(posA, BoardNote{boxId_0, MoveType::arrive}, true);
        board.changeSpot(posC, BoardNote{boxId_2, MoveType::to_arrive}, true); 

        board.sendStateAndChanges();
        
        REQUIRE(listener._dropsPerPosition.size() == 2);
        
        Drop dp0 = listener._dropsPerPosition.at(posA);
        Drop drop2 = listener._dropsPerPosition.at(posC);
        
        REQUIRE(MoveType::arrive == dp0.getMoveType());
        REQUIRE(boxId_0 == dp0.getBoxId());

        REQUIRE(MoveType::to_arrive == drop2.getMoveType());
        REQUIRE(boxId_2 == drop2.getBoxId());
    }

    SECTION("When changeSpots() is unsuccessful verify 1) changeSpots() returns false and 2) both Boxes' levels go up because upLevel argument is true. ")
    {
        // Add Box0 to posA.
        board.changeSpot(posA, BoardNote{boxId_0, MoveType::to_arrive}, true);

        // Try to add Box1 to posA.
        bool isSuccessful = board.changeSpot(posA, BoardNote{boxId_1, MoveType::to_arrive}, true);

        // Verify isSuccessful. Note both boxes' levels go up by 1.
        REQUIRE(false == isSuccessful);
        
        // Again, try to add Box1 to posA.
        isSuccessful = board.changeSpot(posA, BoardNote{boxId_1, MoveType::to_arrive}, true); 

        // Verify isSuccessful is false again. Both Boxes' levels go up by one again.
        REQUIRE(false == isSuccessful);

        // Call Board's sendStateAndChanges method().
        board.sendStateAndChanges();

        // Verify Boxes' levels are 2.
        REQUIRE(2 == listener._boxes.at(0).getLevel());
        REQUIRE(2 == listener._boxes.at(1).getLevel());
    }

    SECTION("When changeSpots() is unsuccessful verify 1) changeSpots() returns false and 2) both Boxes' levels do not go up because upLevel argument is false. ")
    {
        // Add Box0 to posA.
        board.changeSpot(posA, BoardNote{boxId_0, MoveType::to_arrive}, true);

        // Try to add Box1 to posA.
        bool isSuccessful = board.changeSpot(posA, BoardNote{boxId_1, MoveType::to_arrive}, false);

        // Verify isSuccessful. Note both boxes' levels stay the same.
        REQUIRE(false == isSuccessful);
        
        // Again, try to add Box1 to posA.
        isSuccessful = board.changeSpot(posA, BoardNote{boxId_1, MoveType::to_arrive}, false); 

        // Verify isSuccessful is false again. Both Boxes' levels stay the same again.
        REQUIRE(false == isSuccessful);

        // Call Board's sendStateAndChanges method().
        board.sendStateAndChanges();

        // Verify Boxes' levels are 2.
        REQUIRE(0 == listener._boxes.at(0).getLevel());
        REQUIRE(0 == listener._boxes.at(1).getLevel());
    }

    SECTION("Verifty adding a BoardNote with an unknown BoxId results in an exception.")
    {
        // Board does not have a Box id of 100.
        REQUIRE_THROWS(board.changeSpot(posA, BoardNote{100, MoveType::to_arrive}, true));
    }
    
    SECTION("Verifty getBoardProxy() returns a working BoardProxy.")
    {
        BoardProxy boardProxy = board.getBoardProxy();

        board.changeSpot(posA, BoardNote{boxId_0, MoveType::to_arrive}, true);
        board.changeSpot(posB, BoardNote{boxId_1, MoveType::to_arrive}, true);

        // BoardProxy calls Board's sendStateAndChanges() method.
        boardProxy.sendChanges();

        Drop drop0 = listener._dropsPerPosition.at(posA);
        Drop drop1 = listener._dropsPerPosition.at(posB);

        REQUIRE(listener._dropsPerPosition.size() == 2);

        REQUIRE(MoveType::to_arrive == drop0.getMoveType());
        REQUIRE(boxId_0 == drop0.getBoxId());

        REQUIRE(MoveType::to_arrive == drop1.getMoveType());
        REQUIRE(boxId_1 == drop1.getBoxId());
    }

    SECTION("Verify NoteSubscribers receive changes to the Spot they are associatd with.")
    {
        NoteAccountant noteSubscriber{};
        board.registerNoteSubscriber(posA, noteSubscriber);
       
        // box 0 is about to arrive. 
        board.changeSpot(posA, BoardNote{boxId_0, MoveType::to_arrive}, true);

        // box 0 arrives.
        board.changeSpot(posA, BoardNote{boxId_0, MoveType::arrive}, true);

        vector<std::pair<std::chrono::time_point<std::chrono::high_resolution_clock>, BoardNote>> callbackNotes = noteSubscriber.getNotes();

        REQUIRE(2 == callbackNotes.size());
        REQUIRE(BoardNote{0, MoveType::to_arrive} == callbackNotes[0].second);
        REQUIRE(BoardNote{0, MoveType::arrive}  == callbackNotes[1].second);
    }

}
