#ifndef LOGGING_H
#define LOGGING_H

#define log_once(level, cache, key, args) \
do { \
    if (!cache.count(key)) { \
        BOOST_LOG_TRIVIAL(warning) << args; \
        cache.insert(key); \
    } \
} while (false)


// Hash function object for generic std::pair.  Defined here because it's mostly needed for
// caching log messages with a std::pair key.
template<typename T, typename U>
struct std::hash<std::pair<T, U> > {
    std::size_t operator()(const std::pair<T, U>& pair) const {
        std::hash<T> hash1;
        std::hash<U> hash2;
        return hash1(pair.first) + hash2(pair.second);
    }
};

#endif
