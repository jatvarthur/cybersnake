#include "GameObject.h"


GameObject::GameObject()
	: loc_{ 0, 0 }
{
}

GameObject::~GameObject()
{
}

std::string GameObject::getName() const
{
	return name_;
}

void GameObject::setName(const std::string& value)
{
	name_ = value;
}

Vector2i GameObject::getLoc() const
{
	return loc_;
}

void GameObject::setLoc(const Vector2i& value)
{
	loc_ = value;
}


BinaryStream::BinaryStream()
	: pos_(0)
{
}

void BinaryStream::write(const void* buf, int size)
{
	std::copy((const BYTE*)buf, (const BYTE*)buf + size, std::back_inserter(buffer_));
}

void BinaryStream::read(void* buf, int size)
{
	std::copy(buffer_.begin() + pos_, buffer_.begin() + pos_ + size, (BYTE*)buf);
	pos_ += size;
}

void BinaryStream::rewind()
{
	pos_ = 0;
}

const void* BinaryStream::data() const
{
	return buffer_.data();
}

int BinaryStream::size() const
{
	return (int)buffer_.size();
}


void Serializer::serialize(const GameObject* object, Stream* stream)
{
	const struct TypeInfo* typeInfo = object->getTypeInfo();

	uint16_t length = (uint16_t)typeInfo->className.size();
	stream->write(&length, sizeof(length));
	stream->write(typeInfo->className.data(), length);

	for (const auto& pi : typeInfo->properties) {
		length = (uint16_t)pi.name.size();
		stream->write(&length, sizeof(length));
		stream->write(pi.name.data(), length);

		switch (pi.type) {
		case TK_INT: 
			{
				int* p = (int*)((BYTE*)object + pi.offset);
				stream->write(p, sizeof(int));
			}
			break;

		case TK_STRING:
			{
				std::string* s = (std::string*)((BYTE*)object + pi.offset);
				length = (uint16_t)s->size();
				stream->write(&length, sizeof(length));
				stream->write(s->data(), length);
			}
			break;

		case TK_VEC2I:
			{
				Vector2i* v = (Vector2i*)((BYTE*)object + pi.offset);
				stream->write(&v->x, sizeof(int));
				stream->write(&v->y, sizeof(int));
			}
			break;
		}
	}
}

void Serializer::deserialize(GameObject* object, Stream* stream)
{
	const struct TypeInfo* typeInfo = object->getTypeInfo();

	uint16_t length;
	stream->read(&length, sizeof(length));
	std::string buf(length, '\0');
	stream->read(&buf[0], length);

	// property order in RTTI is very important(!), in DECLARE_PROP order
	for (const auto& pi : typeInfo->properties) {
		stream->read(&length, sizeof(length));
		std::string propName(length, '\0');
		stream->read(&propName[0], length);
		assert(pi.name == propName);

		switch (pi.type) {
		case TK_INT:
			{
				int* p = (int*)((BYTE*)object + pi.offset);
				stream->read(p, sizeof(int));
			}
			break;

		case TK_STRING:
			{
				std::string* s = (std::string*)((BYTE*)object + pi.offset);
				stream->read(&length, sizeof(length));
				s->resize(length);
				stream->read(&(*s)[0], length);
			}
			break;

		case TK_VEC2I:
			{
				Vector2i* v = (Vector2i*)((BYTE*)object + pi.offset);
				stream->read(&v->x, sizeof(int));
				stream->read(&v->y, sizeof(int));
			}
			break;
		}
	}

}
