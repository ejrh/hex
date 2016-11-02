#ifndef SERIALISER_H
#define SERIALISER_H

#include "hex/basics/io.h"


bool is_atom_char(int x);
bool needs_quoting(const std::string& str);
int get_character_escape(int ch);


class Serialiser {
public:
    Serialiser(std::ostream &out): out(out), need_seperator(false) { }
    virtual ~Serialiser() { }

    Serialiser& operator<<(const bool x) {
        if (need_seperator)
            out << ", ";
        out << (x ? 't' : 'f');
        need_seperator = true;
        return *this;
    }

    Serialiser& operator<<(const int x) {
        if (need_seperator)
            out << ", ";
        out << x;
        need_seperator = true;
        return *this;
    }

    Serialiser& operator<<(const std::string& x) {
        if (need_seperator)
            out << ", ";
        write_atom(x);
        need_seperator = true;
        return *this;
    }

    Serialiser& operator<<(const char *x) {
        if (need_seperator)
            out << ", ";
        out << x;
        need_seperator = true;
        return *this;
    }

    template <typename K, typename V>
    Serialiser& operator<<(const std::pair<K, V> p) {
        *this << p.first;
        out << ": ";
        need_seperator = false;
        *this << p.second;
        need_seperator = true;
        return *this;
    }

    template<typename T>
    inline Serialiser& operator<<(const std::vector<T>& vector) {
        begin_vector(vector.size());
        for (auto iter = vector.begin(); iter != vector.end(); iter++) {
            *this << *iter;
        }
        end_vector();
        return *this;
    }

    template<typename T>
    inline Serialiser& operator<<(const std::set<T>& set) {
        begin_map(set.size());
        for (auto iter = set.begin(); iter != set.end(); iter++) {
            *this << *iter;
        }
        end_map();
        return *this;
    }

    template<typename K, typename V>
    inline Serialiser& operator<<(const std::map<K, V>& map) {
        begin_map(map.size());
        for (auto iter = map.begin(); iter != map.end(); iter++) {
            *this << *iter;
        }
        end_map();
        return *this;
    }

    void type_begin_tuple(const char *type);
    void begin_vector(int size);
    void end_vector();
    void begin_map(int size);
    void end_map();
    void begin_tuple();
    void end_tuple();
    void end_record();
    void write_atom(const std::string& atom);

private:
    std::ostream &out;
    bool need_seperator;
};


// Technique for return value specialisation, from http://stackoverflow.com/a/15912228/63991
template<typename V>
struct default_value_return { typedef V type; };

template<typename V>
inline typename default_value_return<V>::type default_value() { return V(); }

template<>
struct default_value_return<int>{ typedef int type; };

template<>
inline default_value_return<int>::type default_value<int>() { return 1; }


class Deserialiser {
public:
    Deserialiser(std::istream &in): in(in), pos_buf(in), expect_seperator(false) { }
    virtual ~Deserialiser() { }

    Deserialiser& operator>>(bool& x) {
        skip_to_next();
        int ch = get();
        x = (ch == 'y' || ch == 'Y' || ch == '1' || ch == 't' || ch == 'T');
        expect_seperator = true;
        return *this;
    }

    Deserialiser& operator>>(int& x) {
        skip_to_next();
        in >> x;
        expect_seperator = true;
        return *this;
    }

    Deserialiser& operator>>(std::string& x) {
        skip_to_next();
        read_atom(x);
        expect_seperator = true;
        return *this;
    }

    template<typename K, typename V>
    Deserialiser& operator>>(std::pair<K, V>& p) {
        skip_to_next();
        in >> p.first;
        skip_expected(':');
        in >> p.second;
        expect_seperator = true;
        return *this;
    }

    template<typename T>
    inline Deserialiser& operator>>(std::vector<T>& vector) {
        int size;
        begin_vector(size);
        for (int i = 0; i < size; i++) {
            skip_whitespace();
            if (peek() == ']')
                break;
            skip_to_next();
            if (peek() == ']')
                break;
            T x;
            *this >> x;
            vector.push_back(x);
        }
        end_vector();
        return *this;
    }

    template<typename T>
    inline Deserialiser& operator>>(std::set<T>& set) {
        int size;
        begin_map(size);
        for (int i = 0; i < size; i++) {
            T x;
            if (peek() == '}')
                break;
            *this >> x;
            set.insert(x);
        }
        end_map();
        return *this;
    }

    template<typename K, typename V>
    inline Deserialiser& operator>>(std::map<K, V>& map) {
        int size;
        begin_map(size);
        for (int i = 0; i < size; i++) {
            K x;
            V y = default_value<V>();
            if (peek() == '}')
                break;
            *this >> x;
            if (peek() == ':') {
                skip_separator(':');
                *this >> y;
            }
            map[x] = y;
        }
        end_map();
        return *this;
    }

    void type_begin_tuple(std::string &type_name);
    void begin_vector(int &size);
    void end_vector();
    void begin_map(int &size);
    void end_map();
    void begin_tuple();
    void end_tuple();
    void end_record();
    void skip_to_next();
    void skip_expected(int ch);
    void skip_separator();
    void skip_separator(int ch);
    void skip_whitespace();
    void read_atom(std::string& atom);

    int get() {
        return in.get();
    }

    int peek() {
        return in.peek();
    }

    void error(const char *fmt, ...);

    int line() {
        return pos_buf.line();
    }

    int column() {
        return pos_buf.column();
    }

public:
    bool was_quoted;

private:
    std::istream &in;
    PositionStreambuf pos_buf;
    bool expect_seperator;
};


#endif
