#ifndef IVPCOMPACTMOPP_T_H
#define IVPCOMPACTMOPP_T_H

namespace PHYParser::Format {
struct ivpcompactmopp_t {
	float mass_center[3];
	float rotation_inertia[3];
	float upper_limit_radius;

	int max_deviation : 8;
	int byte_size : 24;

	int offset_ledgetree_root;
	int offset_ledges;
	int size_convex_hull;
	int dummy;
};
}

#endif //IVPCOMPACTMOPP_T_H
