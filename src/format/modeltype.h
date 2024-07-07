#ifndef MODELTYPE_H
#define MODELTYPE_H

namespace PHYParser::Format {
enum class ModelType : short {
	IVPCompactSurface = 0x0,
	// deprecated
	IVPMOPP = 0x1,
};
}

#endif //MODELTYPE_H
