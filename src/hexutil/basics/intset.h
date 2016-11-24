#ifndef INTSET_H
#define INTSET_H

class IntSet {
public:
    static const unsigned int bitsize = sizeof(unsigned int)*8;

    IntSet(): val(0) { }
    IntSet(const IntSet &s): val(s.val) { }

    IntSet(const std::set<int>& s) {
        val = 0;
        for (auto iter = s.begin(); iter != s.end(); iter++) {
            unsigned int x = *iter;
            if (x >= bitsize)
                break;
            val |= (1 << x);
        }
    }

    bool contains(unsigned int i) const {
        return (val >> i) & 1;
    }

    void insert(unsigned int i) {
        val |= (1 << i);
    }

    void toggle(unsigned int i) {
        val ^= (1 << i);
    }

    void clear() {
        val = 0;
    }

    bool empty() const {
        return val == 0;
    }

    size_t size() const {
        size_t s = 0;
        for (unsigned int i = 0; i < bitsize; i++) {
            if ((val >> i) & 1)
                s++;
        }
        return s;
    }

    unsigned int last() const {
        int v = val;
        unsigned int l = (unsigned int) -1;
        while (v != 0) {
            l++;
            v >>= 1;
        }
        return l;
    }

    operator std::set<int>() const {
        std::set<int> s;
        for (unsigned int i = 0; i < bitsize; i++) {
            if ((val >> i) & 1)
                s.insert(i);
        }
        return s;
    }

public:
    unsigned int val;
};


inline std::ostream& operator<<(std::ostream& os, const IntSet& set) {
    os << "{";
    bool first = true;
    for (unsigned int i = 0; i < IntSet::bitsize; i++) {
        if (!set.contains(i))
            continue;
        if (!first)
            os << ", ";
        else
            first = false;
        os << i;
    }
    return os << "}";
}

#endif
