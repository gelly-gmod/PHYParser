#ifndef SOLID_H
#define SOLID_H

#include "PHYData.h"
#include "format/ivpcompactledgenode_t.h"

#include <vector>
#include <format/compactsurfaceheader_t.h>
#include <format/ivpcompactsurface_t.h>

namespace PHYParser {
namespace Semantics {

class Solid {
public:
	struct Triangle {
		Format::VectorAlign16 vertices[3];
	};

	Solid(const PHYData &data, size_t index);
	~Solid() = default;

	[[nodiscard]] auto GetVertexCount() const noexcept -> size_t;
	[[nodiscard]] auto GetTriangleCount() const noexcept -> size_t;
	[[nodiscard]]
	auto GetTriangles() const noexcept -> const std::vector<Triangle> &;
	/**
	 * Get the magic number of the solid. (typically "IVPS")
	 * @param magic A pointer to a char array of at least 4 bytes.
	 */
	auto GetMagic(char *magic) const noexcept -> void;

private:
	size_t dataOffset;
	Format::compactsurfaceheader_t *header;
	Format::ivpcompactsurface_t *surface;
	std::vector<Triangle> triangles;

	auto ParseHeader(
		const PHYData &data) const -> Format::compactsurfaceheader_t *;
	auto ParseSurface(
		const PHYData &data) const -> Format::ivpcompactsurface_t *;
	auto ParseVertices(
		std::vector<Triangle> &triangles) const -> void;
	auto ConvertLedgeToVertices(Format::ivpcompactledge_t *node,
	                            std::vector<Triangle> &triangles) const ->
		void;
};

} // Semantics
} // PHYParser

#endif //SOLID_H
