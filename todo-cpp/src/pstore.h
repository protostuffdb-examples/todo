#pragma once

#include <functional>
#include <deque>
#include <vector>

#include <flatbuffers/flatbuffers.h>

namespace coreds {

enum class FetchType
{
    NONE,
    NEWER,
    OLDER,
    UPDATE
};

enum class SelectionType
{
    NONE = 0,
    RESET = 1,
    
    SELECT = 2,
    RETAIN = 3,
    RESELECT = 4
};

enum class SelectionFlags : uint8_t
{
    NONE = 0,
    CLICKED = 1,
    REFRESH = 2,
    CLICKED_UPDATE = 4,
    FORCE = 8,

    MASK_FORCE_OR_UPDATE = FORCE | CLICKED_UPDATE
};

struct ParamRangeKey
{
    bool desc{ true };
    int limit{ 10 };
    const char* start_key;
    const char* parent_key{ nullptr };
    
    void stringifyTo(std::string& buf)
    {
        buf += R"({"1":)";
        if (desc)
            buf += "true";
        else
            buf += "false";
        
        buf += R"(,"2":)";
        buf += std::to_string(limit);
        
        if (start_key)
        {
            buf += R"(,"3":")";
            buf.append(start_key, 12);
            buf += '"';
        }
        
        if (parent_key)
        {
            buf += R"(,"4":")";
            buf.append(parent_key, 12);
            buf += '"';
        }
        
        buf += '}';
    }
};

struct Opts
{
    int pageSize{ 10 };
    int multiplier{ 3 };
    //bool multiplier_conditional{ false };
};

enum class EventType
{
    DESC,
    LOADING,
    VISIBLE
};

template <typename T, typename F>
struct PojoStore
{
private:
    const std::function<void()> $populate = std::bind(&PojoStore::populate, this);
    std::deque<T> list;
    T* startObj{ nullptr };
    
    bool loading_{ false };
    bool desc_{ true };
    
    int page{ 0 };
    int pageSize{ 10 };
    int multiplier { 3 };
    
    FetchType fetchType { FetchType::NONE };
    
public:
    std::string errmsg;
    
    std::function<const char*(const T& pojo)> $fnKey;
    std::function<const char*(const F* message)> $fnKeyFB;
    std::function<bool(ParamRangeKey prk)> $fnFetch;
    std::function<void(T& pojo, const F* message)> $fnUpdate;
    std::function<void(EventType type, bool on)> $fnEvent;
    std::function<void(int idx, T* pojo)> $fnPopulate;
    std::function<void(std::function<void()> op)> $fnCall;
    
