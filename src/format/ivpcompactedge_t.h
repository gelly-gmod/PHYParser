#ifndef IVPCOMPACTEDGE_T_H
#define IVPCOMPACTEDGE_T_H

namespace PHYParser::Format {
struct ivpcompactedge_t {
	unsigned int start_point_index : 16;
	unsigned int opposite_index : 15;
	unsigned int is_virtual : 1;
};
}

#endif //IVPCOMPACTEDGE_T_H
