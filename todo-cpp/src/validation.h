#pragma once

#include <coreds/nana/ui.h>
#include "util.h"

namespace validation {

bool valid_string(ui::w$::Input& input,
        std::string& newVal,
        coreds::HasState<const std::string&>& hasMsg,
        const std::string& msgRequired = "")
{
    bool valid = true;
    auto sz = newVal.size();
    util::trim(newVal);
    
    if (!newVal.empty())
    {
        // TODO regex validator
        
        // update input if there was whitespace
        if (sz != 0)
            input.$.caption(newVal);
    }
    else if (!msgRequired.empty())
    {
        input.$.focus();
        
        // clear input if every char was whitespace
        if (sz != 0)
            input.$.caption("");
        
        hasMsg.update(msgRequired);
        valid = false;
    }
    else if (sz != 0)
    {
        // clear input since every char was whitespace
        input.$.caption("");
    }
    
    return valid;
}

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
        // TODO regex validator
        
        // update input if there was whitespace
        if (sz != newVal.size())
            input.$.caption(newVal);
        
        *updated = true;
    }
    
    return valid;
}

} // validation
