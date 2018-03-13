#pragma once

#include <coreds/nana/ui.h>

#include "../util.h"

#include "TodoPager.h"

namespace todo {
namespace user {

struct Index : ui::Panel, util::HasState<bool>, util::HasState<const std::string&>
{
private:
    TodoPager pager_{ *this };
public:
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
        if (on && !pager_.store.loading())
            pager_.store.fetchUpdate();
    }
    void update(const std::string& msg) override
    {
        if (msg.empty())
            pager_.msg_.hide();
        else
            pager_.msg_.update(msg);
    }
    void init(coreds::Opts opts, util::RequestQueue& requestQueue)
    {
        pager_.init(opts, requestQueue);
    }
};

} // user
} // todo
