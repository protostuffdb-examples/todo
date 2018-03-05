#include "app.h"

#include <cstring>
#include <fstream>
#include <thread>

extern "C" {
#include <sys/stat.h>
#include <uv.h>
}

#ifndef WIN32

extern "C" {
#include <unistd.h>
}
#define GetCurrentDir getcwd
static const char SEPARATOR = '/';

#else

extern "C" {
#include <direct.h>
#include <windows.h>
#pragma comment(lib, "user32.lib")
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
}
#define GetCurrentDir _getcwd
static const char SEPARATOR = '\\';

#endif

static bool exists(const char *path)
{
    struct stat b;
    return 0 == stat(path, &b);
}

static bool isSPCRLF(char c)
{
    return c == ' ' || c == '\r' || c == '\n';
}

// Load file "name" into "buf" returning true if successful
// false otherwise.  If "binary" is false data is read
// using ifstream's text mode, otherwise data is read with
// no transcoding.
static bool LoadFile(const char *name, bool binary, std::string *buf, bool stripWS = false)
{
    std::ifstream ifs(name, binary ? std::ifstream::binary : std::ifstream::in);
    if (!ifs.is_open())
        return false;
    *buf = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    if (stripWS)
    {
        while (isSPCRLF(buf->back()))
            buf->pop_back();
    }
    return !ifs.bad();
}

#ifndef WIN32
const char PDB_TRY_BIN[] = "protostuffdb-rjre";
#else
const char PDB_TRY_BIN[] = "protostuffdb-sp.exe";
const char EXEC_CWD[] = "C:\\opt\\jre\\bin\\server";
#endif

static const char IDENTIFIER[] = "jni rpc:";

#define OUTPUT_SIZE 1024
static char output[OUTPUT_SIZE];
static int output_used = 0;

static uv_loop_t *loop;
static uv_process_options_t options;

static uv_process_t child_req;
static uv_pipe_t child_out;
static bool child_started = false;

static std::string ip_port;
static char** $argv = nullptr;

static void run()
{
    char* args[]{ $argv[0], const_cast<char*>(ip_port.c_str()), nullptr };
    auto status = todo::run(2, args);
    auto ret = uv_process_kill(&child_req, 15);
    exit(status || ret ? 1 : 0);
}

static void on_child_exit(uv_process_t* req, int64_t status, int signal)
{
    if (status)
        fprintf(stderr, "protostuffdb exited with status %d", static_cast<int>(status));
    
    uv_close((uv_handle_t*) req, NULL);
}

static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
    buf->base = output + output_used;
    buf->len = OUTPUT_SIZE - output_used;
}

static void on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
    if (nread > 0)
        output_used += nread;
    
    if (child_started || nread <= 0)
        return;
    
    if (0 != strncmp(buf->base, IDENTIFIER, sizeof(IDENTIFIER) - 1))
    {
        fprintf(stdout, "%.*s", static_cast<int>(nread), buf->base);
        return;
    }
    
    child_started = true;
    std::thread t(run);
    t.detach();
}

