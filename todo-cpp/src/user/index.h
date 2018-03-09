#pragma once

#include "../util.h"
#include "../ui.h"

#include "todo.h"

namespace todo {
namespace user {

struct Index : ui::Panel, util::HasState<bool>
{
    todo::TodoPager pager_{ *this };
    
    Index(ui::Panel& owner, std::vector<util::HasState<bool>*>& container,
        const char* field, bool active = false) : ui::Panel(owner, 
        "vert"
        "<pager_>"
    )
    {
        container.push_back(this);
        
        place["pager_"] << pager_;
        
        place.collocate();
        
        owner.place[field] << *this;
        owner.place.field_display(field, active);
    }
    void update(bool on) override
    {
        if (on)
            pager_.store.fetchUpdate();
    }
};

} // user
} // todo
