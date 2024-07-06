#ifndef COMPACTSURFACEHEADER_T_H
#define COMPACTSURFACEHEADER_T_H

#include "vector.h"

namespace PHYParser::Format {
struct compactsurfaceheader_t {
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
