#include "Core/Application.hpp"
#include <iostream>

using namespace ShoutBlast;

int main(int argc, char **argv)
{
    Application application("ShoutBlast", 800, 600, true);
    application.Run();
    return 0;
}