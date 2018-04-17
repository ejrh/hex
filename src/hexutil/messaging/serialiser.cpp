#include "common.h"

#include "hexutil/basics/error.h"
#include "hexutil/messaging/serialiser.h"


namespace hex {

bool is_atom_char(int x) {
    return std::isalnum(x) || x == '_' || x == '-' || x == '$';
}

bool needs_quoting(const std::string& str) {
    for (auto iter = str.begin(); iter != str.end(); iter++) {
        if (!is_atom_char(*iter))
            return true;
    }
    return false;
}

int get_character_escape(int ch) {
    switch (ch) {
        case '\n':
            return 'n';
        case '\\':
        case '"':
            return ch;
        default:
            return 0;
    };
}


class serialise_visitor: public boost::static_visitor<> {
public:
    serialise_visitor(Serialiser& serialiser): serialiser(serialiser) { }

    void operator()(const Atom& atom) const {
        serialiser << atom;
    }

    void operator()(const int& i) const {
        serialiser << i;
    }

    void operator()(const std::string& str) const {
        serialiser << str;
    }

private:
    Serialiser& serialiser;
};


Serialiser& Serialiser::operator<<(const Datum& datum) {
    boost::apply_visitor(serialise_visitor(*this), datum.value);
    return *this;
}

void Serialiser::type_begin_tuple(const Atom type) {
    if (need_seperator)
        out << ", ";
    out << type << "(";
    need_seperator = false;
}

void Serialiser::begin_vector(int size) {
    if (need_seperator)
        out << ", ";
    out << "[";
    need_seperator = false;
}

void Serialiser::end_vector() {
    out << "]";
    need_seperator = true;
}

void Serialiser::begin_map(int size) {
    if (need_seperator)
        out << ", ";
    out << "{";
    need_seperator = false;
}

void Serialiser::end_map() {
    out << "}";
    need_seperator = true;
}

void Serialiser::begin_tuple() {
    if (need_seperator)
        out << ", ";
    out << "(";
    need_seperator = false;
}

void Serialiser::end_tuple() {
    out << ")";
    need_seperator = true;
}

void Serialiser::end_record() {
    out << "\n";
    need_seperator = false;
}

void Serialiser::write_atom(const std::string& atom) {
    if (needs_quoting(atom)) {
        out << "\"";
        for (auto iter = atom.begin(); iter != atom.end(); iter++) {
            int ch = *iter;
            int escape = get_character_escape(ch);
            if (escape) {
                out << '\\' << (char) escape;
            } else
                out << (char) ch;
        }
        out << "\"";
    } else {
        out << atom;
    }
}


Deserialiser& Deserialiser::operator>>(Datum& datum) {
    std::string word;
    *this >> word;
    if (was_quoted) {
        datum.value = word;
    } else {
        char *end;
        int intval = std::strtol(word.c_str(), &end, 0);
        if (*end == '\0') {
            datum.value = intval;
        } else {
            Atom atom = AtomRegistry::atom(word);
            datum.value = atom;
        }
    }
    return *this;
}

Deserialiser& Deserialiser::operator>>(Term *& term) {
    skip_to_next();
    int line_no = line();
    if (peek() == '[') {
        std::vector<Term *> terms;
        *this >> terms;
        term = new CompoundTerm(terms);
    } else {
        Datum datum;
        *this >> datum;
        if (datum.is<Atom>() && peek() == '(') {
            //TODO reuse list parsing code for arguments
            CompoundTerm *compound_term = new CompoundTerm(datum.get_as_known<Atom>());
            skip_expected('(');
            expect_seperator = false;
            while (true) {
                skip_whitespace();
                int ch = peek();
                if (ch == ')')
                    break;
                skip_to_next();
                Term *subterm;
                *this >> subterm;
                compound_term->add_subterm(subterm);
            }
            skip_expected(')');
            term = compound_term;
        } else {
            term = new DatumTerm(datum);
        }
    }

    term->line_no = line_no;
    expect_seperator = true;
    return *this;
}

void Deserialiser::type_begin_tuple(Atom& type_name) {
    skip_to_next();
    std::stringbuf sbuf;
    in.get(sbuf, '(');
    type_name = sbuf.str();
    skip_expected('(');
    expect_seperator = false;
}

void Deserialiser::begin_vector(int &size) {
    size = INT_MAX;
    skip_to_next();
    if (peek() == '[')
        skip_expected('[');
    else
        size = 1;
    expect_seperator = false;
}

void Deserialiser::end_vector() {
    expect_seperator = false;
    skip_to_next();
    if (peek() == ']')
        skip_expected(']');
    expect_seperator = true;
}

void Deserialiser::begin_map(int &size) {
    size = INT_MAX;
    skip_to_next();
    skip_expected('{');
    expect_seperator = false;
}

void Deserialiser::end_map() {
    expect_seperator = false;
    skip_to_next();
    skip_expected('}');
    expect_seperator = true;
}

void Deserialiser::begin_tuple() {
    skip_to_next();
    skip_expected('(');
    expect_seperator = false;
}

void Deserialiser::end_tuple() {
    expect_seperator = false;
    skip_to_next();
    skip_expected(')');
    expect_seperator = true;
}

void Deserialiser::end_record() {
    int x;
    do {
        x = get();
    } while (x != '\n' && x != ';' && x != EOF);
    expect_seperator = false;
}

void Deserialiser::skip_to_next() {
    skip_whitespace();
    if (expect_seperator)
        skip_separator();
    skip_whitespace();
}

void Deserialiser::skip_expected(int ch) {
    int x = get();
    if (x == std::char_traits<char>::eof()) {
        throw Error() << " " << boost::format("Expected character: '%c' (%x) but got EOF") % (char) ch % ch;
    }
    if (x != ch) {
        throw Error() << boost::format("Expected character: '%c' (%x) but got: '%c' (%x)") % (char) ch % ch % (char) x % x;
    }
}

void Deserialiser::skip_separator() {
    skip_expected(',');
    while (peek() == ' ')
        skip_expected(' ');
    expect_seperator = false;
}

void Deserialiser::skip_separator(int ch) {
    skip_expected(ch);
    while (peek() == ' ')
        skip_expected(' ');
    expect_seperator = false;
}

void Deserialiser::skip_whitespace() {
    while (1) {
        int x = peek();
        while (std::isspace(x)) {
            get();
            x = peek();
        }

        if (x == '#') {
            while (x != '\n' && !in.eof()) {
                x = get();
            }
        } else {
            break;
        }
    }
}

void Deserialiser::read_atom(std::string& atom) {
    if (peek() == '"') {
        get();
        for (;;) {
            int x = get();
            if (x == '"')
                break;
            if (x == '\\') {
                x = get();
                if (x == 'n')
                    x = '\n';
            }
            atom.push_back(x);
        }
        was_quoted = true;
    } else {
        for (;;) {
            int x = peek();
            if (!is_atom_char(x))
                break;
            atom.push_back(get());
        }
        was_quoted = false;
    }
}

void Deserialiser::error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    va_end(args);

    BOOST_LOG_TRIVIAL(warning) << buffer;
}

};
