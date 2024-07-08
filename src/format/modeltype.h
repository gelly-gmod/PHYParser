#ifndef MODELTYPE_H
#define MODELTYPE_H

namespace PHYParser::Format {
enum class ModelType : short {
	IVPCompactSurface = 0x0,
	// deprecated
	IVPMOPP = 0x1,
	IVPBall = 0x2,
	IVPVirtual = 0x3,
	Unknown = 0x4,
};
}

#endif //MODELTYPE_H
