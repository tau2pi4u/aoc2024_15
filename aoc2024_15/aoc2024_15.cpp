// aoc2024_15.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <unordered_map>
#include <unordered_set>
#include <optional>
#include "utils.hpp"

#define _DEBUG_PRINT 0
#if _DEBUG_PRINT
#define DEBUG_ONLY(cmd) cmd
#define DEBUG_PRINT(dir) Print(dir);
#else
#define _DEBUG_ONLY(cmd)
#define DEBUG_PRINT(dir)
#endif

struct Board
{
    Board(TwoDVector<char> const& inputGrid) :
        boxes(inputGrid.YDim()),
        walls(inputGrid.YDim()),
        xDim(inputGrid.XDim()),
        yDim(inputGrid.YDim())
    {
        for (int y = 0; y < inputGrid.YDim(); ++y)
        {
            for (int x = 0; x < inputGrid.XDim(); ++x)
            {
                const char& inChar = inputGrid[y][x];
                switch (inChar)
                {
                case 'O': boxes[y] |= 1llu << x; break;
                case '#': walls[y] |= 1llu << x; break;
                case '@': robot = Robot{ .x = x, .y = y }; break;
                case '.': break;
                default: __debugbreak();
                }
            }
        }
    }

    std::vector<uint64_t> boxes;
    std::vector<uint64_t> walls;
    
    struct Robot
    {
        int x;
        int y;
    } robot;

    int xDim;
    int yDim;

    bool InBounds(int x, int y)
    {
        return x >= 0 && y >= 0 && x < xDim&& y < yDim;
    }

    std::vector<uint64_t> GatherMoveChain(int x, int y, Direction dir)
    {
        static std::vector<uint64_t> line;
        if (line.empty()) line.resize(yDim);
        std::fill(line.begin(), line.end(), 0);
        
        int xDir = DirectionToX(dir);
        int yDir = DirectionToY(dir);

        do
        {
            line[y] |= 1llu << x;
            x += xDir;
            y += yDir;
        } while (InBounds(x, y) && boxes[y] & (1llu << x));

        return line;
    }

    bool Overlap(std::vector<uint64_t> const& lhs, std::vector<uint64_t> const& rhs)
    {
        for (int i = 0; i < lhs.size() && rhs.size(); ++i)
        {
            if (lhs[i] & rhs[i])
            {
                return true;
            }
        }
        return false;
    }

    std::vector<uint64_t> Shift(std::vector<uint64_t> const& lhs, Direction dir)
    {
        std::vector<uint64_t> out(lhs.size());
        switch (dir)
        {
        case Direction::Up:
        {
            for (int y = 1; y < lhs.size(); ++y)
            {
                out[y - 1] = lhs[y];
            }
            out[lhs.size() - 1] = 0llu;
            break;
        }
        case Direction::Down:
        {
            for (int y = lhs.size() - 2; y != 0 ; --y)
            {
                out[y + 1] = lhs[y];
            }
            out[0] = 0llu;
            break;
        }
        case Direction::Right:
        {
            for (int y = 0; y < lhs.size(); ++y)
            {
                out[y] = lhs[y] << 1;
            }
            break;
        }
        case Direction::Left:
        {
            for (int y = 0; y < lhs.size(); ++y)
            {
                out[y] = lhs[y] >> 1;
            }
            break;
        }
        default: __debugbreak();
        }
        return out;
    }

    void Remove(std::vector<uint64_t>& lhs, std::vector<uint64_t> const& rhs)
    {
        for (int i = 0; i < lhs.size() && i < rhs.size(); ++i)
        {
            lhs[i] &= ~rhs[i];
        }
    }

    void Add(std::vector<uint64_t>& lhs, std::vector<uint64_t> const& rhs)
    {
        for (int i = 0; i < lhs.size() && i < rhs.size(); ++i)
        {
            lhs[i] |= rhs[i];
        }
    }

    void Move(Direction move)
    {        
        // Get chain of things which want to move into new space
        auto moveChain = GatherMoveChain(robot.x, robot.y, move);
        // Get mask of new spaces
        auto shifted = Shift(moveChain, move);

        // See if they can all move
        if (Overlap(walls, shifted)) return;

        // Move the robot
        robot.x += DirectionToX(move);
        robot.y += DirectionToY(move);
        
        // Remove robot from the shifted move chain
        shifted[robot.y] &= ~(1llu << robot.x);

        // Remove the boxes from their current locations
        Remove(boxes, moveChain);

        // Find the new locations of the boxes and add them in
        Add(boxes, shifted);
    }

