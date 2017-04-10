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

MSG_TYPE(ImageFile, WM(std::string))
MSG_TYPE(ImageLibrary, WM2(Atom, std::string))
MSG_TYPE(ImageSet, WM2(std::string, ImageSeries))

MSG_TYPE(CreateTileView, WM(TileViewDef))
MSG_TYPE(TileAnimation, WM2(int, ImageSeries))
MSG_TYPE(TileTransition, WM(TransitionDef))
MSG_TYPE(TileRoads, WM(ImageSeries))
MSG_TYPE(TileFeature, WM(FeatureDef))

MSG_TYPE(CreateUnitView, WM(UnitViewDef))
MSG_TYPE(UnitPaint, WM(Term *))
MSG_TYPE(UnitSounds, WM2(int, SoundSeries))

MSG_TYPE(CreateStructureView, WM(StructureViewDef))
MSG_TYPE(StructureAnimation, WM4(int, int, int, ImageSeries))
MSG_TYPE(StructurePaint, WM(Term *))

MSG_TYPE(CreateFactionView, WM(FactionViewDef))
MSG_TYPE(FactionImageSet, WM2(std::string, ImageSeries))

MSG_TYPE(LoadSong, WM(std::string))
MSG_TYPE(SoundFile, WM(std::string))

#undef P
