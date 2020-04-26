#define OLC_PGE_APPLICATION
#include "engine.hpp"
int main(){
  Engine* test = new Engine(5,10,50);
  if(test->Construct(1920,1080,1,1))
    test->Start();
  return 0;
}

