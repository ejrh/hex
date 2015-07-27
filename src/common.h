#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <exception>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <queue>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/heap/priority_queue.hpp>
#include <boost/algorithm/string.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#ifdef _WIN32
#define vsnprintf(dest, sz, fmt, args) vsnprintf_s(dest, sz, sz, fmt, args)
#define snprintf(dest, sz, fmt, ...) sprintf_s(dest, sz, fmt, __VA_ARGS__)
#endif
