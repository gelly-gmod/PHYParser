#ifndef MOPPSURFACEHEADER_T_H
#define MOPPSURFACEHEADER_T_H

namespace PHYParser::Format {
struct moppsurfaceheader_t {
	int size;
	int vphysicsID; // Should be "VPHY"
	short version;
	short modelType;
	int moppSize; // this is where the mopp data begins to differ from the typical surface data
};
}

#endif //MOPPSURFACEHEADER_T_H
