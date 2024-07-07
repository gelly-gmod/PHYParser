#ifndef DHEADER_T_H
#define DHEADER_T_H
#include "lump_t.h"

namespace PHYParser::BSP::Format {
struct dheader_t {
	int ident;
	int version;
	lump_t lumps[64];
	int mapRevision;
};
}

#endif //DHEADER_T_H
