#ifndef SERIALISER_H
#define SERIALISER_H

#include "hex/basics/error.h"


inline bool is_atom_char(int x) {
    return std::isalnum(x) || x == '_';
}

inline bool needs_quoting(const std::string& str) {
    for (std::string::const_iterator iter = str.begin(); iter != str.end(); iter++) {
        if (!is_atom_char(*iter))
            return true;
    }
    return false;
}

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
        if (needs_quoting(x)) {
            out << "\"" << x << "\"";
        } else {
            out << x;
        }
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
        for (typename std::vector<T>::const_iterator iter = vector.begin(); iter != vector.end(); iter++) {
            *this << *iter;
        }
        end_vector();
        return *this;
    }

    template<typename T>
    inline Serialiser& operator<<(const std::set<T>& set) {
        begin_map(set.size());
        for (typename std::set<T>::const_iterator iter = set.begin(); iter != set.end(); iter++) {
            *this << *iter;
        }
        end_map();
        return *this;
    }

    template<typename K, typename V>
    inline Serialiser& operator<<(const std::map<K, V>& map) {
        begin_map(map.size());
        for (typename std::map<K, V>::const_iterator iter = map.begin(); iter != map.end(); iter++) {
            *this << *iter;
        }
        end_map();
        return *this;
    }

    void type_begin_tuple(const char *type) {
        if (need_seperator)
            out << ", ";
        out << type << "(";
        need_seperator = false;
    }

    void begin_vector(int size) {
        if (need_seperator)
            out << ", ";
        out << "[";
        need_seperator = false;
    }

    void end_vector() {
        out << "]";
        need_seperator = true;
    }

    void begin_map(int size) {
        if (need_seperator)
            out << ", ";
        out << "{";
        need_seperator = false;
    }

    void end_map() {
        out << "}";
        need_seperator = true;
    }

    void begin_tuple() {
        if (need_seperator)
            out << ", ";
        out << "(";
        need_seperator = false;
    }

    void end_tuple() {
        out << ")";
        need_seperator = true;
    }

    void end_record() {
        out << "\n";
        need_seperator = false;
    }

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
    Deserialiser(std::istream &in): in(in), expect_seperator(false) { }
    virtual ~Deserialiser() { }

    Deserialiser& operator>>(bool& x) {
        if (expect_seperator)
            skip_separator();
        int ch = in.get();
        x = (ch == 'y' || ch == 'Y' || ch == '1' || ch == 't' || ch == 'T');
        expect_seperator = true;
        return *this;
    }

    Deserialiser& operator>>(int& x) {
        if (expect_seperator)
            skip_separator();
        in >> x;
        expect_seperator = true;
        return *this;
    }

    Deserialiser& operator>>(std::string& x) {
        if (expect_seperator)
            skip_separator();
        read_atom(x);
        expect_seperator = true;
        return *this;
    }

    template<typename K, typename V>
    Deserialiser& operator>>(std::pair<K, V>& p) {
        if (expect_seperator)
            skip_separator();
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
            T x;
            if (in.peek() == ']')
                break;
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
            if (in.peek() == '}')
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
            if (in.peek() == '}')
                break;
            *this >> x;
            if (in.peek() == ':') {
                skip_separator(':');
                *this >> y;
            }
            map[x] = y;
        }
        end_map();
        return *this;
    }

    void type_begin_tuple(std::string &type_name) {
        if (expect_seperator)
            skip_separator();
        std::stringbuf sbuf;
        in.get(sbuf, '(');
        type_name.assign(sbuf.str());
        skip_expected('(');
        expect_seperator = false;
    }

    void begin_vector(int &size) {
        size = INT_MAX;
        if (expect_seperator)
            skip_separator();
        skip_expected('[');
        expect_seperator = false;
    }

    void end_vector() {
        skip_expected(']');
        expect_seperator = true;
    }

    void begin_map(int &size) {
        size = INT_MAX;
        if (expect_seperator)
            skip_separator();
        skip_expected('{');
        expect_seperator = false;
    }

    void end_map() {
        skip_expected('}');
        expect_seperator = true;
    }

    void begin_tuple() {
        if (expect_seperator)
            skip_separator();
        skip_expected('(');
        expect_seperator = false;
    }

    void end_tuple() {
        skip_expected(')');
        expect_seperator = true;
    }

    void end_record() {
        if (peek() == '\r')
            skip_expected('\r');
        skip_expected('\n');
        expect_seperator = false;
    }

    void skip_expected(int ch) {
        int x = in.get();
        if (x == std::char_traits<char>::eof()) {
            throw Error() << " " << boost::format("Expected character: '%c' (%x) but got EOF") % (char) ch % ch;
        }
        if (x != ch) {
            throw Error() << boost::format("Expected character: '%c' (%x) but got: '%c' (%x)") % (char) ch % ch % (char) x % x;
        }
    }

    void skip_separator() {
        skip_expected(',');
        while (peek() == ' ')
            skip_expected(' ');
        expect_seperator = false;
    }

    void skip_separator(int ch) {
        skip_expected(ch);
        while (peek() == ' ')
            skip_expected(' ');
        expect_seperator = false;
    }

    void read_atom(std::string& atom) {
        if (in.peek() == '"') {
            in.get();
            for (;;) {
                int x = in.get();
                if (x == '"')
                    break;
                if (x == '\\') {
                    x = in.get();
                    if (x == 'n')
                        x = '\n';
                }
                atom.push_back(x);
            }
            was_quoted = true;
        } else {
            for (;;) {
                int x = in.peek();
                if (!is_atom_char(x))
                    break;
                atom.push_back(in.get());
            }
            was_quoted = false;
        }
    }

    int peek() {
        return in.peek();
    }

    void error(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);

        char buffer[2048];
        vsnprintf(buffer, sizeof(buffer), fmt, args);

        va_end(args);

        BOOST_LOG_TRIVIAL(warning) << buffer;
    }

public:
    bool was_quoted;

private:
    std::istream &in;
    bool expect_seperator;
};


#endif
