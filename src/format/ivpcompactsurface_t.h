#ifndef IVPCOMPACTSURFACE_T_H
#define IVPCOMPACTSURFACE_T_H
#include <cstring>

namespace PHYParser::Format {

struct ivpcompactsurface_t {
	float mass_center[3];
	float rotation_inertia[3];
	float upper_limit_radius;

	int max_deviation : 8;
	int byte_size : 24;

	int offset_ledgetree_root;
	int unused[3];

	/**
	 * Get the magic signature of this compact surface. Ideal for checking if this is a valid compact surface, as it should be "IVPS" (ipion virtual physics surface).
	 * @param magic The magic signature to write into, should be at least 5 bytes.
	 */
	auto GetMagic(char *magic) const -> void {
		std::memcpy(magic, &unused[2], 4);
		magic[4] = '\0';
	}
};

}

#endif //IVPCOMPACTSURFACE_T_H
