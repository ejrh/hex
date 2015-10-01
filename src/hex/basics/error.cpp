#include "common.h"

#include "hex/basics/error.h"


static boost::mutex error_lock;


Error::Error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    va_end(args);

    message.assign(buffer);
}
