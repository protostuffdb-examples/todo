#include "app.h"

#include <cstring>
#include <process.hpp>
#include <fstream>
#include <condition_variable>
#include <signal.h>

extern "C" {
#include <sys/stat.h>
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
#include <tchar.h>
#include <windows.h>
#pragma comment(lib, "user32.lib")
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
}
#define GetCurrentDir _getcwd
static const char SEPARATOR = '\\';

static std::string absolute_path(const std::string &filepath) {
    char abs_path[MAX_PATH];
    return GetFullPathNameA(filepath.c_str(), MAX_PATH, abs_path, nullptr) ? abs_path : filepath;
}

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

static const char IDENTIFIER[] = "jni rpc:";

struct Signal
{
    sig_atomic_t count;
    std::mutex mutex;
    std::condition_variable cv;
    
    Signal(int val = 0) : count(val) {}
    Signal& set(int val)
    {
        count = val;
        return *this;
    }
    bool notify()
    {
        bool notify = 0 == --count;
        if (notify)
        {
            std::unique_lock<std::mutex> lock(mutex);
            lock.unlock();
            cv.notify_one();
        }
        return notify;
    }
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]{return 1 > count;});
    }
    
    // no copy
private:
    Signal(const Signal&) = delete;
    const Signal& operator=(const Signal&) = delete;
};

#ifndef WIN32
const char PDB_TRY_BIN[] = "protostuffdb-rjre";
#else
const char PDB_TRY_BIN[] = "protostuffdb.exe";
const char EXEC_CWD[] = "C:\\opt\\jre\\bin\\server";
#endif

static const char FMT_STR[] = "%.*s";
static const char FMT_STR_LN[] = "%.*s\n";

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
    
    std::string ip_port;
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
    
    // child reads from stdin for eof
    cmd += " -Dprotostuffdb.rt_flags=8 ";
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
    
    Signal signal(1);
    TinyProcessLib::Process p(cmd, target_dir, [&signal](const char* bytes, size_t n) {
        if (n > sizeof(IDENTIFIER) && 0 == memcmp(bytes, IDENTIFIER, sizeof(IDENTIFIER) - 1))
            signal.notify();
        else
            fprintf(stdout, '\n' == bytes[n-1] ? FMT_STR : FMT_STR_LN, static_cast<int>(n), bytes);
    }, [](const char* bytes, size_t n) {
        fprintf(stderr, '\n' == bytes[n-1] ? FMT_STR : FMT_STR_LN, static_cast<int>(n), bytes);
    }, true, 512); // open stdin
    
    signal.wait();
    
    int ret = 0;
    char* args[]{ argv[0], const_cast<char*>(ip_port.c_str()), nullptr };
    try
    {
        ret = todo::run(2, args);
    }
    catch (...)
    {
        ret = 1;
    }
    
    p.kill();
    
    return ret;
}
