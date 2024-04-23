#include "catch.hpp"

#include "../src/BroadcastAgent.h"

using namespace std;

TEST_CASE("BroadcastAgent_core::")
{
    SECTION("calling requestBroadcastChanges() causes BoardProxy to call its sendChanges() method()")
    {

        class TestListener : public BoardListener 
        {
        public: 

            void receiveChanges(unordered_map<SpotType, unordered_set<Drop>> setOfDropsPerType,
                                unordered_map<int, BoxInfo> boxesPerId) override
            {
                try
                {
                    ++_count;
                }
                catch(...)
                {
                    return;
                }
            }
       
            // counts how many times receivedChanges is called. 
            int _count = 0;
            
        };

        Board board{20, 20, vector<Box>{}};
        TestListener listener{};
        board.registerListener(&listener);

        BoardProxy boardProxy{board};
        BroadcastAgent agent{std::move(boardProxy)};

        agent.requestBroadcast();
        agent.requestBroadcast();

        REQUIRE(2 == listener._count);
    }
}