    PojoStore()
    {
        
    }
    bool isLoading()
    {
        return loading_;
    }
    void init(Opts opts)
    {
        pageSize = opts.pageSize;
        multiplier = opts.multiplier;
    }
    const T& latest()
    {
        return desc_ ? list.front() : list.back();
    }
    bool isVisible(int idx)
    {
        int pageStart = page * pageSize;
        return pageStart == idx || (idx > pageStart && idx < (pageStart + pageSize));
    }
    bool toggleDesc()
    {
        if (loading_)
            return false;
        
        desc_ = !desc_;
        
        $fnEvent(EventType::VISIBLE, false);
        
        // TODO iterate backwards and repaint
        
        $fnEvent(EventType::DESC, desc_);
        
        $fnEvent(EventType::VISIBLE, true);
        return true;
    }
    void select(int idx, SelectionFlags flags)
    {
        // TODO
    }
    void populate()
    {
        int i = 0;
        int offset = page * pageSize;
        int len = std::min(pageSize, static_cast<int>(list.size()));
        for (; i < len; i++) $fnPopulate(i, &list[offset + i]);
        for (; i < pageSize; i++) $fnPopulate(i, nullptr);
    }
    /*
     * Returns true if an item was removed.
     */
    bool update(const flatbuffers::Vector<flatbuffers::Offset<F>>* updateList)
    {
        int populatePages, populateLen, idx, i, removed;
        int size = list.size();
        int updateLen = updateList == nullptr ? 0 : updateList->size();
        if (updateLen == 0)
        {
            if (size <= pageSize)
            {
                list.clear();
                return true;
            }
            
            populatePages = page * pageSize;
            list.erase(list.begin() + populatePages, list.begin() + (populatePages + size));
            $fnEvent(EventType::VISIBLE, false);
            return true;
        }
        
        populateLen = pageSize;
        i = 0;
        removed = 0;
        populatePages = page * populateLen;
        idx = populatePages;
        
        for (;;)
        {
            auto update = updateList->Get(i);
            auto& existing = list[idx];
            if (0 == memcmp($fnKeyFB(update), $fnKey(existing), 9))
            {
                i++;
                
                $fnUpdate(existing, update);
                
                if (++idx == size)
                    break;
                
                if (i != updateLen)
                    continue;
                
                if (updateLen == populateLen)
                    break;
                
                list.erase(list.begin() + idx);
                removed++;
                size--;

                break;
            }
            
            list.erase(list.begin() + idx);
            removed++;
            size--;

            if (idx == size)
                break;
        }
        
        for (; i < updateLen; i++)
        {
            list.emplace_back(updateList->Get(i));
        }
        
        // TODO check if current page is affected before you populate
        $fnCall($populate);
        return removed != 0;
    }
    void prependAll(const flatbuffers::Vector<flatbuffers::Offset<F>>* p, bool reversed = false)
    {
        if (reversed)
        {
            for (int i = p->size(); i--> 0;)
                list.emplace_front(p->Get(i));
        }
        else
        {
            for (int i = 0, len = p->size(); i < len; i++)
                list.emplace_front(p->Get(i));
        }
        
        // TODO check if current page is affected before you populate
        $fnCall($populate);
    }
    void appendAll(const flatbuffers::Vector<flatbuffers::Offset<F>>* p, bool reversed = false)
    {
        if (reversed)
        {
            for (int i = p->size(); i--> 0;)
                list.emplace_back(p->Get(i));
        }
        else
        {
            for (int i = 0, len = p->size(); i < len; i++)
                list.emplace_back(p->Get(i));
        }
        
        // TODO check if current page is affected before you populate
        $fnCall($populate);
    }
    bool cbFetchFailed() {
        if (fetchType == FetchType::NONE)
            return false;
        
        fetchType = FetchType::NONE;
        loading_ = false;
        $fnEvent(EventType::LOADING, false);
        return true;
    }
    bool cbFetchSuccess(const flatbuffers::Vector<flatbuffers::Offset<F>>* p) {
        if (fetchType == FetchType::NONE)
            return false;
        
        switch (fetchType) {
            case FetchType::NEWER:
                if (p == nullptr || 0 == p->size())
                    break;

                if (!list.empty() && desc_)
                    prependAll(p, true);
                else
                    appendAll(p);
              break;
            case FetchType::OLDER:
                if (p == nullptr || 0 == p->size())
                    break;
                
                if (desc_)
                    appendAll(p);
                else
                    prependAll(p, true);
                break;
            case FetchType::UPDATE:
                update(p);
                break;
        }
        
        fetchType = FetchType::NONE;
        loading_ = false;
        $fnEvent(EventType::LOADING, false);
        return true;
    }
    bool fetchNewer()
    {
        if (loading_)
            return false;
        
        bool empty = list.empty();
        if (empty && !desc_)
        {
            desc_ = true;
            $fnEvent(EventType::DESC, true);
        }
        
        ParamRangeKey prk;
        prk.desc = empty;
        prk.limit = empty ? pageSize * multiplier + 1 : (desc_ ? pageSize : pageSize * multiplier);
        prk.start_key = empty ? nullptr : $fnKey(latest());
        
        if (!$fnFetch(prk))
            return false;
        
        fetchType = FetchType::NEWER;
        loading_ = true;
        $fnEvent(EventType::LOADING, true);
        return true;
    }
    bool fetchOlder()
    {
        if (loading_ || list.empty())
            return false;
        
        ParamRangeKey prk;
        prk.desc = true;
        prk.limit = desc_ ? pageSize * multiplier : pageSize;
        prk.start_key = $fnKey(desc_ ? list.back() : list.front());
        
        if (!$fnFetch(prk))
            return false;
        
        fetchType = FetchType::OLDER;
        loading_ = true;
        $fnEvent(EventType::LOADING, true);
        return true;
    }
    bool fetchUpdate()
    {
        return false;
        /*
        if (list.empty())
            return fetchNewer();
        
        if (loading_)
            return false;
        
        // the first item in the visible list
        auto& pojo = list[page * pageSize];
        
        ParamRangeKey prk;
        prk.desc = desc_;
        prk.limit = std::min(pageSize, static_cast<int>(list.size()));
        // TODO implement base64 decode and increment key and encode again
        prk.start_key = nullptr;
        
        if (!$fnFetch(prk))
            return false;
        
        fetchType = FetchType::UPDATE;
        loading_ = true;
        $fnEvent(EventType::LOADING, true);
        return true;*/
    }
    bool appendPageInfoTo(std::string& buf)
    {
        int start, end;
        int size = list.size();
        if (size == 0)
        {
            // noop
        }
        else if (size == (start = (page * pageSize) + 1))
        {
            // 1 of 1
            const auto str = std::to_string(size);
            buf += str;
            buf += " of ";
            buf += str;
        }
        else if (size > (end = start + pageSize - 1))
        {
            buf += std::to_string(start);
            buf += " - ";
            buf += std::to_string(end);
            buf += " of ";
            buf += std::to_string(size);
        }
        else
        {
            const auto str = std::to_string(size);
            buf += std::to_string(start);
            buf += " - ";
            buf += str;
            buf += " of ";
            buf += str;
        }
        
        return size != 0;
    }
    // from ts verson
    /*int populate(SelectionType type, SelectionFlags flags,
            bool main, int idxSelected, int page)
    {
        return 0;
    }*/
};

} // coreds
