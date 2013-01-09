/********************************************************************************
* OpenGL-Framework                                                              *
* Copyright (c) 2013 Daniel Chappuis                                            *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

// Libraries
#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <locale>
#include <cctype>
#include <map>
#include <string>
#include <algorithm>

// Namespaces
using namespace openglframework;
using namespace std;

// Constructor
Mesh::Mesh() {

}

// Destructor
Mesh::~Mesh() {

    // Destroy the mesh
    destroy();
}

// Destroy the mesh
void Mesh::destroy() {

    mVertices.clear();
    mNormals.clear();
    mTangents.clear();
    mIndices.clear();
    mColors.clear();
    mUVs.clear();
}

// Load a mesh from an OBJ file
bool Mesh::loadFromFile(std::string filename) {

    // Parse the OBJ file
    return parseOBJFile(filename);
}

// Parse an OBJ file with a triangular or quad mesh
bool Mesh::parseOBJFile(const std::string filename) {

    std::ifstream fileOBJ(filename.c_str());
    assert(fileOBJ.is_open());
    if(!fileOBJ.is_open()) return false;

    std::string buffer;
    string line, tmp;
    int id1, id2, id3, id4;
    int nId1, nId2, nId3, nId4;
    int tId1, tId2, tId3, tId4;
    float v1, v2, v3;
    int found1,found2;
    std::vector<bool> isQuad;
    std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<Vector2> uvs;
    std::vector<uint> verticesIndices;
    std::vector<uint> normalsIndices;
    std::vector<uint> uvsIndices;

    // ---------- Collect the data from the file ---------- //

    // For each line of the file
    while(std::getline(fileOBJ, buffer)) {

        std::istringstream lineStream(buffer);
        std::string word;
        lineStream >> word;
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        if(word == "usemtl") {  // Material definition

            // Loading of MTL file is not implemented

        }
        else if(word == "v") {  // Vertex position
            sscanf_s(buffer.c_str(), "%*s %f %f %f", &v1, &v2, &v3);
            vertices.push_back(Vector3(v1, v2, v3));
        }
        else if(word == "vt") { // Vertex texture coordinate
            sscanf_s(buffer.c_str(), "%*s %f %f", &v1, &v2);
            uvs.push_back(Vector2(v1,v2));
        }
        else if(word == "vn") { // Vertex normal
            sscanf_s(buffer.c_str(), "%*s %f %f %f", &v1, &v2, &v3);
            normals.push_back(Vector3(v1 ,v2, v3));
        }
        else if (word == "f") { // Face
            line = buffer;
            found1 = (int)line.find("/");
            bool isFaceQuad = false;

            // If the face definition only contains vertices (and not texture coordinates)
            if(found1 == string::npos) {
                int nbVertices = sscanf_s(buffer.c_str(), "%*s %d %d %d %d", &id1, &id2, &id3, &id4);
                if (nbVertices == 4) isFaceQuad = true;
            }
            else {  // If the face definition contains vertices and texture coordinates

                //get the part of the string until the second index
                tmp = line.substr(found1+1);
                found2 = (int)tmp.find(" ");
                tmp = tmp.substr(0,found2);
                found2 = (int)tmp.find("/");

                // If the face definition is of the form "f vert1/textcoord1 vert2/textcoord2 ..."
                if(found2 == string::npos) {
                    int n = sscanf_s(buffer.c_str(), "%*s %d/%d %d/%d %d/%d %d/%d", &id1, &tId1, &id2, &tId2, &id3, &tId3, &id4, &tId4);
                    if (n == 8) isFaceQuad = true;
                    uvsIndices.push_back(tId1-1);
                    uvsIndices.push_back(tId2-1);
                    uvsIndices.push_back(tId3-1);
                    if (isFaceQuad) uvsIndices.push_back(tId4-1);
                }
                else {
                    tmp = line.substr(found1+1);
                    found2 = (int)tmp.find("/");

                    // If the face definition is of the form "f vert1/normal1 vert2/normal2 ..."
                    if(found2 == 0) {
                        int n = sscanf_s(buffer.c_str(), "%*s %d//%d %d//%d %d//%d %d//%d", &id1, &nId1, &id2, &nId2, &id3, &nId3, &id4, &nId4);
                        if (n == 8) isFaceQuad = true;
                    }
                    // If the face definition is of the form "f vert1/textcoord1/normal1 ..."
                    else {
                        int n = sscanf_s(buffer.c_str(), "%*s %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &id1, &tId1, &nId1, &id2, &tId2, &nId2, &id3, &tId3, &nId3, &id4, &tId4, &nId4);
                        if (n == 12) isFaceQuad = true;
                        uvsIndices.push_back(tId1-1);
                        uvsIndices.push_back(tId2-1);
                        uvsIndices.push_back(tId3-1);
                        if (isFaceQuad) uvsIndices.push_back(tId4-1);
                    }
                    normalsIndices.push_back(nId1-1);
                    normalsIndices.push_back(nId2-1);
                    normalsIndices.push_back(nId3-1);
                    if (isFaceQuad) normalsIndices.push_back(nId4-1);
                }
            }
            verticesIndices.push_back(id1-1);
            verticesIndices.push_back(id2-1);
            verticesIndices.push_back(id3-1);
            if (isFaceQuad) verticesIndices.push_back((id4-1));
            isQuad.push_back(isFaceQuad);
        }
    }

    assert(!verticesIndices.empty());
    assert(normalsIndices.empty() || normalsIndices.size() == verticesIndices.size());
    assert(uvsIndices.empty() || uvsIndices.size() == verticesIndices.size());
    fileOBJ.close();

    // ---------- Merge the data that we have collected from the file ---------- //

    // Destroy the current mesh
    destroy();

    std::vector<unsigned int> indicesMesh(verticesIndices.size());

    typedef std::map<VertexMergingData, unsigned int, VertexMergingDataComparison> VertexMerginMap;
    VertexMerginMap vertexMergingMap;
    VertexMerginMap::iterator it;
    for(unsigned int i = 0; i < verticesIndices.size(); ++i)
    {
        VertexMergingData vmd;
        vmd.indexPosition = verticesIndices[i];
        if(!normalsIndices.empty()) vmd.indexNormal = normalsIndices[i];
        if(!uvsIndices.empty()) vmd.indexUV = uvsIndices[i];
        it = vertexMergingMap.find(vmd);
        if(it != vertexMergingMap.end())
        {
            indicesMesh[i] = it->second;
        }
        else
        {
            indicesMesh[i] = (unsigned int)mVertices.size();
            vertexMergingMap[vmd] = (unsigned int)mVertices.size();
            mVertices.push_back(vertices[verticesIndices[i]]);
            if(!normalsIndices.empty()) mNormals.push_back(normals[normalsIndices[i]]);
            if(!uvsIndices.empty()) mUVs.push_back(uvs[uvsIndices[i]]);
        }
    }
    assert(!mVertices.empty());
    assert(mNormals.empty() || mNormals.size() == mVertices.size());
    assert(mUVs.empty() || mUVs.size() == mVertices.size());

    // We cannot load mesh with several parts for the moment
    uint meshPart = 0;

    // Fill in the vertex indices
    // We also triangulate each quad face
    mIndices.push_back(std::vector<uint>());
    for(int i = 0, j = 0; i < indicesMesh.size(); j++) {

        // Get the current vertex IDs
        uint i1 = indicesMesh[i];
        uint i2 = indicesMesh[i+1];
        uint i3 = indicesMesh[i+2];

        // If the current vertex not in a quad (it is part of a triangle)
        if (!isQuad[j]) {


            mIndices[meshPart].push_back(i1);
            mIndices[meshPart].push_back(i2);
            mIndices[meshPart].push_back(i3);

            i+=3;
        }
        else {  // If the current vertex is in a quad

            Vector3 v1 = mVertices[i1];
            Vector3 v2 = mVertices[i2];
            Vector3 v3 = mVertices[i3];
            uint i4 = indicesMesh[i+3];
            Vector3 v4 = mVertices[i4];

            Vector3 v13 = v3-v1;
            Vector3 v12 = v2-v1;
            Vector3 v14 = v4-v1;

            float a1 = v13.dot(v12);
            float a2 = v13.dot(v14);
            if((a1 >= 0 && a2 <= 0) || (a1 <= 0 && a2 >= 0)) {
                mIndices[meshPart].push_back(i1);
                mIndices[meshPart].push_back(i2);
                mIndices[meshPart].push_back(i3);
                mIndices[meshPart].push_back(i1);
                mIndices[meshPart].push_back(i3);
                mIndices[meshPart].push_back(i4);
            }
            else {
                mIndices[meshPart].push_back(i1);
                mIndices[meshPart].push_back(i2);
                mIndices[meshPart].push_back(i4);
                mIndices[meshPart].push_back(i2);
                mIndices[meshPart].push_back(i3);
                mIndices[meshPart].push_back(i4);
            }
            i+=4;
        }


    }

    return true;
}

// Compute the normals of the mesh
void Mesh::calculateNormals() {

    mNormals = vector<Vector3>(getNbVertices(), Vector3(0, 0, 0));

    // For each triangular face
    for (uint i=0; i<getNbFaces(); i++) {

        // Get the three vertices index of the current face
        uint v1 = getVertexIndexInFace(i, 0);
        uint v2 = getVertexIndexInFace(i, 1);
        uint v3 = getVertexIndexInFace(i, 2);

        assert(v1 < getNbVertices());
        assert(v2 < getNbVertices());
        assert(v3 < getNbVertices());

        // Compute the normal of the face
        Vector3 p = getVertex(v1);
        Vector3 q = getVertex(v2);
        Vector3 r = getVertex(v3);
        Vector3 normal = (q-p).cross(r-p).normalize();

        // Add the face surface normal to the sum of normals at
        // each vertex of the face
        mNormals[v1] += normal;
        mNormals[v2] += normal;
        mNormals[v3] += normal;
    }

    // Normalize the normal at each vertex
    for (uint i=0; i<getNbVertices(); i++) {
        assert(mNormals[i].length() > 0);
        mNormals[i] = mNormals[i].normalize();
    }
}

// Compute the tangents of the mesh
void Mesh::calculateTangents() {

    mTangents = std::vector<Vector3>(getNbVertices(), Vector3(0, 0, 0));

    // For each face
    for (uint i=0; i<getNbFaces(); i++) {

        // Get the three vertices index of the face
        uint v1 = getVertexIndexInFace(i, 0);
        uint v2 = getVertexIndexInFace(i, 1);
        uint v3 = getVertexIndexInFace(i, 2);

        assert(v1 < getNbVertices());
        assert(v2 < getNbVertices());
        assert(v3 < getNbVertices());

        // Get the vertices positions
        Vector3 p = getVertex(v1);
        Vector3 q = getVertex(v2);
        Vector3 r = getVertex(v3);

        // Get the texture coordinates of each vertex
        Vector2 uvP = getUV(v1);
        Vector2 uvQ = getUV(v2);
        Vector2 uvR = getUV(v3);

        // Get the three edges
        Vector3 edge1 = q - p;
        Vector3 edge2 = r - p;
        Vector2 edge1UV = uvQ - uvP;
        Vector2 edge2UV = uvR - uvP;

        float cp = edge1UV.y * edge2UV.x - edge1UV.x * edge2UV.y;

        // Compute the tangent
        if (cp != 0.0f) {
            float factor = 1.0f / cp;
            Vector3 tangent = (edge1 * -edge2UV.y + edge2 * edge1UV.y) * factor;
            tangent.normalize();
            mTangents[v1] = tangent;
            mTangents[v2] = tangent;
            mTangents[v3] = tangent;
        }
    }
}

// Calculate the bounding box of the mesh
void Mesh::calculateBoundingBox(Vector3& min, Vector3& max) const {

    // If the mesh contains vertices
    if (!mVertices.empty())  {

        min = mVertices[0];
        max = mVertices[0];

        std::vector<Vector3>::const_iterator  it(mVertices.begin());

        // For each vertex of the mesh
        for (; it != mVertices.end(); ++it) {

            if( (*it).x < min.x ) min.x = (*it).x;
            else if ( (*it).x > max.x ) max.x = (*it).x;

            if( (*it).y < min.y ) min.y = (*it).y;
            else if ( (*it).y > max.y ) max.y = (*it).y;

            if( (*it).z < min.z ) min.z = (*it).z;
            else if ( (*it).z > max.z ) max.z = (*it).z;
        }
    }
    else {
        std::cerr << "Error : Impossible to calculate the bounding box of the mesh because there" <<
                    "are no vertices !" << std::endl;
        assert(false);
    }
}

// Scale of vertices of the mesh using a given factor
void Mesh::scaleVertices(float factor) {

    // For each vertex
    for (uint i=0; i<getNbVertices(); i++) {
        mVertices.at(i) *= factor;
    }
}