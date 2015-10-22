#include "ObjParser.h"
#include <fstream>
#include <sstream>


ObjParser::ObjParser() : ModelFileParser()
{
}

ObjParser::~ObjParser()
{
}

bool ObjParser::Load(char* buffer, unsigned int bufSize)
{
	//f.rdbuf()->pubsetbuf(buf, bufSize);
	std::stringstream f;
	f.rdbuf()->sputn(buffer, (std::streamsize)bufSize); //this copies the buffer data into the stringstream, which sucks!

	std::string str;
	while (!f.eof())
	{
		f >> str;

		if (str == "#" || str == "s")	ParseComment(f);
		else if (str == "v")				ParsePosition(f);
		else if (str == "vn")			ParseNormal(f);
		else if (str == "vt")			ParseTexCoord(f);
		else if (str == "f")				ParseFace(f);

		str = "";
	}

	return true;
}

void ObjParser::ParseComment(std::stringstream& f)
{
	std::string str;
	std::getline(f, str);
}

void ObjParser::ParsePosition(std::stringstream& f)
{
	vec3 p;

	f >> p.x;
	f >> p.y;
	f >> p.z;

	mPositions.push_back(p);
}

void ObjParser::ParseTexCoord(std::stringstream& f)
{
	vec2 t;

	f >> t.x;
	f >> t.y;

	mTexCoords.push_back(t);
}

void ObjParser::ParseNormal(std::stringstream& f)
{
	vec3 n;

	f >> n.x;
	f >> n.y;
	f >> n.z;
	//n.z *= -1.0f;

	mNormals.push_back(n);
}

void ObjParser::ParseFace(std::stringstream& f)
{
	std::string tri;
	std::string sub;
	int count = 0;
	for (int i = 0; i < 3; ++i)
	{
		f >> tri;
		int v[3];
		for (int j = 0; j < 3; ++j)
		{
			sub = tri.substr(0, tri.find("/"));
			int off = tri.find("/");
			tri = tri.substr(off + 1);
			v[j] = atoi(sub.c_str()) - 1;

			if (off < 0)
				break;
		}
		vec3 vp = vec3(0, 0, 0);
		vec3 vn = vec3(0, 0, 0);
		vec2 vt = vec2(0, 0);

		if (v[0] >= 0)
			vp = mPositions[v[0]];
		if (v[1] >= 0)
			vt = mTexCoords[v[1]];
		if (v[2] >= 0)
			vn = mNormals[v[2]];

		//m_vertexPoints.push_back(VertexPoint(vp, vt, vn));

		Vector3 vertex;
		Vector2 texCoord;
		Vector3 normal;

		vertex.val[0] = vp.x;
		vertex.val[1] = vp.y;
		vertex.val[2] = vp.z;
		m_finalPositions.push_back(vertex);

		texCoord.val[0] = vt.x;
		texCoord.val[1] = vt.y;
		m_finalTexCoords.push_back(texCoord);

		normal.val[0] = vn.x;
		normal.val[1] = vn.y;
		normal.val[2] = vn.z;
		m_finalNormals.push_back(normal);

		count++;
	}
	//SetTangents(mCurrentGroup->mVertices.size() - 3);


	if (f.peek() != '\n')
	{
		std::string str;
		f >> str;
		int v[3];
		for (int j = 0; j < 3; ++j)
		{
			sub = str.substr(0, str.find("/"));
			int off = str.find("/");
			str = str.substr(off + 1);
			v[j] = atoi(sub.c_str()) - 1;
			if (off < 0)
				break;
		}
		vec3 vp = vec3(0, 0, 0);
		vec3 vn = vec3(0, 0, 0);
		vec2 vt = vec2(0, 0);

		if (v[0] >= 0)
			vp = mPositions[v[0]];
		if (v[1] >= 0)
			vt = mTexCoords[v[1]];
		if (v[2] >= 0)
			vn = mNormals[v[2]];

		Vector3 vertex;
		Vector2 texCoord;
		Vector3 normal;

		//m_vertexPoints.push_back(m_vertexPoints[m_vertexPoints.size() - 3]);
		vertex.val[0] = m_finalPositions[m_finalPositions.size() - 3].val[0];
		vertex.val[1] = m_finalPositions[m_finalPositions.size() - 3].val[1];
		vertex.val[2] = m_finalPositions[m_finalPositions.size() - 3].val[2];
		m_finalPositions.push_back(vertex);

		texCoord.val[0] = m_finalTexCoords[m_finalTexCoords.size() - 3].val[0];
		texCoord.val[1] = m_finalTexCoords[m_finalTexCoords.size() - 3].val[1];
		m_finalTexCoords.push_back(texCoord);

		normal.val[0] = m_finalNormals[m_finalNormals.size() - 3].val[0];
		normal.val[1] = m_finalNormals[m_finalNormals.size() - 3].val[1];
		normal.val[2] = m_finalNormals[m_finalNormals.size() - 3].val[2];
		m_finalNormals.push_back(normal);

		//m_vertexPoints.push_back(m_vertexPoints[m_vertexPoints.size() - 2]);
		vertex.val[0] = m_finalPositions[m_finalPositions.size() - 2].val[0];
		vertex.val[1] = m_finalPositions[m_finalPositions.size() - 2].val[1];
		vertex.val[2] = m_finalPositions[m_finalPositions.size() - 2].val[2];
		m_finalPositions.push_back(vertex);

		texCoord.val[0] = m_finalTexCoords[m_finalTexCoords.size() - 2].val[0];
		texCoord.val[1] = m_finalTexCoords[m_finalTexCoords.size() - 2].val[1];
		m_finalTexCoords.push_back(texCoord);

		normal.val[0] = m_finalNormals[m_finalNormals.size() - 2].val[0];
		normal.val[1] = m_finalNormals[m_finalNormals.size() - 2].val[1];
		normal.val[2] = m_finalNormals[m_finalNormals.size() - 2].val[2];
		m_finalNormals.push_back(normal);

		//m_vertexPoints.push_back(VertexPoint(vp, vt, vn));
		vertex.val[0] = vp.x;
		vertex.val[1] = vp.y;
		vertex.val[2] = vp.z;
		m_finalPositions.push_back(vertex);

		texCoord.val[0] = vt.x;
		texCoord.val[1] = vt.y;
		m_finalTexCoords.push_back(texCoord);

		normal.val[0] = vn.x;
		normal.val[1] = vn.y;
		normal.val[2] = vn.z;
		m_finalNormals.push_back(normal);
	}
}


void ObjParser::ltrim(std::string& str, const std::locale& loc)
{
	std::string::size_type pos = 0;

	while (pos < str.size() && isspace(str[pos]))
		pos++;

	str.erase(0, pos);
}

void ObjParser::rtrim(std::string& str, const std::locale& loc)
{
	std::string::size_type pos = str.size();

	while (pos > 0 && isspace(str[pos - 1]))
		pos--;

	str.erase(pos);
}

void ObjParser::btrim(std::string& str, const std::locale& loc)
{
	ltrim(str, loc);
	rtrim(str, loc);
}

