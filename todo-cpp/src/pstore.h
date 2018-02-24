#pragma once

#include <functional>
#include <vector>

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>

namespace coreds {

enum class SelectionFlags : uint8_t {
    NONE = 0,
    CLICKED = 1,
    REFRESH = 2,
    CLICKED_UPDATE = 4,
    FORCE = 8,

    MASK_FORCE_OR_UPDATE = FORCE | CLICKED_UPDATE
};

struct ParamRangeKey {
    bool desc{ true };
    int limit{ 10 };
    const char* start_key;
    const char* parent_key{ nullptr };
};

struct Opts {
    bool desc{ true };
    int multiplier{ 3 };
    bool multiplier_conditional{ false };
    int pageSize{ 10 };
};

struct Pager {
    
};

template <typename T>

struct PojoStore
{
    const char* const name;
    const char* const wrapperName;
private:
    flatbuffers::Parser parser;
    Opts opts;
    std::vector<T*> mainArray;
    std::vector<T*> altArray;
    std::vector<T*>* currentArray{ nullptr };
    T* startObj{ nullptr };
    
    std::function<const char*(const T* pojo)> $fnKey;
    std::function<void(const ParamRangeKey prk)> $fnFetch;
    std::function<void(const T* pojo)> $fnUpdate;
    
public:
    PojoStore(const char* name, const char* wrapperName) : name(name), wrapperName(wrapperName)
    {
        
    }
    bool init(Opts options, const char* schema)
    {
        opts = options;
        //return parser.Parse(schema);
        return true;
    }
    void bind(
        std::function<const char*(const T* pojo)> fnKey,
        std::function<void(const ParamRangeKey prk)> fnFetch,
        std::function<void(const T* pojo)> fnUpdate
    )
    {
        $fnKey = fnKey;
        $fnFetch = fnFetch;
        $fnUpdate = fnUpdate;
    }
    void select(int idx, SelectionFlags flags)
    {
        
    }
};

} // coreds
