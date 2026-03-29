#ifndef SHOUTBLASTER_BASICSHADER_HPP
#define SHOUTBLASTER_BASICSHADER_HPP

#include <string>

namespace ShoutBlast
{
    class BasicShader
    {
    public:
        static std::string GetVertexSource();
        static std::string GetFragmentSource();
    };
}

#endif