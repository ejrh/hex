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
MSG_TYPE(TileTransition, WM(TransitionDef))
MSG_TYPE(TileRoads, WM(ImageSeries))
MSG_TYPE(TileFeature, WM(FeatureDef))

MSG_TYPE(CreateUnitView, WM(UnitViewDef))
MSG_TYPE(UnitHoldAnimation, WM3(int, int, ImageSeries))
MSG_TYPE(UnitMoveAnimation, WM3(int, int, ImageSeries))
MSG_TYPE(UnitAttackAnimation, WM3(int, int, ImageSeries))
MSG_TYPE(UnitRecoilAnimation, WM3(int, int, ImageSeries))
MSG_TYPE(UnitDieAnimation, WM3(int, int, ImageSeries))
MSG_TYPE(UnitShadowAnimation, WM3(int, int, ImageSeries))

MSG_TYPE(CreateStructureView, WM(StructureViewDef))
MSG_TYPE(StructureAnimation, WM4(int, int, int, ImageSeries))

MSG_TYPE(CreateFactionView, WM(FactionViewDef))
MSG_TYPE(FactionImageSet, WM2(std::string, ImageSeries))

MSG_TYPE(LoadSong, WM(std::string))

MSG_TYPE(ClearGame, Message)
MSG_TYPE(CreateUnitType, WM(UnitType))
MSG_TYPE(CreateTileType, WM(TileType))
MSG_TYPE(CreateStructureType, WM(StructureType))

MSG_TYPE(SetLevel, WM2(int, int))
MSG_TYPE(SetLevelData, WM2(Point, CompressableStringVector))
MSG_TYPE(CreateFaction, WM3(int, std::string, std::string))

MSG_TYPE(CreateStack, WM3(int, Point, int))
MSG_TYPE(CreateUnit, WM2(int, std::string))
MSG_TYPE(TransferUnits, WM4(int, IntSet, Path, int))
MSG_TYPE(DestroyStack, WM(int))

MSG_TYPE(CreateStructure, WM3(Point, std::string, int))

MSG_TYPE(GrantFactionView, WM3(int, int, bool))
MSG_TYPE(GrantFactionControl, WM3(int, int, bool))

MSG_TYPE(Chat, WM(std::string))

MSG_TYPE(FactionReady, WM2(int, bool))
MSG_TYPE(TurnBegin, WM(int))
MSG_TYPE(TurnEnd, Message)

MSG_TYPE(UnitMove, WM4(int, IntSet, Path, int))
MSG_TYPE(DoBattle, WM3(int, Point, std::vector<Move>))

MSG_TYPE(StreamOpen, WM(std::string))
MSG_TYPE(StreamClose, Message)
MSG_TYPE(StreamReplay, WM2(int, int))
MSG_TYPE(StreamState, WM2(int, int))

#undef P
