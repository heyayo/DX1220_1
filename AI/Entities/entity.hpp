#ifndef DX1220_ENTITY_HPP
#define DX1220_ENTITY_HPP

#include "Mesh.h"
#include "Vector3.h"

class Entity
{
	Mesh* _mesh;
	unsigned int _texture;
	Vector3 _position{};
	Vector3 _rotation{};
	Vector3 _scale{};
	const char* _tag;
	
public:
	Entity(Mesh* mesh, unsigned tex);
	
	constexpr const Vector3& getPosition() { return _position; }
	constexpr const Vector3& getRotation() { return _rotation; }
	constexpr const Vector3& getScale()    { return _scale; }
	constexpr const char* getTag() { return _tag; }
	constexpr Mesh* getMesh() { return _mesh; }
	constexpr unsigned getTextureID() const { return _texture; }
	
	inline void setTag(const char* tag) { _tag = tag; }
	inline void setPosition(const Vector3& pos) { _position = pos; }
	inline void setRotation(const Vector3& rot) { _rotation = rot; }
	inline void setScale(const Vector3& scale) { _scale = scale; }
};


#endif
