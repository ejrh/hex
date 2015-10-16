#ifndef ERROR_H
#define ERROR_H

template<class E>
class BaseError: public std::exception {
public:
    BaseError() { }
    virtual ~BaseError() throw() { }
    virtual const char *what() const throw() { return message.c_str(); }

    template<typename T>
    E &operator<<(const T& x) {
        std::ostringstream ss;
        ss << x;
        message.append(ss.str());
        return *static_cast<E *>(this);
    }

private:
    std::string message;
};

class Error: public BaseError<Error> {
};

#endif
