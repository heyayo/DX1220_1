#ifndef DX1220_ENTITY_HPP
#define DX1220_ENTITY_HPP

#include "Mesh.h"
#include "Vector3.h"

class Entity
{
	Mesh* _mesh;
	Vector3 _position{};
	Vector3 _rotation{};
	Vector3 _scale{};
	
public:
	Entity(Mesh* mesh);
	
	constexpr const Vector3& getPosition() { return _position; }
	constexpr const Vector3& getRotation() { return _rotation; }
	constexpr const Vector3& getScale()    { return _scale; }
	constexpr Mesh* getMesh() { return _mesh; }
	
	inline void setPosition(const Vector3& pos) { _position = pos; }
	inline void setRotation(const Vector3& rot) { _rotation = rot; }
	inline void setScale(const Vector3& scale) { _scale = scale; }
};


#endif
