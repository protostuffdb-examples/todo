#include <string>
#include <vector>
#include <fstream>

#ifdef WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include "app.h"

static bool isSPCRLF(char c)
{
    return c == ' ' || c == '\r' || c == '\n';
}

static bool loadFile(const char *name, bool binary, std::string *buf, bool stripWS = false)
{
    std::ifstream ifs(name, binary ? std::ifstream::binary : std::ifstream::in);
    if (!ifs.is_open())
        return false;
    *buf = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    if (stripWS && !buf->empty())
    {
        while (isSPCRLF(buf->back()))
            buf->pop_back();
    }
    return !ifs.bad();
}

static bool findSpace(std::string& args, std::vector<size_t>& offsets)
{
    for (size_t idx = 0, len = args.size();
            idx < len && std::string::npos != (idx = args.find(' ', idx));
            idx++)
    {
        offsets.push_back(idx);
        args[idx] = '\0';
    }
    
    return !offsets.empty();
}

int main(int argc, char* argv[])
{
    int ret;
    std::string args;
    std::vector<size_t> offsets;
    if (argc > 1 || !loadFile("target/ARGS.txt", false, &args, true))
    {
        ret = todo::run(argc, argv);
    }
    else if (!findSpace(args, offsets))
    {
        char* argv_override[]{ argv[0], const_cast<char*>(args.c_str()), nullptr };
        ret = todo::run(2, argv_override);
    }
    else
    {
        size_t i = 0, start = 0, count = 1 + 1 + offsets.size();
        char** argv_override = new char*[count + 1];
        
        argv_override[count] = nullptr;
        argv_override[i++] = argv[0];
        for (auto idx : offsets)
        {
            argv_override[i++] = const_cast<char*>(args.data() + start);
            start = idx + 1;
        }
        argv_override[i++] = const_cast<char*>(args.data() + start);
        
        ret = todo::run(static_cast<int>(count), argv_override);
        
        delete[] argv_override;
    }
    return ret;
}
