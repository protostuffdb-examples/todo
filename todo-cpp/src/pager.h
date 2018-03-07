#pragma once

#include <coreds/pstore.h>
#include "ui.h"

namespace ui {

template <typename T, typename F, typename W>
struct Pager : ui::Panel
{
    coreds::PojoStore<T, F> store;
    
    std::function<void()> $beforePopulate{
        std::bind(&Pager::beforePopulate, this)
    };
    std::function<void(const nana::arg_keyboard& arg)> $navigate{
        std::bind(&Pager::navigate, this, std::placeholders::_1)
    };
    std::function<void(nana::label::command cmd, const std::string& target)> $onLabelEvent{
        std::bind(&Pager::onLabelEvent, this, std::placeholders::_1, std::placeholders::_2)
    };
protected:
    virtual void beforePopulate() = 0;
    virtual void afterPopulate(int selectedIdx) = 0;
private:
    std::forward_list<W> items;
    std::vector<W*> array;
    nana::color selected_bg;
    int selected_idx{ -1 };
    
public:    
    Pager(nana::widget& owner, const char* layout = nullptr, unsigned selected_bg = 0xF3F3F3):
        ui::Panel(owner, layout ? layout : "margin=[5,0] <items_ vert>"),
        selected_bg(nana::color_rgb(selected_bg))
    {
        
    }
    
    void collocate(int pageSize = 10)
    {
        for (int i = 0; i < pageSize; i++)
        {
            items.emplace_front(*this);
            place["items_"] << items.front();
            array.push_back(&items.front());
        }
        
        place.collocate();
    }
    
    int size()
    {
        return array.size();
    }
    
    void populate(int idx, T* pojo)
    {
        array[idx]->update(pojo);
    }
    
    bool trySelect(int idx)
    {
        int prev_idx = selected_idx;
        if (idx == prev_idx)
            return false;
        
        if (idx == -1)
        {
            // deselect
            selected_idx = idx;
            array[prev_idx]->bgcolor(nana::colors::white);
            return true;
        }
        
        if (idx < 0 || idx >= array.size())
            return false;
        
        selected_idx = idx;
        
        if (prev_idx != -1)
            array[prev_idx]->bgcolor(nana::colors::white);
        
        array[idx]->bgcolor(selected_bg);
        return true;
    }
    
    void select(int idx)
    {
        if (trySelect(idx))
            store.select(idx);
    }
    
    void onLabelEvent(nana::label::command cmd, const std::string& target)
    {
        if (nana::label::command::click != cmd)
            return;
        
        int i = std::atoi(target.c_str());
        switch (i)
        {
            case 0:
            case 1:
                store.toggleDesc();
                break;
            case 3: // refresh
                store.fetchUpdate();
                break;
            case 4:
                store.pageTo(0);
                break;
            case 5:
                if (0 == (i = store.getPage()))
                    store.fetch(store.isDesc());
                else
                    store.pageTo(i - 1);
                break;
            case 6:
                if (store.getPageCount() == (i = store.getPage()))
                    store.fetch(!store.isDesc());
                else
                    store.pageTo(i + 1);
                break;
            case 7:
                store.pageTo(store.getPageCount());
                break;
        }
    }
    
    void navigate(const nana::arg_keyboard& arg)
    {
        int idx = store.getSelectedIdx();
        switch (arg.key)
        {
            case nana::keyboard::os_arrow_up:
                if (arg.ctrl)
                {
                    select(0);
                }
                else if (-1 == idx)
                {
                    select(store.getVisibleCount() - 1);
                }
                else if (0 != idx)
                {
                    select(idx - 1);
                }
                else if (0 != store.getPage())
                {
                    store.pageTo(store.getPage() - 1, $beforePopulate);
                    afterPopulate(store.getVisibleCount() - 1);
                }
                break;
            case nana::keyboard::os_arrow_down:
                if (arg.ctrl)
                {
                    select(store.getVisibleCount() - 1);
                }
                else if (-1 == idx)
                {
                    select(0);
                }
                else if (++idx != store.getVisibleCount())
                {
                    select(idx);
                }
                else if (store.getPageCount() != store.getPage())
                {
                    store.pageTo(store.getPage() + 1, $beforePopulate);
                    afterPopulate(0);
                }
                break;
            case nana::keyboard::os_arrow_left:
                if (arg.ctrl)
                    store.pageTo(0);
                else if (0 == store.getPage())
                    store.fetch(store.isDesc());
                else
                    store.pageTo(store.getPage() - 1);
                break;
            case nana::keyboard::os_arrow_right:
                if (arg.ctrl)
                    store.pageTo(store.getPageCount());
                else if (store.getPageCount() == store.getPage())
                    store.fetch(!store.isDesc());
                else
                    store.pageTo(store.getPage() + 1);
                break;
            case nana::keyboard::space:
                if (arg.ctrl)
                    store.fetchUpdate();
                else if (arg.shift)
                    store.toggleDesc();
                break;
        }
    }
};

} // ui
