#pragma once

#include <coreds/pstore.h>

#include "../g/user/index_generated.h"

namespace todo {

struct Todo// : brynet::NonCopyable
{
    static const int FN_TITLE = 3,
            FN_COMPLETED = 4;
    
    std::string key;
    uint64_t ts;
    
    std::string title;
    bool completed;
    
    Todo(const todo::user::Todo* src):
        key(src->key()->str()),
        ts(src->ts()),
        title(src->title()->str()),
        completed(src->completed())
    {
        
    }
};

using TodoStore = coreds::PojoStore<Todo, todo::user::Todo>;

} // todo
