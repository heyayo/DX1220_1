#ifndef DX1220_ENTITYLITE_HPP
#define DX1220_ENTITYLITE_HPP

#include <utility>

using vec2 = std::pair<int,int>;

struct EntityLite
{
    vec2 pos;
    unsigned texture;
};


#endif //DX1220_ENTITYLITE_HPP
