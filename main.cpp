#define OLC_PGE_APPLICATION
#define _USE_MATH_DEFINES
#include "engine.hpp"

int main() 
{
    Engine engine(50,50,10);
    if(engine.Construct(1920, 1080, 1, 1))
        engine.Start();
    return 0;
}

