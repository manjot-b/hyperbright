#ifndef OBJMODEL_H
#define OBJMODEL_H

/***************************************************************************
 *
 * ObjModel, a helper class for loading 3D data from OBJ files. This isn't
 *  designed to be a complete loader, so for instance the output isn't ideal
 *  for use with OpenGL. Still, it should be enough to get you started.
 *
 *  Author: Haysn Hornbeck
 */


//***** INCLUDES

#include <cmath>
using std::isnan;

#include <cstddef>
using std::size_t;


#include <array>
using std::array;

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <fstream>
using std::fstream;
using std::ifstream;

#include <memory>
using std::make_shared;
using std::shared_ptr;

#include <stdexcept>
using std::invalid_argument;
using std::out_of_range;

#include <string>
using std::getline;
using std::stof;
using std::stoi;
using std::string;

#include <vector>
using std::vector;


#include <boost/tokenizer.hpp>
using boost::char_separator;
using boost::tokenizer;



//***** DATA DECLARATIONS

typedef unsigned char uchar;
typedef unsigned int uint;

// build up what we need to create a triangle
typedef struct {

	float x;
	float y;
	float z;
	float w;

	} Position;

typedef struct {

	float x;
	float y;
	float z;

	} Normal;

typedef struct {

	float s;
	float t;
	float u;

	} TexCoord;

typedef struct {

	Position pos;
	Normal norm;
	TexCoord tex;

	} Vertex;

typedef struct {

	array<Vertex,3> vertex;

	} Triangle;


//***** CLASSES

// load up an OBJ file, and provide a friendly interface
class ObjModel {

	private:
	string filename;		// the name of the current object
	vector<Triangle> contents;	//  and its contents

	vector<Position> pos;		// caches used during the reading process
	vector<Normal> norm;
	vector<TexCoord> tex;
	vector<Triangle> tempContents;	//  including the future "contents"

	string buffer;			// a buffer to store lines in, and separators
	char_separator<char> whitespace;
	char_separator<char> faceSeparator;

	bool readVertex();		// a helper to read in vertex data
	bool readFace();		//  and the same for faces

	public:
	ObjModel();

	bool load( string filename );	// load up an OBJ file

					// basic setters/getters
	bool isLoaded() { return contents.size() > 0; }
	string getFilename() { return filename; }
	uint triangleCount() const { return contents.size(); };

					// retrieve some geometry
	Triangle& operator[]( size_t index );
	const Triangle& operator[]( size_t index ) const;

	};


#endif
