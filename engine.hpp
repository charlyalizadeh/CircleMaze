#include "circlemaze.hpp"
#include "olcPixelGameEngine.h"
#include <cmath>
#include <string>


class Engine : public olc::PixelGameEngine
{
  public:
    Engine(int dim0,int dim1,int radius, int originX = -1,int originY = -1) : m_radius(radius), m_originX(originX),m_originY(originY) 
    {
      m_maze = new CircleMaze(dim0,dim1);
    }

    bool OnUserCreate() override
    {
      m_originX = m_originX == -1 ? ScreenWidth()/2 : m_originX;
      m_originY = m_originY == -1 ? ScreenHeight()/2 : m_originY;
      m_maze->backtrack();
      return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {
      drawMaze();
      return true;
    }

  private:
    int m_radius;
    CircleMaze* m_maze;
    int m_originX,m_originY;
       
  private:
    int convertCoordX(int coord)
    {
      return coord - m_originX; 
    }
    int convertCoordY(int coord)
    {
      return m_originY - coord;
    }
    double getR(int x,int y)
    {
      return std::sqrt(x*x+y*y);
    }
    double getTeta(double x,double y)
    {
      double teta = std::atan2(y,x);
      if(teta<0)
        teta += 2.0*M_PI;
      return teta;
    }
    int getDim0(int x,int y)
    {
      x = convertCoordX(x);
      y = convertCoordY(y);
      int r = getR(x,y);
      return m_maze->getDim0() - r/m_radius - 1;
    }
    int getDim1(int x,int y)
    {
      x = convertCoordX(x);
      y = convertCoordY(y);
      double teta = getTeta(x,y);
      return teta * (m_maze->getDim1()/(2.0*M_PI));
    }
    int getX(double r,double teta)
    {
      return r * std::cos(teta);
    }
    int getY(double r,double teta)
    {
      return r * std::sin(teta);
    }


    void drawMaze()
    {
      for(int x = 0;x<ScreenWidth();x++)
      {
        for(int y = 0;y<ScreenHeight();y++)
        {
          drawCell(x,y);
        }
      }
      drawWalls();

    }
    void drawWalls()
    {
      for(int j = 0;j<m_maze->getDim1();j++)
      {
        for(int i = 0;i<m_maze->getDim0()-1;i++){
          if(m_maze->getWallV(i*m_maze->getDim1()+j))
          {
            double teta = (double)j/(m_maze->getDim1())*M_PI*2.0;
            int r = m_radius*m_maze->getDim0() - i*m_radius;
            int xStart = getX(r,teta)+m_originX;
            int yStart = m_originY + getY(r,teta);

            r-=m_radius;
            int xEnd = getX(r,teta)+m_originX;
            int yEnd = m_originY + getY(r,teta);

            DrawLine(xStart,yStart,xEnd,yEnd,olc::WHITE);
          }

        }
      }
    }
    void drawCell(int x,int y)
    {
      int dim0 = getDim0(x,y);
      int dim1 = getDim1(x,y);
      int index = dim0*m_maze->getDim1()+dim1;
      if(index<0){
        Draw(x,y,olc::BLACK);
        return;
      }
      if(index>m_maze->getDim0()*m_maze->getDim1()+1)
        index = m_maze->getDim0()*m_maze->getDim1()+1;
      if(m_maze->getCellState(index))
        Draw(x,y,olc::WHITE);
    }

    void drawWallsTest()
    {
      for(int i = 0;i<m_maze->getDim1();i++)
      {
        double teta = (double)i/(double)m_maze->getDim1()*(2.0*M_PI);
        int r = m_maze->getDim0()*m_radius;
        int xStart = getX(r,teta) + m_originX;
        int yStart = m_originY + getY(r,teta);
        
        teta = (double)(i + m_maze->getDim1()/2)/(double)m_maze->getDim1()*(2.0*M_PI);
        int xEnd = getX(r,teta) + m_originX;
        int yEnd = m_originY + getY(r,teta);
        DrawLine(xStart,yStart,xEnd,yEnd,olc::BLUE);
      }
    }
    void drawCirclesTest()
    {
      int currentDim0 = getDim0(GetMouseX(),GetMouseY());
      for(int i = 0;i<m_maze->getDim0();i++)
      {
        if(currentDim0 == m_maze->getDim0() - i - 1)
          DrawCircle(m_originX,m_originY,(i+1)*m_radius,olc::RED);
        else 
          DrawCircle(m_originX,m_originY,(i+1)*m_radius,olc::GREEN);
      }
    }
    void test()
    {
      FillRect(0,0,ScreenWidth(),ScreenHeight(),olc::GREY);
      DrawCircle(m_originX,m_originY,200,olc::BLUE);
      DrawLine(0,m_originY,ScreenWidth(),m_originY,olc::RED);
      DrawLine(m_originX,0,m_originX,ScreenHeight(),olc::RED);
      int mouseX = convertCoordX(GetMouseX());
      int mouseY = convertCoordY(GetMouseY());
      double r = getR(mouseX,mouseY);
      double teta = getTeta(mouseX,mouseY);
      std::string str = "r = "  + std::to_string(r) + "  teta = " + std::to_string(teta);
      DrawString(GetMouseX() + 2,GetMouseY(),str);
    }

};

