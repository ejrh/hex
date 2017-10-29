#ifndef STRING_POOL_H
#define STRING_POOL_H

class StringPool {
public:
    class Entry {
    public:
        Entry(const std::string& str): str(str) {
        }

        bool operator<(const Entry& other) {
            return str < other.str;
        }

    public:
        std::string str;
    };

    struct EntryCmp {
        bool operator() (const Entry *lhs, const Entry *rhs) const { return lhs->str < rhs->str; }
    };

    StringPool() {
    }

    ~StringPool() {
        for (auto iter = data.begin(); iter != data.end(); iter++) {
            delete *iter;
        }
    }

    Entry *get(const std::string& str) {
        Entry e(str);
        auto iter = data.find(&e);
        if (iter != data.end()) {
            return *iter;
        }
        Entry *e_new = new Entry(e);
        data.insert(iter, e_new);
        return e_new;
    }

private:
    std::unordered_set<Entry *> data;
};

extern StringPool string_pool;

class poolstr {
public:
    poolstr(const std::string& str) {
        entry = string_pool.get(str);
    }

    operator const std::string&() const {
        return entry->str;
    }

    /*poolstr& operator=(const StringPool::index_type& index) {
        this->index = index;
        return *this;
    }*/

    bool operator==(const poolstr& other) const {
        return entry == other.entry;
    }

private:
    StringPool::Entry *entry;

    friend std::ostream& operator<<(std::ostream& os, const poolstr& ps);
};

inline std::ostream& operator<<(std::ostream& os, const poolstr& ps) {
    return os << ps.entry->str;
}

#endif
