#ifndef LUMP_T_H
#define LUMP_T_H

namespace PHYParser::BSP::Format {
struct lump_t {
	int fileofs;
	int filelen;
	int version;
	char fourCC[4];
};
}

#endif //LUMP_T_H
