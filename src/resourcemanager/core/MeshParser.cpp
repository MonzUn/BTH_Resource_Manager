#include "MeshParser.h"
#include <fstream>
#include <sstream>

MeshParser::MeshParser() : ModelFileParser()
{
}

MeshParser::~MeshParser()
{
}

bool MeshParser::Load(char* buffer, unsigned int bufSize)
{
	int size;

	size = *buffer;
	memcpy(&size, buffer, sizeof(int));
	buffer += sizeof(int);
	m_finalPositions = std::vector<Vector3>(size / sizeof(Vector3));
	memcpy(&m_finalPositions[0], buffer, size);
	buffer += size;
	memory += size;


	size = *buffer;
	memcpy(&size, buffer, sizeof(int));
	buffer += sizeof(int);
	m_finalNormals = std::vector<Vector3>(size / sizeof(Vector3));
	memcpy(&m_finalNormals[0], buffer, size);
	buffer += size;
	memory += size;

	size = *buffer;
	memcpy(&size, buffer, sizeof(int));
	buffer += sizeof(int);
	m_finalTangent = std::vector<Vector3>(size / sizeof(Vector3));
	memcpy(&m_finalTangent[0], buffer, size);
	buffer += size;
	memory += size;

	size = *buffer;
	memcpy(&size, buffer, sizeof(int));
	buffer += sizeof(int);
	m_finalBiTangent = std::vector<Vector3>(size / sizeof(Vector3));
	memcpy(&m_finalBiTangent[0], buffer, size);
	buffer += size;
	memory += size;

	size = *buffer;
	memcpy(&size, buffer, sizeof(int));
	buffer += sizeof(int);
	m_finalTexCoords = std::vector<Vector2>(size / sizeof(Vector2));
	memcpy(&m_finalTexCoords[0], buffer, size);
	memory += size;

	return true;
}