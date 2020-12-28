#pragma once
#ifndef GAME_OBJECT_H__
#define GAME_OBJECT_H__

#include "system.h"

struct Vector2i {
	int x;
	int y;
};


enum TypeKind {
	TK_UNDEF,
	TK_INT,
	TK_STRING,
	TK_VEC2I
};

struct PropertyInfo {
	std::string name;
	enum TypeKind type;
	bool isArray;		// std::vector
	unsigned offset;
};

struct TypeInfo {
	std::string className;
	std::vector<PropertyInfo> properties;
};

#define DECLARE_TYPE_INFO(className) \
	virtual const TypeInfo* getTypeInfo() const \
	{ \
		static TypeInfo typeInfo = { \
			#className,

#define END_TYPE_INFO() \
		}; \
		return &typeInfo; \
	}

#define BEGIN_PROPS() std::vector<PropertyInfo>({
#define END_PROPS() })

#define DECLARE_PROP(className, propName, propType, isArray) \
	{ #propName,  propType,  isArray, (unsigned)&((className*)(nullptr))->propName ## _ },


class GameObject {
public:
	DECLARE_TYPE_INFO(GameObject)
		BEGIN_PROPS()
			DECLARE_PROP(GameObject, name, TK_STRING, false)
			DECLARE_PROP(GameObject, loc,  TK_VEC2I,  false)
		END_PROPS()
	END_TYPE_INFO()

public:
	GameObject();
	virtual ~GameObject() noexcept;

	std::string getName() const;
	void setName(const std::string& value);
	
	Vector2i getLoc() const;
	void setLoc(const Vector2i& value);

private:
	std::string name_;
	Vector2i loc_;
};


class Stream {
public:
	virtual ~Stream() {}

	virtual void write(const void* buf, int size) = 0;
	virtual void read(void* buf, int size) = 0;
};

class BinaryStream: public Stream {
public:
	BinaryStream();

	virtual void write(const void* buf, int size) override;
	virtual void read(void* buf, int size) override;

	void rewind();
	
	const void* data() const;
	int size() const;

private:
	std::vector<BYTE> buffer_;
	int pos_;
};

class Serializer {
public:
	void serialize(const GameObject* object, Stream* stream);
	void deserialize(GameObject* object, Stream* stream);
};



#endif
