#include <vector>
#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <stack>


struct Cell 
{
    bool state;
    int index;
    Cell(int _index = -1, bool _state = false) :index(_index), state(_state) {}
};

class CircleMaze
{
public:
    CircleMaze(int dim0, int dim1, int seed = time(NULL)) : m_dim0(dim0), m_dim1(dim1), m_seed(seed)// dim0 = number of circles, dim1 = cells per circle
    {
        for (int i = 0; i <= m_dim0 * m_dim1 + 1; i++)
            m_cells.push_back(new Cell(i));
        m_matWallH = std::vector<bool>(m_dim0 * m_dim1, true);
        m_matWallV = std::vector<bool>(m_dim0 * m_dim1, true);
        m_currentCell = new Cell();
        m_cells[m_dim0 * m_dim1 + 1]->state = true;
    }

    void backtrackInit()
    {
        srand(m_seed);
        int index = rand() % (m_dim0 * m_dim1 + 1);
        m_stack.push(m_cells[index]);
    }
    bool backtrackIt()
    {
        if (m_stack.empty())
            return false;
        else
        {
            m_currentCell = m_stack.top();
            m_stack.pop();
            std::vector<Cell*> nghb = getValidNeighboors(m_currentCell->index);
            if (!nghb.empty())
            {
                m_stack.push(m_currentCell);
                Cell* neighboor = nghb[rand() % (nghb.size())];
                removeWall(m_currentCell->index, neighboor->index);
                neighboor->state = true;
                m_stack.push(neighboor);
                m_currentCell = neighboor;
            }
            return true;
        }
    }
    void backtrack()
    {
        backtrackInit();
        while (backtrackIt());
    }

    bool getWallH(int index)
    {
        return m_matWallH[index];
    }
    bool getWallV(int index)
    {
        return m_matWallV[index];
    }
    bool getCellState(int index)
    {
        return m_cells[index]->state;
    }
    int getDim0()
    {
        return m_dim0;
    }
    int getDim1()
    {
        return m_dim1;
    }
    int getCurrentCellIndex()
    {
        return m_currentCell->index;
    }

private:
    int m_dim0, m_dim1;
    std::vector<Cell*> m_cells;
    std::vector<bool> m_matWallH, m_matWallV;
    int m_seed;
    std::stack<Cell*> m_stack;
    Cell* m_currentCell;

private:
    std::vector<int> getNeighboorsIndex(int i)
    {
        std::vector<int> nghb;
        if (i % m_dim1 == 0)
        {
            nghb = std::vector<int>{ i - m_dim1,i + (m_dim1 - 1),i + 1,i + m_dim1 };
        }
        else if ((i + 1) % m_dim1 == 0)
        {
            nghb = std::vector<int>{ i - m_dim1,i - (m_dim1 - 1),i - 1,i + m_dim1 };
        }
        else if (i == m_dim0 * m_dim1)
        {
            for (int k = i - m_dim1; k < i - 1; i++)
                nghb.push_back(k);
        }
        else
        {
            nghb = std::vector<int>{ i - m_dim1,i + m_dim1,i + 1,i - 1 };
        }
        nghb.erase(std::remove_if(nghb.begin(), nghb.end(),
            [](const int& x) {
                return x < 0;
            }), nghb.end());
        std::replace_if(nghb.begin(), nghb.end(),
            [this](const int& x) {
                return x > m_dim0* m_dim1;
            }, m_dim0 * m_dim1);
        return nghb;
    }
    std::vector<Cell*> getValidNeighboors(int i)
    {
        std::vector<int> nghbIndex = getNeighboorsIndex(i);
        std::vector<Cell*> nghb;
        for (int k : nghbIndex)
            if (!m_cells[k]->state)
                nghb.push_back(m_cells[k]);
        return nghb;
    }
    void removeWall(int coord1, int coord2)
    {
        int min = std::min(coord1, coord2);
        int max = std::max(coord1, coord2);
        if (min == max - m_dim1 || max == m_dim0 * m_dim1)
            m_matWallH[min] = false;
        else if (min == max - (m_dim1 - 1))
            m_matWallV[min] = false;
        else
            m_matWallV[max] = false;
    }
};

