#include "common.h"

#include "hex/game/traits.h"

class TraitMap {
public:

#define TRAIT_TYPE(n) data[#n] = n;
    TraitMap() {
        #include "hex/game/trait_types.h"
    }
#undef TRAIT_TYPE

    std::map<std::string, TraitType> data;
};

TraitMap trait_map;

TraitType get_trait_type(const std::string& name) {
    std::map<std::string, TraitType>::iterator iter = trait_map.data.find(name);
    if (iter != trait_map.data.end())
        return iter->second;
    return UnknownTraitType;
}

const std::string get_trait_type_name(const TraitType trait_type) {

#define TRAIT_TYPE(n) case n: return std::string(#n); break;
    switch (trait_type) {
        #include "hex/game/trait_types.h"
    }
#undef TRAIT_TYPE

    return "unknown";
}