    void Move(std::vector<Direction> const& moves)
    {
        DEBUG_PRINT(Direction::Count)
        for (auto& move : moves)
        {
            Move(move);
            if (!InBounds(robot.x, robot.y) || (walls[robot.y] & (1llu << robot.x)) || (boxes[robot.y] & (1llu << robot.x)))
            {
                __debugbreak();
            }
            DEBUG_PRINT(move);
        }
    }

    void Print(Direction move = Direction::Count)
    {
        printf("\n");
        switch (move)
        {
        case Direction::Up: printf("Up\n"); break;
        case Direction::Down: printf("Down\n"); break;
        case Direction::Left: printf("Left\n"); break;
        case Direction::Right: printf("Right\n"); break;
        }
        for (int y = 0; y < yDim; ++y)
        {
            for (int x = 0; x < xDim; ++x)
            {
                if (walls[y] & (1llu << x)) printf("#");
                else if (boxes[y] & (1llu << x)) printf("O");
                else if (x == robot.x && y == robot.y) printf("@");
                else printf(".");
            }
            printf("\n");
        }
        printf("\n");
    }

    int P1Score()
    {
        int score = 0;
        for (int y = 0; y < yDim; ++y)
        {
            for (int x = 0; x < xDim; ++x)
            {
                if (!(boxes[y] & (1llu << x))) continue;
                score += 100 * y + x;
            }
        }
        return score;
    }
};

std::vector<Direction> GetMoves(std::vector<std::string> const& moveStr)
{
    std::vector<Direction> moves;
    for (auto& line : moveStr)
    {
        for (auto& c : line)
        {
            switch (c)
            {
            case '^': moves.push_back(Direction::Up); break;
            case '>': moves.push_back(Direction::Right); break;
            case '<': moves.push_back(Direction::Left); break;
            case 'v': moves.push_back(Direction::Down); break;
            default: __debugbreak();
            }
        }
    }
    return moves;
}

struct BoardP2
{
    BoardP2(TwoDVector<char> const& inputGrid) :
        xDim(inputGrid.XDim() * 2),
        yDim(inputGrid.YDim())
    {
        for (int y = 0; y < inputGrid.YDim(); ++y)
        {
            for (int x = 0; x < inputGrid.XDim(); ++x)
            {
                const char& inChar = inputGrid[y][x];
                switch (inChar)
                {
                case 'O': boxes.insert(Coord{ .x = x * 2, .y = y }); break;
                case '#': 
                {
                    walls.insert(Coord{ .x = x * 2,     .y = y }); 
                    walls.insert(Coord{ .x = x * 2 + 1, .y = y });
                    break;
                }
                case '@': robot = Coord{ .x = x * 2, .y = y }; break;
                case '.': break;
                default: __debugbreak();
                }
            }
        }
    }

    struct Coord
    {
        int x;
        int y;
    } robot;

    struct CoordHasher
    {
        size_t operator()(Coord const& coord) const
        {
            return static_cast<size_t>(coord.x) | (static_cast<size_t>(coord.y) << 32);
        }
    };
    
    int xDim;
    int yDim;
    std::unordered_set<Coord, CoordHasher> boxes;
    std::unordered_set<Coord, CoordHasher> walls;

    bool VerticalHitBox(int x, int y, int dirY, std::vector<Coord> & boxesToMove)
    {
        int minX = INT_MAX;
        int maxX = 0;
        for (auto& box : boxesToMove)
        {
            if (box.y != y - dirY) continue;
            minX = std::min(minX, box.x);
            maxX = std::max(maxX, box.x + 1);
        }

        // boxes are wide
        minX -= 1;

        bool boxesFound = false;
        for (int i = minX; i <= maxX; ++i)
        {
            Coord test{ i, y };
            if (boxes.count(test) == 0) continue;
            boxesFound = true;
            boxesToMove.push_back(test);
        }
        return boxesFound;
    }

    bool HoriztontalHitBox(int x, int y, Direction direction, std::vector<Coord>& boxesToMove)
    {
        int offset = direction == Direction::Left ? 1 : 0;
        Coord test{ x - offset, y };
        if (boxes.count(test) == 0) return false;
        boxesToMove.push_back(test);
        return true;
    }

