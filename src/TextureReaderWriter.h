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

#ifndef TEXTURE_READER_WRITER_H
#define TEXTURE_READER_WRITER_H

// Libraries
#include "Texture2D.h"
#include <fstream>
#include <iostream>

namespace openglframework {

// Class TextureReaderWriter
// This class is used to load or write a texture image in different picture format.
// It currently allows to read and write the following formats : .tga
class TextureReaderWriter {

    private :

        // -------------------- Methods -------------------- //

        // Constructor (private because we do not want instances of this class)
        TextureReaderWriter();

        // Read a TGA picture
        static bool readTGAPicture(const std::string& filename,
                                   Texture2D& textureToCreate) throw(std::runtime_error);

        // Write a TGA picture
        static void writeTGAPicture(const std::string& filename,
                                    const Texture2D& texture) throw(std::runtime_error);

    public :

        // -------------------- Methods -------------------- //

        // Load a texture from a file
        static bool loadTextureFromFile(const std::string& filename,
                                        Texture2D& textureToCreate)
                                        throw(std::runtime_error, std::invalid_argument);

        // Write a texture to a file
        static void writeTextureToFile(const std::string& filename,
                                       const Texture2D& texture)
                                       throw(std::runtime_error, std::invalid_argument);
};

}

#endif