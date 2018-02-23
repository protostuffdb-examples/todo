//#include "util.h"
#include "app.h"
//#include <cstdio>

int main(int argc, char* argv[])
{
    //int port;
    //const char* host = util::resolveIpPort(argc > 1 ? argv[1] : nullptr, &port);
    /*
    flatbuffers::Parser parser;
    if (!parser.Parse(todo_user_schema) || !parser.SetRootType("Todo_PList"))
    {
        fprintf(stderr, "Could not load schema.\n");
        return 1;
    }
    
    if (!parser.ParseJson(R"({"p":[{"key":"CgAAAAAAAACZ","ts":1491921868559,"title":"world","completed":false}]})"))
    {
        fprintf(stderr, "Failed to parse json.\n%s\n", parser.error_.c_str());
        return 1;
    }
    printTodos(parser.builder_.GetBufferPointer());
    
    // numeric
    if (!parser.ParseJson(R"({"1":[{"1":"CgAAAAAAAACZ","2":1491921868559,"3":"world","4":false}]})", true))
    {
        fprintf(stderr, "Failed to parse numeric json.\n%s\n", parser.error_.c_str());
        return 1;
    }
    printTodos(parser.builder_.GetBufferPointer());
    */
    
    return todo::run(argc, argv);
}
