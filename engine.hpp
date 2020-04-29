#include "circlemaze.hpp"
#include "olcPixelGameEngine.h"
#include <cmath>
#include <string>

const olc::Pixel bg = olc::Pixel(0, 0, 0);
const olc::Pixel fg = olc::Pixel(255,255,255);


class Engine : public olc::PixelGameEngine
{
public:
    Engine(int dim0, int dim1, int radius, int originX = -1, int originY = -1) : m_radius(radius), m_originX(originX), m_originY(originY)
    {
        m_maze = new CircleMaze(dim0, dim1);
    }

    bool OnUserCreate() override
    {
        m_originX = m_originX == -1 ? ScreenWidth() / 2 : m_originX;
        m_originY = m_originY == -1 ? ScreenHeight() / 2 : m_originY;
        //m_maze->backtrackInit();
        m_maze->backtrack();
        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {
        FillRect(0, 0, ScreenWidth(), ScreenHeight(), bg);
        /*if (m_maze->backtrackIt())
            drawMaze(true);
        else*/
            drawMazeStatic();
        //drawCirclesTest();
        return true;
    }

private:
    int m_radius;
    CircleMaze* m_maze;
    int m_originX, m_originY;



private:
    int convertCoordX(int coord)
    {
        return coord - m_originX;
    }
    int convertCoordY(int coord)
    {
        return m_originY - coord;
    }
    double getR(int x, int y)
    {
        return std::sqrt(x * x + y * y);
    }
    double getTeta(double x, double y)
    {
        double teta = std::atan2(y, x);
        if (teta <= 0)
            teta += 2.0 * M_PI;
        return teta;
    }
    double getX(double teta, double r)
    {
        return r * std::cos(teta);
    }
    double getY(double teta, double r)
    {
        return r * std::sin(teta);
    }

    //The midpoint circle algorithm changed so it will only display points which have a teta between teta1 and teta2
    void drawArc(int r, double teta1, double teta2, olc::Pixel color = fg)
    {
        int x = 0, y = r;
        int d = 3 - 2 * r;
        drawBresCircle(x, y, teta1, teta2, color);
        while (y >= x)
        {
            x++;
            if (d > 0)
            {
                y--;
                d = d + 4 * (x - y) + 10;
            }
            else
                d = d + 4 * x + 6;
            drawBresCircle(x, y, teta1, teta2, color);
        }
    }
    void drawBresCircle(int x, int y, double teta1, double teta2, olc::Pixel color = fg)
    {
        std::vector<int> xVect = { -x,x };
        std::vector<int> yVect = { -y,y };
        for (int j : xVect)
        {
            for (int i : yVect)
            {
                double teta = getTeta(j, i);
                if (teta > teta1&& teta < teta2) Draw(m_originX + j, (m_originY - i), color);

                teta = getTeta(i, j);
                if (teta >= teta1 && teta <= teta2)  Draw(m_originX + i, (m_originY - j), color);
            }
        }
    }

    int getDim0(int x, int y)
    {
        double r = getR(convertCoordX(x), convertCoordY(y));
        return m_maze->getDim0() - (r / (double)m_radius) + 1;
    }
    int getDim1(int x, int y)
    {
        double teta = getTeta(convertCoordX(x), convertCoordY(y));
        return teta * ((double)m_maze->getDim1() / (2.0 * M_PI));
    }
    int getIndex(int x, int y)
    {
        int index = getDim0(x, y) * m_maze->getDim1() + getDim1(x, y);
        if (index >= m_maze->getDim0() * m_maze->getDim1())
            index = m_maze->getDim0() * m_maze->getDim1();
        return index;
    }

    void drawMazeStatic(bool displayCell = true)
    {
        if (displayCell)
            FillCircle(m_originX, m_originY, (m_maze->getDim0() + 1) * m_radius, fg);
        for (int i = 0; i < m_maze->getDim0() * m_maze->getDim1(); i++)
        {
            if (m_maze->getWallH(i))
                drawWallH(i, bg);
            if (m_maze->getWallV(i))
                drawWallV(i, bg);
        }
    }
    void drawMaze(bool displayCell)
    {
        if (displayCell)
            for (int i = 0; i < m_maze->getDim0() * m_maze->getDim1() + 1; i++)
            {
                if(m_maze->getCurrentCellIndex()==i)
                    drawCell1(i, olc::GREEN);
                else if(m_maze->getCellState(i))
                    drawCell1(i,fg);

            }
        for (int i = 0; i < m_maze->getDim0() * m_maze->getDim1(); i++)
        {
            if (m_maze->getWallH(i))
                drawWallH(i, bg);
            else
                drawWallH(i, fg);
            if (m_maze->getWallV(i))
                drawWallV(i, bg);
            else
                drawWallV(i, fg);
        }
    }

    void drawCell(int index, olc::Pixel color = bg)
    {
        double coeffTeta = index % m_maze->getDim1();
        double teta1 = (coeffTeta / (double)m_maze->getDim1()) * (2.0 * M_PI);

        int coeffRadius = index / m_maze->getDim1();
        int radius = (m_maze->getDim0() - coeffRadius + 1) * m_radius;

        double teta2 = teta1 + (1.0 / (double)m_maze->getDim1()) * (2.0 * M_PI);
        int radiusMin = radius - m_radius;
        while (radius > radiusMin) {
            drawArc(radius, teta1, teta2, color);
            radius--;
        }

    }
    void drawCell1(int index, olc::Pixel color = bg)//This solution is way faster than drawCell, but I suppose it has its limit.
    {
        if (index != m_maze->getDim0() * m_maze->getDim1())
        {
            double coeffTeta = index % m_maze->getDim1();
            double teta1 = (coeffTeta / (double)m_maze->getDim1()) * (2.0 * M_PI);

            int coeffRadius = index / m_maze->getDim1();
            int radius = (m_maze->getDim0() - coeffRadius + 1) * m_radius;

            double teta2 = teta1 + (1.0 / (double)m_maze->getDim1()) * (2.0 * M_PI);
            while (teta1 < teta2) {
                int xStart = getX(-teta1, radius - 1);
                int yStart = getY(-teta1, radius - 1);
                int xEnd = getX(-teta1, radius - m_radius + 1);
                int yEnd = getY(-teta1, radius - m_radius + 1);
                xStart += ScreenWidth() / 2;
                xEnd += ScreenWidth() / 2;
                yStart += ScreenHeight() / 2;
                yEnd += ScreenHeight() / 2;
                DrawLine(xStart, yStart, xEnd, yEnd, color);
                teta1 += 0.002;
            }
        }
        else
        {
            FillCircle(m_originX, m_originY, m_radius, color);
        }
    }
    void drawWallV(int index, olc::Pixel color = fg)
    {
        double coeffTeta = index % m_maze->getDim1();
        double teta = -(coeffTeta / (double)m_maze->getDim1()) * (2.0 * M_PI);

        int coeffRadius = index / m_maze->getDim1();
        int radius = (m_maze->getDim0() - coeffRadius + 1) * m_radius;

        int xStart = getX(teta, radius);
        int yStart = getY(teta, radius);


        radius -= m_radius;

        int xEnd = getX(teta, radius);
        int yEnd = getY(teta, radius);

        xStart += ScreenWidth() / 2;
        xEnd += ScreenWidth() / 2;
        yEnd += ScreenHeight() / 2;
        yStart += ScreenHeight() / 2;
        DrawLine(xStart, yStart, xEnd, yEnd, color);
    }
    void drawWallH(int index, olc::Pixel color = fg)
    {
        double coeffTeta = index % m_maze->getDim1();
        double teta1 = (coeffTeta / (double)m_maze->getDim1()) * (2.0 * M_PI);

        int coeffRadius = index / m_maze->getDim1();
        int radius = (m_maze->getDim0() - coeffRadius) * m_radius;


        double teta2 = teta1 + (1.0 / (double)m_maze->getDim1()) * (2.0 * M_PI);

        drawArc(radius, teta1, teta2, color);

    }
    
    void drawCirclesTest()
    {
        int x = convertCoordX(GetMouseX());
        int y = convertCoordY(GetMouseY());
        int currentDim0 = getDim0(GetMouseX(), GetMouseY());
        int currentDim1 = getDim1(GetMouseX(), GetMouseY());
        int index = getIndex(GetMouseX(), GetMouseY());
        for (int i = 0; i < m_maze->getDim1() * m_maze->getDim0() + 1; i++)
        {
            if (index == i)
                drawCell1(i, olc::RED);
            else
                drawCell1(i, fg);
        }
        for (int i = 0; i < m_maze->getDim0() * m_maze->getDim1(); i++)
        {
            if (index == i)
                drawWallV(i, olc::GREEN);
            else
                drawWallV(i, bg);
        }
        for (int i = 0; i < m_maze->getDim1() * m_maze->getDim0(); i++)
        {
            if (index == i)
                drawWallH(i, olc::BLUE);
            else
                drawWallH(i, bg);
        }

        DrawString(0, 30, "Index = " + std::to_string(index));
        DrawString(0, 0, "Dim 0 = "+ std::to_string(currentDim0));
        DrawString(0, 20, "Dim 1 = " + std::to_string(currentDim1));
        DrawString(0, 40, "R = " +std::to_string(getR(x,y)));
        DrawString(0, 50, "Teta = " + std::to_string(getTeta(x, y)));
    }

};

