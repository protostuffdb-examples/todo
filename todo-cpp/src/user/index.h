#pragma once

#include "../ui.h"

#include "todo.h"

namespace todo {
namespace user {

struct Index : ui::Panel
{
    todo::TodoPager pager_{ *this };
    
    Index(ui::Panel& owner, const char* field, const bool display = true) : ui::Panel(owner, 
        "vert"
        "<pager_>"
    )
    {
        place["pager_"] << pager_;
        
        place.collocate();
        
        owner.place[field] << *this;
        if (!display)
            owner.place.field_display(field, false);
    }
};

} // user
} // todo
