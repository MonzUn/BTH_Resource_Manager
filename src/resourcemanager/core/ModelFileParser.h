#ifndef MODELFILEPARSER_H
#define MODELFILEPARSER_H

struct Vector3
{
	float val[3];
};
struct Vector2
{
	float val[2];
};

class ModelFileParser
{
public:
	ModelFileParser() {}
	~ModelFileParser() {}

	virtual bool Load(char* buffer, unsigned int bufSize) = 0;

	virtual float* GetVertexDataPtr() = 0;
	virtual float* GetTexCoordDataPtr() { return nullptr; }
	virtual float* GetNormalDataPtr() { return nullptr; }

	virtual unsigned int GetVertexCount() = 0;
};

#endif