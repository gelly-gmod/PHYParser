#ifndef DPHYSDISP_T_H
#define DPHYSDISP_T_H

namespace PHYParser::BSP::Format {
struct dphysdisp_t {
	unsigned short numDisplacements;

	unsigned short *FetchDataSize() {
		return reinterpret_cast<unsigned short *>(this + 1);
	}
};
}

#endif //DPHYSDISP_T_H
