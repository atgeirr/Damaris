#include <iostream>
#include "Model.hpp"

using namespace std;
using namespace Damaris::Model;


int main (int argc, char* argv[])
{
  try
  {
    auto_ptr<simulation> sim(model(argv[1],xml_schema::flags::dont_validate));
	std::cout << sim->name() << std::endl;  
}
  catch (const xml_schema::exception& e)
  {
    cerr << e << endl;
    return 1;
  }
}
