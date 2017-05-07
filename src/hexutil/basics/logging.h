#ifndef LOGGING_H
#define LOGGING_H

#define log_once(level, cache, key, args) \
do { \
    if (!cache.count(key)) { \
        BOOST_LOG_TRIVIAL(warning) << args; \
        cache.insert(key); \
    } \
} while (false)

#endif
