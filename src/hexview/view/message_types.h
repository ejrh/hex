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

MSG_TYPE(CreateTileView, WM(TileViewDef))
MSG_TYPE(TilePaint, WM(Term *))
MSG_TYPE(TransitionPaint, WM(Term *))

MSG_TYPE(CreateFeatureView, WM(FeatureViewDef))
MSG_TYPE(FeaturePaint, WM(Term *))

MSG_TYPE(CreateUnitView, WM(UnitViewDef))
MSG_TYPE(UnitPaint, WM(Term *))
MSG_TYPE(UnitSounds, WM2(int, SoundSeries))

MSG_TYPE(CreateStructureView, WM(StructureViewDef))
MSG_TYPE(StructurePaint, WM(Term *))

MSG_TYPE(CreateFactionView, WM(FactionViewDef))
MSG_TYPE(FactionImageSet, WM2(std::string, ImageSeries))

#undef P

#ifdef DUMMY_MSG_TYPE
#undef MSG_TYPE
#undef DUMMY_MSG_TYPE
#endif
