#ifndef OBJMAP_H
#define OBJMAP_H

#include "hex/basics/error.h"

template<typename K, typename T>
class ObjMap {
public:
    typedef typename std::map<K, typename T::pointer> map;
    typedef typename map::iterator iterator;
    typedef typename map::const_iterator const_iterator;

    ObjMap(const char *name): name(name) { }

    size_t size() const {
        return data.size();
    }

    void clear() {
        data.clear();
    }

    iterator begin() {
        return data.begin();
    }

    iterator end() {
        return data.end();
    }

    const_iterator begin() const {
        return data.begin();
    }

    const_iterator end() const {
        return data.end();
    }

    void put(const K& id, const typename T::pointer& obj) {
        if (data.find(id) != data.end())
            throw DataError() << "Item '" << id << "' already exists in " << name;
        data[id] = obj;
    }

    void put_and_warn(const K& id, const typename T::pointer& obj) {
        if (data.find(id) != data.end()) {
            BOOST_LOG_TRIVIAL(warning) << "Item '" << id << "' replaced in " << name;
        }
        data[id] = obj;
    }

    void remove(const K& id) {
        data.erase(id);
    }

    typename T::pointer& get(const K& id) const {
        if (data.find(id) == data.end())
            throw DataError() << "Item '" << id << "' not found in " << name;
        map& const_data = const_cast<map&>(data);
        return const_data[id];
    }

    typename T::pointer& get_and_warn(const K& id) {
        if (data.find(id) == data.end()) {
            BOOST_LOG_TRIVIAL(warning) << "Item '" << id << "' implicitly created in " << name;
            data[id] = boost::make_shared<T>(id);
        }
        return data[id];
    }

    typename T::pointer find(const K& id) {
        if (data.find(id) == data.end())
            return typename T::pointer();
        else
            return data[id];
    }

    const typename T::pointer find(const K& id) const {
        if (data.find(id) == data.end())
            return typename T::pointer();
        else
            return data[id];
    }

    const K& highest() const {
        return data.rbegin()->first;
    }

    operator map&() {
        return data;
    }

private:
    std::string name;
    map data;
};

template<typename T>
class StrMap: public ObjMap<std::string, T> {
public:
    StrMap(const char *name): ObjMap<std::string, T>(name) { }
};

template<typename T>
class IntMap: public ObjMap<int, T> {
public:
    IntMap(const char *name): ObjMap<int, T>(name) { }

    int get_free_id() const {
        return this->highest() + 1;
    }
};

#endif
