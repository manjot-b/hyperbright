#include "CubeMap.h"

#include <stb_image.h>

#include <iostream>

namespace hyperbright {
namespace openGLHelper {
CubeMap::CubeMap(const char* filename)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);    // cubemap texture should not be flipped.
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cout << "Failed to load cube map: " << filename << std::endl;
    }

    auto faceData = extractFaces(data, width, height, nrChannels);
    int faceWidth = width / 4;
    int faceHeight = height / 3;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	// tells opengl not to assume any alignment of rows.
    for (uint32_t i = 0; i < faceData.size(); i++)
    {    
        switch (nrChannels)
        {
        case 1:
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RED, faceWidth, faceHeight, 0, GL_RED, GL_UNSIGNED_BYTE, faceData[i]);
            break;
        case 2:
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RG, faceWidth, faceHeight, 0, GL_RG, GL_UNSIGNED_BYTE, faceData[i]);
            break;
        case 3:
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, faceWidth, faceHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, faceData[i]);
            break;
        case 4:
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, faceWidth, faceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, faceData[i]);
            break;
        }
    }
    //glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    stbi_image_free(data);
    for (auto face : faceData)
        delete[] face;
}

std::array<unsigned char*, 6> CubeMap::extractFaces(const unsigned char* cubemap, int width, int height, int nrChannels)
{
    std::array<unsigned char*, 6> faces;

    // cubemaps follow this layout
    //       TOP
    // LEFT FRONT  RIGHT  BACK
    //     BOTTOM
    faceWidth = width / 4;
    faceHeight = height / 3;

    for (uint32_t i = 0; i < faces.size(); i++)
    {
        faces[i] = new unsigned char[faceWidth * faceHeight * nrChannels];
    }

    // Follow OpenGL texture targets ordering.
    extractFace(cubemap, width, height, nrChannels, faces[0], faceWidth, faceHeight, faceWidth * 2, faceHeight);   // right
    extractFace(cubemap, width, height, nrChannels, faces[1], faceWidth, faceHeight, 0, faceHeight);               // left
    extractFace(cubemap, width, height, nrChannels, faces[2], faceWidth, faceHeight, faceWidth, 0);                // top
    extractFace(cubemap, width, height, nrChannels, faces[3], faceWidth, faceHeight, faceWidth, faceHeight * 2);   // bottom
    extractFace(cubemap, width, height, nrChannels, faces[4], faceWidth, faceHeight, faceWidth, faceHeight);       // front
    extractFace(cubemap, width, height, nrChannels, faces[5], faceWidth, faceHeight, faceWidth * 3, faceHeight);   // back

    return faces;
}

/*
 Extracts a face from the cubemap.
 Parameters:
    cubemap: The cubemap with all the faces stiched together.
    face: The face to store the data into.
*/
void CubeMap::extractFace(const unsigned char* cubemap, uint32_t mapWidth, uint32_t mapHeight, uint32_t nrChannels,
    unsigned char* face, uint32_t faceWidth, uint32_t faceHeight,
    uint32_t startX, uint32_t startY)
{
    // TO-DO:
    // Clean this up by using std::memcpy
    for (uint32_t i = startY; i < startY + faceHeight; i++)
    {
        uint32_t cubemapStart = (mapWidth * i + startX) * nrChannels;
        uint32_t cubemapEnd = faceWidth * nrChannels;
        uint32_t faceStart = (faceWidth * (i - startY)) * nrChannels;
        std::memcpy(&(face[faceStart]), &(cubemap[cubemapStart]), cubemapEnd);
    }
}

unsigned int CubeMap::getId() const
{
    return id;
}

int CubeMap::getFaceWidth() const
{
    return faceWidth;
}

int CubeMap::getFaceHeight() const
{
    return faceHeight;
}

/**
*	Sets the current active CubeMap to the one specified in the parameter,
*	and then binds the CubeMap.
*/
void CubeMap::bind(GLenum texture) const
{
    glActiveTexture(texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}
}	// namespace openGLHelper
}	// namespace hyperbright