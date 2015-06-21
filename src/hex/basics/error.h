#ifndef ERROR_H
#define ERROR_H

class Error: public std::exception {
public:
    Error(const char *fmt, ...);
    virtual ~Error() throw() { }
    virtual const char *what() const throw() { return message.c_str(); }

private:
    std::string message;
};

extern void warn(const char *fmt, ...);
extern void trace(const char *fmt, ...);

#endif