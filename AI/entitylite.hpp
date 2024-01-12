#ifndef DX1220_ENTITYLITE_HPP
#define DX1220_ENTITYLITE_HPP

#include <utility>
#include <vector>

using vec2 = std::pair<int,int>;
using vec2f = std::pair<float,float>;

enum MODIFIERS
{
    NOMOD,
    PUSHABLE
};

struct EntityLite
{
    MODIFIERS modifier {NOMOD};
    unsigned texture;
    std::vector<vec2> course;
    int action_points = 0;
    int base_points = 0;
};


#endif //DX1220_ENTITYLITE_HPP
