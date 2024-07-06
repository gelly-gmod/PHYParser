#ifndef COMPACTSURFACEHEADER_T_H
#define COMPACTSURFACEHEADER_T_H

#include "vector.h"

namespace PHYParser::Format {
struct compactsurfaceheader_t {
	// some PHY implementations may split this into two structs, one 12 bytes and one 20, we just combine them
	int size;
	int vphysicsID; // Should be "VPHY"
	short version;
	short modelType;
	int surfaceSize;
	Vector dragAxisAreas;
	int axisMapSize;
};
}

#endif //COMPACTSURFACEHEADER_T_H
