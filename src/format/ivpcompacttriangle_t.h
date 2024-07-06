#ifndef IVPCOMPACTTRIANGLE_T_H
#define IVPCOMPACTTRIANGLE_T_H
#include "ivpcompactedge_t.h"

namespace PHYParser::Format {
struct ivpcompacttriangle_t {
	unsigned int tri_index : 12;
	unsigned int pierce_index : 12;
	unsigned int mat_index : 7;
	unsigned int is_virtual : 1;

	ivpcompactedge_t edges[3];
};
}

#endif //IVPCOMPACTTRIANGLE_T_H
