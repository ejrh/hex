#ifndef TRAIT_TYPE
#warning "TRAIT_TYPE macro not defined before inclusion of trait_types.h"
#define TRAIT_TYPE(t)
#endif

// Tile properties
TRAIT_TYPE(Walkable)
TRAIT_TYPE(SlowWalking)
TRAIT_TYPE(Swimmable)
TRAIT_TYPE(Flyable)
TRAIT_TYPE(SlowFlying)
TRAIT_TYPE(Climbable)
TRAIT_TYPE(Roadable)

// Unit abilities
TRAIT_TYPE(Walking)
TRAIT_TYPE(Swimming)
TRAIT_TYPE(Flying)
TRAIT_TYPE(Climbing)

TRAIT_TYPE(LongSight)
