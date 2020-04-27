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
      m_maze->backtrackInit();
      testIndex = 0;
      return true;
    }
    bool OnUserUpdate(float fElapsedTime) override
    {
      FillRect(0,0,ScreenWidth(),ScreenHeight(),olc::BLACK);
      m_maze->backtrackIt();
      drawMaze();
      int index = getIndex(GetMouseX(),GetMouseY());
      DrawString(0,0,std::to_string(index),olc::BLACK);
    return true;
    }

  private:
    int m_radius;
    CircleMaze* m_maze;
    int m_originX,m_originY;

    int testIndex;
       
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
    double getX(double teta,double r)
    {
      return r * std::cos(teta);
    }
    double getY(double teta,double r)
    {
      return r * std::sin(teta);
    }
    
    void drawArc(int r,double teta1,double teta2,olc::Pixel color = olc::WHITE)
    {
      int x = 0, y = r; 
      int d = 3 - 2 * r; 
      drawBresCircle(x,y,teta1,teta2,color); 
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
          drawBresCircle(x,y,teta1,teta2,color);
      }

    }
    void drawBresCircle(int x,int y,double teta1,double teta2,olc::Pixel color = olc::WHITE)
    {
      std::vector<int> xVect = {-x,x};
      std::vector<int> yVect = {-y,y};
      for(int j : xVect)
      {
        for(int i : yVect)
        {
          double teta = getTeta(j,i);
          if(teta>teta1 && teta<teta2)
            Draw(m_originX + j,(m_originY - i),color);
          teta = getTeta(i,j);
          if(teta>=teta1 && teta<=teta2)
            Draw(m_originX + i,(m_originY - j),color);
        }
      }
    }

    int getDim0(int x,int y)
    {
      int r = getR(convertCoordX(x),convertCoordY(y));
      return m_maze->getDim0() - r/m_radius;
    }
    int getDim1(int x,int y)
    {
      double teta = getTeta(convertCoordX(x),convertCoordY(y));
      return teta * ((double)m_maze->getDim1()/(2.0*M_PI));
    }
    int getIndex(int x,int y)
    {
      int index = getDim0(x,y)*m_maze->getDim1()+getDim1(x,y);
      if(index>=m_maze->getDim0()*m_maze->getDim1())
        index = m_maze->getDim0()*m_maze->getDim1();
      return index;
    }

    void drawMaze()
    {
      for(int i = 0;i<m_maze->getDim0()*m_maze->getDim1();i++)
      {
        if(i==m_maze->getCurrentCellIndex())
          drawCell(i,olc::GREEN);
        else if(m_maze->getCellState(i))
          drawCell(i);
        else
          drawCell(i,olc::BLACK);
      }
      for(int i = 0;i<m_maze->getDim0()*m_maze->getDim1();i++)
      {
        if(m_maze->getWallH(i))
          drawWallH(i);
        else
          drawWallH(i,olc::WHITE);
        if(m_maze->getWallV(i))
          drawWallV(i);
        else
          drawWallV(i,olc::WHITE);
      }
      DrawCircle(m_originX,m_originY,(m_maze->getDim0()+1)*m_radius,olc::BLACK);
    }
    void drawCell(int index,olc::Pixel color = olc::WHITE)
    {
      double coeffTeta = index%m_maze->getDim1();
      double teta1 =  (coeffTeta/(double)m_maze->getDim1()) * (2.0*M_PI);

      int coeffRadius = index/m_maze->getDim1();
      int radius = (m_maze->getDim1()-coeffRadius + 1)*m_radius;

      double teta2 = teta1 + (1.0/(double)m_maze->getDim1())* (2.0*M_PI);
      int radiusMin = radius - m_radius;
      while(radius>radiusMin){
        drawArc(radius,teta1,teta2,color);
        radius--;
      }

    }
    void drawCell1(int index,olc::Pixel color = olc::WHITE)
    {
      int temp = (m_maze->getDim0())*m_radius;
      for(int x = m_originX-temp;x<m_originX+temp;x++)
      {
        for(int y = m_originY - temp;y<m_originY+temp;y++)
        {
          if(getIndex(x,y)==index)
            Draw(x,y,color);
        }
      }
    }
    void drawWallV(int index,olc::Pixel color = olc::BLACK)
    {
      double coeffTeta = index%m_maze->getDim1();
      double teta =  -(coeffTeta/(double)m_maze->getDim1()) * (2.0*M_PI);

      int coeffRadius = index/m_maze->getDim1();
      int radius = (m_maze->getDim1()+1-coeffRadius)*m_radius;
       
      int xStart = getX(teta,radius);
      int yStart = getY(teta,radius);


      radius -=m_radius;

      int xEnd = getX(teta,radius);
      int yEnd = getY(teta,radius);
      
      xStart+=ScreenWidth()/2;
      xEnd += ScreenWidth()/2;
      yEnd += ScreenHeight()/2;
      yStart += ScreenHeight()/2;
      DrawLine(xStart,yStart,xEnd,yEnd,color);
    }
    void drawWallH(int index,olc::Pixel color = olc::BLACK)
    {
      double coeffTeta = index%m_maze->getDim1();
      double teta1 =  (coeffTeta/(double)m_maze->getDim1()) * (2.0*M_PI);

      int coeffRadius = index/m_maze->getDim1();
      int radius = (m_maze->getDim1()-coeffRadius)*m_radius;


      double teta2 = teta1 + (1.0/(double)m_maze->getDim1())* (2.0*M_PI);

      drawArc(radius,teta1,teta2,color);
    }
    
    void drawCirclesTest()
    {
      int currentDim0 = getDim0(GetMouseX(),GetMouseY());
      int currentDim1 = getDim1(GetMouseX(),GetMouseY());
      int index = getIndex(GetMouseX(),GetMouseY());
      for(int i = 0;i<m_maze->getDim0()*m_maze->getDim1();i++)
      {
        drawWallH(i);
      }
      for(int i =  0;i<m_maze->getDim1()*m_maze->getDim0();i++)
      {
        drawCell(i);
      }

      for(int i =  0;i<m_maze->getDim1()*m_maze->getDim0();i++)
      {
        drawWallV(i);
      }
      
      DrawString(0,30,std::to_string(index));
      DrawString(0,0,std::to_string(currentDim0));
      DrawString(0,20,std::to_string(currentDim1));
    }

};

