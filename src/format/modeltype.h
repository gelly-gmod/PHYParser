#ifndef MODELTYPE_H
#define MODELTYPE_H

namespace PHYParser::Format {
enum class ModelType : short {
	IVPCompactSurface = 0x0,
	// used for literally anything that isn't a displacement
	IVPMOPP = 0x1, // typically used for old map displacement physics
};
}

#endif //MODELTYPE_H
