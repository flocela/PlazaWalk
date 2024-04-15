#include "Util.h"

using namespace std;
// TODO test this getRandom
int  Util::getRandom(int start, int end)
{
    int min = std::min(start, end);
    int max = std::max(start, end);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::mt19937::result_type> distribution(min, max);
   
    return distribution(gen);

}
vector<int> Util::getRandom(int start, int end, int count)
{
    int min = std::min(start, end);
    int max = std::max(start, end);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::mt19937::result_type> distribution(min, max);
   
    vector<int> randomInts{};
    for(int ii=0; ii<count; ++ii)
    {
        randomInts.push_back(distribution(gen));
    }
    return randomInts;
} 

vector<Position> Util::getRandomInRectangle(Position cornerA, Position cornerB, int count)
{
    int x1 = cornerA.getX();
    int x2 = cornerB.getX();
    int y1 = cornerA.getY();
    int y2 = cornerB.getY();

    vector<int> randomXs = getRandom(x1, x2, count);
    vector<int> randomYs = getRandom(y1, y2, count);

    vector<Position> positions{};
    for(int ii=0; ii<count; ++ii)
    {
        positions.push_back({randomXs[ii], randomYs[ii]});
    }
    
    return positions;
}

vector<Box> Util::getBoxes(int firstBoxId, int numOfGroups, int groupSize)
{
    vector<Box> boxes{};
    for(int groupNum=0; groupNum<numOfGroups; ++groupNum)
    {
        int boxId = firstBoxId + (groupSize * groupNum);
        for(int boxIdx=0; boxIdx<groupSize; ++boxIdx)
        {
            boxes.push_back(Box{boxId+boxIdx, groupNum, 3, 3});
        }
    }
    return boxes;
}
