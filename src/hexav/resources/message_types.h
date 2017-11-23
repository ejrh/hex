#ifndef MSG_TYPE
#define MSG_TYPE(s, c)
#define DUMMY_MSG_TYPE
#endif


// Useful for wrapping a template argument for passing to a C preprocessor macro
// (CPP macros don't recognise < > as brackets, but do recognise , as a delimiter)
#define P(x,y) x,y

#define WM(t) WrapperMessage<t>
#define WM2(t1,t2) WrapperMessage2<t1,t2>
#define WM3(t1,t2,t3) WrapperMessage3<t1,t2,t3>
#define WM4(t1,t2,t3,t4) WrapperMessage4<t1,t2,t3,t4>

MSG_TYPE(ImageFile, WM(std::string))
MSG_TYPE(ImageLibraries, WM(std::string))
MSG_TYPE(ImageLibrary, WM2(Atom, std::string))
MSG_TYPE(ImageSet, WM2(std::string, ImageSeries))

MSG_TYPE(LoadSong, WM(std::string))
MSG_TYPE(SoundFile, WM(std::string))

#undef P

#ifdef DUMMY_MSG_TYPE
#undef MSG_TYPE
#undef DUMMY_MSG_TYPE
#endif
