#include <climits>
#include <cmath>
#include <cstdio>
#include <exception>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
#include <cctype>
#include <memory>
#include <utility>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <boost/heap/priority_queue.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/array.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <boost/crc.hpp>
#include <boost/variant.hpp>
#include <boost/regex.hpp>


#ifdef _WIN32
#define vsnprintf(dest, sz, fmt, args) vsnprintf_s(dest, sz, sz, fmt, args)
#define snprintf(dest, sz, fmt, ...) sprintf_s(dest, sz, fmt, __VA_ARGS__)
#endif


// Hash function object for generic std::pair.
template<typename T, typename U>
struct std::hash<std::pair<T, U> > {
    std::size_t operator()(const std::pair<T, U>& pair) const {
        std::hash<T> hash1;
        std::hash<U> hash2;
        return hash1(pair.first) + hash2(pair.second);
    }
};