    std::vector<Coord> GatherBoxes(int x, int y, Direction direction)
    {
        int dirX = DirectionToX(direction);
        int dirY = DirectionToY(direction);

        std::vector<Coord> boxesToMove;
        if (dirY)
        {
            y += dirY;
            // first should be manual
            if (boxes.count({ x, y })) boxesToMove = { {x, y} };
            else if (boxes.count({ x - 1, y })) boxesToMove = { {x - 1, y} };
            else return boxesToMove;
            y += dirY;

            while (VerticalHitBox(x, y, dirY, boxesToMove))
            {
                y += dirY;
            }
        }
        else
        {
            x += dirX;
            while (HoriztontalHitBox(x, y, direction, boxesToMove))
            {
                x += dirX * 2; // box is 2 wide
            }
        }
        return boxesToMove;
    }

    bool CheckBoxesValid(std::vector<Coord> const& boxesToMove, Direction direction) const
    {
        int dirX = DirectionToX(direction);
        int dirY = DirectionToY(direction);

        for (auto box : boxesToMove)
        {
            box.x += dirX;
            box.y += dirY;

            if (walls.count(box)) return false;
            box.x += 1;
            if (walls.count(box)) return false;
        }
        return true;
    }

    void MoveBoxes(std::vector<Coord> const& boxesToMove, Direction direction)
    {
        int dirX = DirectionToX(direction);
        int dirY = DirectionToY(direction);

        for (auto const& box : boxesToMove)
        {
            boxes.erase(box);
        }

        for (auto box : boxesToMove)
        {
            box.x += dirX;
            box.y += dirY;

            boxes.insert(box);
        }
    }

    void Move(Direction const& direction)
    {
        if (walls.count({ 
            robot.x + DirectionToX(direction), 
            robot.y + DirectionToY(direction) 
            }))
        {
            return;
        }
        auto boxesToMove = GatherBoxes(robot.x, robot.y, direction);
        if (!CheckBoxesValid(boxesToMove, direction))
        {
            _DEBUG_ONLY(printf("REJECTED\n"));
            return;
        }
        _DEBUG_ONLY(printf("SUCCESS\n"));
        MoveBoxes(boxesToMove, direction);
        robot.x += DirectionToX(direction);
        robot.y += DirectionToY(direction);
    }

    void Move(std::vector<Direction> const& moves)
    {
        DEBUG_PRINT(Direction::Count);
        int i = 0;
        for (auto& move : moves) 
        {
#if _DEBUG_PRINT
            printf("%d: ", i++);
            switch (move)
            {
            case Direction::Up: printf("Up "); break;
            case Direction::Down: printf("Down "); break;
            case Direction::Left: printf("Left "); break;
            case Direction::Right: printf("Right "); break;
            }
#endif
            Move(move);
            DEBUG_PRINT(move);

            for (auto const& box : boxes)
            {
                if (boxes.count({ box.x + 1, box.y })) __debugbreak();
                if (box.y == robot.y && (box.x == robot.x || box.x + 1 == robot.x))__debugbreak();
            }
        }

    }

    void Print(Direction move = Direction::Count)
    {
        printf("\n");
        for (int y = 0; y < yDim; ++y)
        {
            for (int x = 0; x < xDim; ++x)
            {
                if (walls.count({ x, y })) printf("#");
                else if (boxes.count({ x, y })) printf("[");
                else if (boxes.count({ x - 1, y })) printf("]");
                else if (x == robot.x && y == robot.y) printf("@");
                else printf(".");
            }
            printf("\n");
        }
        printf("\n");
    }

    int P2Score() const
    {
        int score = 0;
        for (auto const& box : boxes)
        {
            score += box.y * 100 + box.x;
        }
        return score;
    }
};

bool operator==(BoardP2::Coord const& lhs, BoardP2::Coord const& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

int main()
{
    auto inputStrings = GetInputAsString("input.txt");
    auto midpoint = std::find_if(inputStrings.begin(), inputStrings.end(), [](std::string& string) { return string.empty(); });
    std::vector<std::string> gridString = std::vector<std::string>(inputStrings.begin(), midpoint);
    auto grid = GetInputGrid<char>(gridString);
    Board board(grid);
    auto moves = GetMoves(std::vector<std::string>(midpoint + 1, inputStrings.end()));
    /*board.Move(moves);
    auto p1 = board.P1Score();
    printf("%d\n", p1);*/

    BoardP2 boardp2(grid);
    boardp2.Move(moves);
    auto p2 = boardp2.P2Score();
    printf("%d\n", p2);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
