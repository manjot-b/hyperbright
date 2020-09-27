#include "ObjModel.h"

// boring constructor
ObjModel::ObjModel() {

	whitespace	= char_separator<char>( " \t" );
	faceSeparator	= char_separator<char>( "/" );

	}

// read in some vertex data
bool ObjModel::readVertex() {

	// read in the type of vertex data
	tokenizer<char_separator<char>> tokens( buffer, whitespace );	

	auto tokenIt = tokens.begin();
	string type = *tokenIt;
	tokenIt++;

	// parse the actual coordinate data
	float coords[] = { 0, 0, 0, 0 };
	uchar numCoords = 0;
	while( tokenIt != tokens.end() ) {

		try { coords[numCoords] = stof( *tokenIt ); }
		catch ( invalid_argument& ia ) { return false; }	// exit on fail
		catch ( out_of_range& oor ) { return false; }

		if ( isnan(coords[numCoords]) )
			return false;					// NAN = bad

		tokenIt++;
		numCoords++;
		}

	if ( numCoords < 2 )	// we need a minimum number of coordinates
		return false;

	if ( type == "vn" )
		norm.push_back( { coords[0], coords[1], coords[2] } );
	
	else if ( type == "vt" )
		tex.push_back( { coords[0], coords[1], coords[2] } );
	else
		pos.push_back( { coords[0], coords[1], coords[2], coords[3] } );
	
	// otherwise, we're set
	return true;
	}

// read in face information
bool ObjModel::readFace() {

	vector<Vertex> points;	// cache these

	tokenizer<char_separator<char>> tokens( buffer, whitespace );	
	auto tokenIt = tokens.begin();
	tokenIt++;		// skip past the first bit

				// for each vertex
	for ( ; tokenIt != tokens.end(); tokenIt++ ) {

				// move the pointers into a vector
		tokenizer<char_separator<char>> pointers( *tokenIt, faceSeparator );
		vector<uint> datasets;
		for ( auto it = pointers.begin(); it != pointers.end(); it++ ) {

			if (*it == "") {	// blanks = dummy values
				datasets.push_back( 0xffffffff );
				continue;
				}

			try { datasets.push_back( stoi(*it)-1 ); }
			catch ( invalid_argument& ia ) { return false; }	// exit on fail
			catch ( out_of_range& oor ) { return false; }
			}

		if (datasets.size() < 3)
			return false;		// insufficient data

				// try to build up a vertex
		Vertex output;
		uint count = 0;
		if ( datasets[0] < pos.size() ) {
			output.pos = pos[ datasets[0] ];
			count++;
			}
		if ( datasets[1] < tex.size() ) {
			output.tex = tex[ datasets[1] ];
			count++;
			}
		if ( datasets[2] < norm.size() ) {
			output.norm = norm[ datasets[2] ];
			count++;
			}

		if ( count > 0 )
			points.push_back( output );	// save it, assuming we read something in
		}

	if ( points.size() < 3 )
		return false;	// not enough to build a triangle?

				// auto-triangularize
	for (uint it = 1; it < points.size() - 1; it++) {

		Triangle tri;
		tri.vertex[0] = points[0];
		tri.vertex[1] = points[it];
		tri.vertex[2] = points[it+1];

		tempContents.push_back( tri );
		}

	return true;

	}

// load up the OBJ file
bool ObjModel::load( string f ) {

	// null check, of course
	if ( f.empty() ) {

		cerr << "ERROR: ObjModel: Given a blank filename." << endl;
		return false;

		}

	// try to open the file
	ifstream file;
	file.open( f, fstream::in );

	// didn't work? fail!
	if ( file.fail() ) {

		cerr << "ERROR: ObjModel: Couldn't load \"" << f << "\"." << endl;
		return false;

		}

	buffer.clear();		// store individual lines
	pos.clear();		// clear out the caches
	norm.clear();
	tex.clear();
	tempContents.clear();	// cache these, so we don't clobber on error

	while( file.good() ) {	// while we have data, read in a line

		getline( file, buffer );

				// clear out any comments
		size_t commentPos = buffer.find("#");
		if ( commentPos == 0 )
			continue;	// the entire line was a comment? trash it!
		else if ( commentPos != string::npos )
			buffer = buffer.substr( 0, commentPos );

		switch( buffer[0] ) {	// first char is a major clue to what we're reading

			case 'v': 	// vertex data of some sort

				if ( !readVertex() )	// obey any quit signal
					return false;
				break;

			case 'f':	// face data of some sort

				if ( !readFace() )
					return false;
				break;

			}
		}

	// if we made it this far, commit what we have and continue
	contents = tempContents;
	filename = f;

	return true;
	}
		

// allow access to the triangles we've read in
Triangle& ObjModel::operator[]( size_t index ) {

	if ( index >= contents.size() )
		throw new out_of_range( "ObjModel: Triangle out of range" );
	else
		return contents[index];

	}

const Triangle& ObjModel::operator[]( size_t index ) const {

	if ( index >= contents.size() )
		throw new out_of_range( "ObjModel: Triangle out of range" );
	else
		return contents[index];

	}
