#ifndef PROPERTY_TYPE
#warning "PROPERTY_TYPE macro not defined before inclusion of property_types.h"
#define PROPERTY_TYPE(t)
#endif

// Tile properties
PROPERTY_TYPE(Walkable)
PROPERTY_TYPE(SlowWalking)
PROPERTY_TYPE(Swimmable)
PROPERTY_TYPE(Sailable)
PROPERTY_TYPE(Flyable)
PROPERTY_TYPE(SlowFlying)
PROPERTY_TYPE(Climbable)
PROPERTY_TYPE(Roadable)
PROPERTY_TYPE(Forest)

// Unit abilities
PROPERTY_TYPE(Walking)
PROPERTY_TYPE(Swimming)
PROPERTY_TYPE(Sailing)
PROPERTY_TYPE(Flying)
PROPERTY_TYPE(Climbing)
PROPERTY_TYPE(Forestry)

PROPERTY_TYPE(Transport)

PROPERTY_TYPE(Capturable)

// Combat move types
PROPERTY_TYPE(Archery)
PROPERTY_TYPE(Charge)
PROPERTY_TYPE(Healing)
PROPERTY_TYPE(Strike)

// Unit statistics
PROPERTY_TYPE(Moves)
PROPERTY_TYPE(Sight)
PROPERTY_TYPE(Attack)
PROPERTY_TYPE(Defence)
PROPERTY_TYPE(Damage)
PROPERTY_TYPE(Health)
