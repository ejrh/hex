#ifndef MSG_TYPE
#warning "MSG_TYPE macro not defined before inclusion of message_types.h"
#define MSG_TYPE(s, c)
#endif

// Useful for wrapping a template argument for passing to a C preprocessor macro
// (CPP macros don't recognise < > as brackets, but do recognise , as a delimiter)
#define P(x,y) x,y

MSG_TYPE(ImageFile, WrapperMessage<std::string>)
MSG_TYPE(ImageSet, P(WrapperMessage2<std::string, ImageSeries>))

MSG_TYPE(CreateTileView, WrapperMessage<TileViewDef>)
MSG_TYPE(TileAnimation, WrapperMessage<ImageSeries>)
MSG_TYPE(CreateUnitView, WrapperMessage<UnitViewDef>)
MSG_TYPE(UnitAnimation, P(WrapperMessage2<int, ImageSeries>))

MSG_TYPE(CreateUnitType, WrapperMessage<UnitType>)
MSG_TYPE(CreateTileType, WrapperMessage<TileType>)

MSG_TYPE(SetLevel, P(WrapperMessage2<int, int>))
MSG_TYPE(SetLevelData, P(WrapperMessage2<Point, std::vector<std::string> >))
MSG_TYPE(CreateStack, CreateStackMessage)
MSG_TYPE(CreateUnit, CreateUnitMessage)
MSG_TYPE(PlayerReady, PlayerReadyMessage)
MSG_TYPE(PlayerName, PlayerNameMessage)
MSG_TYPE(Chat, WrapperMessage<std::string>)
MSG_TYPE(TurnEnd, TurnEndMessage)
MSG_TYPE(TurnBegin, TurnBeginMessage)
MSG_TYPE(UnitMove, UnitMoveMessage)

MSG_TYPE(StreamOpen, WrapperMessage<std::string>)
MSG_TYPE(StreamClose, Message)
MSG_TYPE(StreamReplay, WrapperMessage<int>)

#undef P
