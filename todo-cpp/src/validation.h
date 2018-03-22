#pragma once

#include <coreds/nana/ui.h>
#include "util.h"

namespace validation {

bool update_string(ui::w$::Input& input,
        std::string& newVal, std::string& oldVal, bool* updated,
        coreds::HasState<const std::string&>& hasMsg,
        const std::string& msgRequired = "")
{
    bool valid = true;
    auto sz = newVal.size();
    util::trim(newVal);
    *updated = false;
    
    if (newVal.empty())
    {
        input.$.focus();
        input.$.caption(oldVal);
        
        if (!msgRequired.empty())
        {
            hasMsg.update(msgRequired);
            valid = false;
        }
    }
    else if (newVal == oldVal)
    {
        // reset input if there was whitespace
        if (sz != newVal.size())
            input.$.caption(newVal);
    }
    else
    {
        // set input if there was whitespace
        if (sz != newVal.size())
            input.$.caption(newVal);
        
        *updated = true;
    }
    
    return valid;
}

} // validation
