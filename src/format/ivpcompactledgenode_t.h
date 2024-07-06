#ifndef IVPCOMPACTLEDGENODE_T_H
#define IVPCOMPACTLEDGENODE_T_H
#include "ivpcompactledge_t.h"

namespace PHYParser::Format {
struct ivpcompactledgenode_t {
	int offset_right_node;
	int offset_compact_node;
	// THIS IS *NOT* ANOTHER NODE, THE LEFT NODE IS offset_right_node + 1
	float center[3];
	float radius;
	unsigned char box_sizes[3];
	unsigned char unknown_0;

	ivpcompactledgenode_t *FetchLeftNode() {
		return this + 1;
	}

	ivpcompactledgenode_t *FetchRightNode() {
		return reinterpret_cast<ivpcompactledgenode_t *>(
			reinterpret_cast<char *>(this) + offset_right_node);
	}

	[[nodiscard]] bool IsTerminal() const {
		return offset_right_node == 0;
	}

	ivpcompactledge_t *FetchCompactNode() {
		return reinterpret_cast<ivpcompactledge_t *>(
			reinterpret_cast<char *>(this) + offset_compact_node);
	}
};
}

#endif //IVPCOMPACTLEDGENODE_T_H
