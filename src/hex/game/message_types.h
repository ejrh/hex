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
MSG_TYPE(ImageFile, WM(std::string))
MSG_TYPE(ImageSet, WM2(std::string, ImageSeries))

MSG_TYPE(CreateTileView, WM(TileViewDef))
MSG_TYPE(TileAnimation, WM2(int, ImageSeries))
MSG_TYPE(TileTransition, WM3(ImageSeries, ImageSeries, ImageSeries))
MSG_TYPE(CreateUnitView, WM(UnitViewDef))
MSG_TYPE(UnitHoldAnimation, WM3(int, int, ImageSeries))
MSG_TYPE(UnitMoveAnimation, WM3(int, int, ImageSeries))
MSG_TYPE(CreateFactionView, WM(FactionViewDef))

MSG_TYPE(CreateUnitType, WM(UnitType))
MSG_TYPE(CreateTileType, WM(TileType))

MSG_TYPE(SetLevel, WM2(int, int))
MSG_TYPE(SetLevelData, WM2(Point, std::vector<std::string>))
MSG_TYPE(CreateFaction, WM3(int, std::string, std::string))

MSG_TYPE(CreateStack, WM3(int, Point, int))
MSG_TYPE(CreateUnit, WM2(int, std::string))

MSG_TYPE(FactionReady, WM2(int, bool))
MSG_TYPE(Chat, WM(std::string))
MSG_TYPE(TurnEnd, WM(int))
MSG_TYPE(TurnBegin, WM(int))

MSG_TYPE(UnitMove, WM4(int, std::set<int>, Path, int))

MSG_TYPE(StreamOpen, WM(std::string))
MSG_TYPE(StreamClose, Message)
MSG_TYPE(StreamReplay, WM2(int, int))
MSG_TYPE(StreamState, WM2(int, int))

#undef P
