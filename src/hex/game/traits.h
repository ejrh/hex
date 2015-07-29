#ifndef TRAITS_H
#define TRAITS_H

class Unit;

#define TRAIT_TYPE(n) n,
enum TraitType {
    UnknownTraitType,
    #include "hex/game/trait_types.h"
    NUM_TRAIT_TYPES
};
#undef TRAIT_TYPE

typedef std::set<TraitType> TraitSet;

TraitType get_trait_type(const std::string& name);
const std::string get_trait_type_name(const TraitType trait_type);

#endif
