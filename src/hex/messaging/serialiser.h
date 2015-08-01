#ifndef SERIALISER_H
#define SERIALISER_H

#include "hex/basics/error.h"


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
        out << "\"" << x << "\"";
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
        begin_set(set.size());
        for (typename std::set<T>::const_iterator iter = set.begin(); iter != set.end(); iter++) {
            *this << *iter;
        }
        end_set();
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

    void begin_set(int size) {
        if (need_seperator)
            out << ", ";
        out << "{";
        need_seperator = false;
    }

    void end_set() {
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


class Deserialiser {
public:
    Deserialiser(std::istream &in): in(in), expect_seperator(false) { }
    virtual ~Deserialiser() { }

    Deserialiser& operator>>(bool& x) {
        if (expect_seperator)
            skip_separator();
        int ch = in.get();
        x = (ch == 'y' || ch == 'Y' || ch == '1');
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
        if (in.peek() == '"') {
            skip_expected('"');
            std::stringbuf sbuf;
            in.get(sbuf, '"');
            skip_expected('"');
            x.assign(sbuf.str());
        } else {
            in >> x;
        }
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
        begin_set(size);
        for (int i = 0; i < size; i++) {
            T x;
            if (in.peek() == '}')
                break;
            *this >> x;
            set.insert(x);
        }
        end_set();
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

    void begin_set(int &size) {
        size = INT_MAX;
        if (expect_seperator)
            skip_separator();
        skip_expected('{');
        expect_seperator = false;
    }

    void end_set() {
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
            throw Error("Expected character: %c (%x) but got EOF", ch, ch);
        }
        if (x != ch) {
            throw Error("Expected character: %c (%x) but got: %c (%x)", ch, ch, x, x);
        }
    }

    void skip_separator() {
        skip_expected(',');
        while (peek() == ' ')
            skip_expected(' ');
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

        warn(buffer);
    }

private:
    std::istream &in;
    bool expect_seperator;
};


#endif
