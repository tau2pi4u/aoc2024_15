#include <string>
#include <sstream>
#include <fstream>
#include <vector>

template<typename T>
void EasyErase(std::vector<T>& vec, T const& toErase)
{
    auto itr = std::find(vec.begin(), vec.end(), toErase);
    if (itr == vec.end()) return;
    vec.erase(itr);
}

template <typename T>
struct TwoDVector
{
    TwoDVector(size_t innerDim, size_t reserve = 0) : innerDim(innerDim) { _vec.reserve(reserve ? reserve : innerDim); }

    T* operator[](size_t idx)
    {
        return _vec.data() + idx * innerDim;
    }

    T const* operator[](size_t idx) const
    {
        return _vec.data() + idx * innerDim;
    }

    void push_back(T const& val)
    {
        _vec.push_back(val);
    }

    template<typename iterator>
    void append(iterator begin, typename iterator end)
    {
        _vec.insert(_vec.end(), begin, end);
    }

    typename std::vector<T>::iterator begin()
    {
        return _vec.begin();
    }

    typename std::vector<T>::iterator end()
    {
        return _vec.end();
    }

    typename std::vector<T>::const_iterator begin() const
    {
        return _vec.begin();
    }

    typename std::vector<T>::const_iterator end() const
    {
        return _vec.end();
    }

    size_t XDim() const
    {
        return innerDim;
    }

    size_t YDim() const
    {
        return _vec.size() / innerDim;
    }

    size_t IsInBounds(int64_t x, int64_t y) const
    {
        return y >= 0 && x >= 0 && y < YDim() && x < XDim();
    }

    size_t innerDim;
    std::vector<T> _vec;
};

std::vector<std::string> GetInputAsString(std::ifstream& ifstream)
{
    std::vector<std::string> output;
    std::string line;
    while (std::getline(ifstream, line))
    {
        output.emplace_back(std::move(line));
    }
    return output;
}

std::vector<std::string> GetInputAsString(const char* fpath)
{
    std::ifstream ifstream;
    ifstream.open(fpath);
    std::vector<std::string> output;
    std::string line;
    while (std::getline(ifstream, line))
    {
        output.emplace_back(std::move(line));
    }
    return output;
}

template<typename T>
TwoDVector<T> GetInputGrid(std::vector<std::string> const& inputLines)
{
    TwoDVector<T> output(inputLines.front().size(), inputLines.front().size() * inputLines.size());
    for (auto& line : inputLines)
    {
        output.append(line.begin(), line.end());
    }
    return output;
}

template<typename T>
TwoDVector<T> GetInputGrid(std::ifstream& ifstream)
{
    return GetInputGrid<T>(GetInputAsString(ifstream));
}

template<typename T>
TwoDVector<T> GetInputGrid(const char * fpath)
{
    std::ifstream ifstream;
    ifstream.open(fpath);
    return GetInputGrid<T>(GetInputAsString(ifstream));
}

enum class Direction : uint8_t
{
    Up,
    Right,
    Down,
    Left,
    Count
};

enum class Reflection : uint8_t
{
    Leading,
    Trailing,
    Count
};

template <typename T>
size_t constexpr AsInt(T e) 
{
    return static_cast<int>(e);
}

Direction Rotate(Direction startDir, uint8_t count = 1)
{
    return static_cast<Direction>((AsInt(startDir) + count) % AsInt(Direction::Count));
}

Direction Reflect(Direction startDir, Reflection reflector)
{
    // Leading \
    // Up    (0) -> Left  (3)
    // Right (1) -> Down  (2)
    // Down  (2) -> Right (1)
    // Left  (3) -> Up    (0)

    // Trailing /
    // Up    (0) -> Right (2)
    // Right (1) -> Up    (0)
    // Down  (2) -> Left  (3)
    // Left  (3) -> Down  (2)
    static const Direction lookup[AsInt(Reflection::Count)][AsInt(Direction::Count)] =
    {
        {
            Direction::Left, Direction::Down, Direction::Right, Direction::Up
        },
        {
            Direction::Right, Direction::Up, Direction::Left, Direction::Down
        }
    };

    return lookup[AsInt(reflector)][AsInt(startDir)];
}

int DirectionToX(Direction dir)
{
    static int lookup[] = { 0, 1, 0, -1 };
    return lookup[AsInt(dir)];
}

int DirectionToY(Direction dir)
{
    static int lookup[] = { -1, 0, 1, 0 };
    return lookup[AsInt(dir)];
}

uint8_t DirectionToMask(Direction dir)
{
    return 1 << AsInt(dir);
}