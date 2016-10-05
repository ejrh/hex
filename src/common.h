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
#include <set>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
#include <cctype>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
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

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <mikmod.h>


#ifdef _WIN32
#define vsnprintf(dest, sz, fmt, args) vsnprintf_s(dest, sz, sz, fmt, args)
#define snprintf(dest, sz, fmt, ...) sprintf_s(dest, sz, fmt, __VA_ARGS__)
#endif
