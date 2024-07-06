#ifndef IVPCOMPACTLEDGE_T_H
#define IVPCOMPACTLEDGE_T_H

namespace PHYParser::Format {
struct ivpcompactledge_t {
	int c_point_offset;

	union {
		int ledgetree_node_offset;
		int source_userdata; // Unknown what this actually is
	};

	unsigned int has_children_flag : 2;
	int is_compact_flag : 2;
	unsigned int dummy : 4;
	unsigned int size_div_16 : 24;

	short n_triangles;
	short unknown;
};
}

#endif //IVPCOMPACTLEDGE_T_H
