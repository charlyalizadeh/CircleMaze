#define OLC_PGE_APPLICATION
#include "engine.hpp"
int main(){
  Engine* test = new Engine(20,20,10);
  if(test->Construct(1920,1080,1,1))
    test->Start();
  return 0;
}