int main(int argc, char* argv[])
{
    char buf[FILENAME_MAX];
    if (!GetCurrentDir(buf, FILENAME_MAX))
    {
        std::fprintf(stderr, "Could not obtain $PWD.\n");
        return 1;
    }
    
    std::string pwd(buf);
    
    // ==================================================
    
    std::string name;
    std::string name_path;
    name_path += pwd;
    name_path += SEPARATOR;
    name_path += "target";
    name_path += SEPARATOR;
    name_path += "name.txt";
    if (!LoadFile(name_path.c_str(), false, &name, true))
    {
        std::fprintf(stderr, "target/name.txt not found.\n");
        return 1;
    }
    
    std::string target_dir;
    target_dir += pwd;
    target_dir += SEPARATOR;
    target_dir += "target";
    
    std::string pdb_bin;
    pdb_bin += target_dir;
    pdb_bin += SEPARATOR;
    pdb_bin += PDB_TRY_BIN;
    
    #ifndef WIN32
    bool relative_jre = true;
    #endif
    
    if (!exists(pdb_bin.c_str()))
    {
        #ifndef WIN32
        // remove -rjre prefix
        pdb_bin.erase(pdb_bin.size() - 5, 5);
        if (!exists(pdb_bin.c_str()))
        {
            fprintf(stderr, "target/%s not found.\n", PDB_TRY_BIN);
            return 1;
        }
        
        relative_jre = false;
        target_dir.clear(); // cwd
        #else
        fprintf(stderr, "target/%s not found.\n", PDB_TRY_BIN);
        return 1;
        #endif
    }
    
    #ifndef WIN32
    if (relative_jre)
    #else
    if (!exists(EXEC_CWD))
    #endif
    {
        target_dir += SEPARATOR;
        target_dir += "jre";
        if (!exists(target_dir.c_str()))
        {
            fprintf(stderr, "target/jre not found.\n");
            return 1;
        }
        #ifndef WIN32
        target_dir.clear();
        #else
        target_dir += "\\bin\\server";
        #endif
    }
    #ifdef WIN32
    else
    {
        target_dir.assign(EXEC_CWD);
    }
    #endif
    
    // ==================================================
    
    // global arg
    //std::string ip_port;
    
    std::string port;
    std::string port_path;
    port_path += pwd;
    port_path += SEPARATOR;
    port_path += "PORT.txt";
    if (!LoadFile(port_path.c_str(), false, &port, true))
    {
        ip_port = "127.0.0.1:5000";
    }
    else if (std::string::npos == port.find(':'))
    {
        ip_port = "127.0.0.1:" + port;
    }
    else
    {
        ip_port = port;
    }
    
    // ==================================================
    
    std::string jvm_args;
    std::string jvm_args_path;
    jvm_args_path += pwd;
    jvm_args_path += SEPARATOR;
    jvm_args_path += "ARGS.txt";
    if (!LoadFile(jvm_args_path.c_str(), false, &jvm_args, true))
    {
        std::fprintf(stderr, "ARGS.txt not found.\n");
        return 1;
    }
    
    // ==================================================
    
    std::string cmd;
    
    cmd += pdb_bin;
    cmd += ' ';
    cmd += ip_port;
    
    // ==================================================
    // todo-ts/g/user/UserServices.json
    
    cmd += ' ';
    cmd += pwd;
    cmd += SEPARATOR;
    cmd += name;
    cmd += "-ts";
    cmd += SEPARATOR;
    cmd += 'g';
    cmd += SEPARATOR;
    cmd += "user";
    cmd += SEPARATOR;
    cmd += "UserServices.json";
    
    // ==================================================
    // jvm args
    
    cmd += ' ';
    cmd += jvm_args;
    
    // ==================================================
    // -Djava.class.path=todo-all/target/todo-all-jarjar.jar todo.all.Main
    
    cmd += " -Djava.class.path=";
    cmd += pwd;
    cmd += SEPARATOR;
    cmd += name;
    cmd += "-all";
    cmd += SEPARATOR;
    cmd += "target";
    cmd += SEPARATOR;
    cmd += name;
    cmd += "-all-jarjar.jar ";
    cmd += name;
    cmd +=".all.Main";
    
    // ==================================================
    // app args
    
    cmd += ' ';
    cmd += pwd;
    
    // placeholder for null char
    cmd += ' ';
    
    // ==================================================
    
    char* args[128]; // should be enough
    size_t i = 0;
    for (size_t start = 0, size = cmd.size(), pos; start < size; i++, start = pos + 1)
    {
        pos = cmd.find(' ', start);
        cmd[pos] = '\0';
        args[i] = const_cast<char*>(cmd.c_str() + start);
    }
    
    args[i] = nullptr;
    
    $argv = argv;
    loop = uv_default_loop();
    
    options.exit_cb = on_child_exit;
    options.args = args;
    options.file = pdb_bin.c_str();
    #ifdef WIN32
    options.cwd = target_dir.c_str();
    #endif
    
    uv_pipe_init(loop, &child_out, 0);
    
    uv_stdio_container_t child_stdio[3];
    options.stdio = child_stdio;
    options.stdio_count = 3;
    
    child_stdio[0].flags = UV_IGNORE;
    child_stdio[1].flags = static_cast<uv_stdio_flags>(UV_CREATE_PIPE | UV_WRITABLE_PIPE);
    child_stdio[1].data.stream = (uv_stream_t *) &child_out;
    child_stdio[2].flags = UV_INHERIT_FD;
    child_stdio[2].data.fd = 2;
    
    int r;
    if ((r = uv_spawn(loop, &child_req, &options)))
    {
        fprintf(stderr, "protostuffdb spawn failed: %s\n", uv_strerror(r));
        return 1;
    }
    
    if ((r = uv_read_start((uv_stream_t*)&child_out, on_alloc, on_read)))
    {
        fprintf(stderr, "%s\n", uv_strerror(r));
        return 1;
    }
    
    //fprintf(stderr, "Launched process with ID %d\n", child_req.pid);
    
    return uv_run(loop, UV_RUN_DEFAULT);
}
