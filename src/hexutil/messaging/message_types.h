#ifndef MSG_TYPE
#warning "MSG_TYPE macro not defined before inclusion of message_types.h"
#define MSG_TYPE(s, c)
#endif


// Useful for wrapping a template argument for passing to a C preprocessor macro
// (CPP macros don't recognise < > as brackets, but do recognise , as a delimiter)
#define P(x,y) x,y

#define WM(t) WrapperMessage<t>
#define WM2(t1,t2) WrapperMessage2<t1,t2>
#define WM3(t1,t2,t3) WrapperMessage3<t1,t2,t3>
#define WM4(t1,t2,t3,t4) WrapperMessage4<t1,t2,t3,t4>

MSG_TYPE(IncludeResource, WM(std::string))
MSG_TYPE(IncludeIfResourceExists, WM(std::string))

MSG_TYPE(StreamOpen, WM(std::string))
MSG_TYPE(StreamClose, Message)
MSG_TYPE(StreamReplay, WM2(int, int))
MSG_TYPE(StreamState, WM2(int, int))

MSG_TYPE(DefineScript, WM2(Atom, Term *))


#undef P
