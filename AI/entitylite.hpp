#ifndef DX1220_ENTITYLITE_HPP
#define DX1220_ENTITYLITE_HPP

#include <utility>

using vec2 = std::pair<int,int>;

enum MODIFIERS
{
    NOMOD,
    PUSHABLE
};

struct EntityLite
{
    MODIFIERS modifier {NOMOD};
    unsigned texture;
};


#endif //DX1220_ENTITYLITE_HPP
