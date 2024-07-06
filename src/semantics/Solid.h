#ifndef SOLID_H
#define SOLID_H

#include "PHYData.h"

#include <format/compactsurfaceheader_t.h>
#include <format/ivpcompactsurface_t.h>

namespace PHYParser {
namespace Semantics {

class Solid {
public:
	Solid(const PHYData &data, size_t index);
	~Solid() = default;

	[[nodiscard]] auto GetVertexCount() const noexcept -> int;
	/**
	 * Get the magic number of the solid. (typically "IVPS")
	 * @param magic A pointer to a char array of at least 4 bytes.
	 */
	auto GetMagic(char *magic) const noexcept -> void;

private:
	size_t dataOffset;
	Format::compactsurfaceheader_t header;
	Format::ivpcompactsurface_t surface;

	auto ParseHeader(
		const PHYData &data) const -> Format::compactsurfaceheader_t;
	auto ParseSurface(const PHYData &data) const -> Format::ivpcompactsurface_t;
};

} // Semantics
} // PHYParser

#endif //SOLID_H
