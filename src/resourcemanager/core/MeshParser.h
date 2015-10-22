#ifndef MESHPARSER_H
#define MESHPARSER_H

#include "glm\glm.hpp"
#include <string>
#include <vector>
#include "../core/ModelFileParser.h"

using glm::vec2;
using glm::vec3;

class MeshParser : public ModelFileParser
{
private:
	std::vector<Vector3> m_finalPositions;
	std::vector<Vector3> m_finalNormals;
	std::vector<Vector3> m_finalTangent;
	std::vector<Vector3> m_finalBiTangent;
	std::vector<Vector2> m_finalTexCoords;


public:
	MeshParser(void);
	~MeshParser(void);

	bool Load(char* buffer, unsigned int bufSize);

	//std::vector<VertexPoint>* GetVertexPoints() { return &m_vertexPoints; }
	float* GetVertexDataPtr() { return &m_finalPositions[0].val[0]; }
	float* GetTexCoordDataPtr() { return &m_finalTexCoords[0].val[0]; }
	float* GetNormalDataPtr() { return &m_finalNormals[0].val[0]; }

	unsigned int GetVertexCount() { return m_finalPositions.size(); }

};

#endif