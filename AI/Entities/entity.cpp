#include "entity.hpp"

Entity::Entity(Mesh* mesh,unsigned tex) : _mesh(mesh), _texture(tex)
{
    _tag = "";
}
