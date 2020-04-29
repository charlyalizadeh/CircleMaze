#define OLC_PGE_APPLICATION
#define _USE_MATH_DEFINES
#include "engine.hpp"

int main() 
{
    Engine* test = new Engine(50, 50, 10);
    if (test->Construct(1920, 1080, 1, 1))
        test->Start();
    return 0;
}

