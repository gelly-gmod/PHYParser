#ifndef LUMP_T_H
#define LUMP_T_H

namespace PHYParser::BSP::Format {
enum LumpType {
	/**
	 * Very important to note: up until 2009, all displacement physics data was stored in LUMP_PHYSCOLLIDE. Why it was suddenly moved to its own lump in 2009 is unknown.
	 * From an RE analysis, maps don't have any sort of tell-tale sign other than that LUMP_2009_PHYSDISP's `filelen` is always 0 in pre-2009 maps.
	 */
	LUMP_2009_PHYSDISP = 28,
	LUMP_PHYSCOLLIDE = 29,
};

struct lump_t {
	int fileofs;
	int filelen;
	int version;
	char fourCC[4];
};
}

#endif //LUMP_T_H
