#ifndef SOLID_H
#define SOLID_H

#include "PHYData.h"
#include "format/ivpcompactledgenode_t.h"
#include "format/ivpcompactmopp_t.h"
#include "format/moppsurfaceheader_t.h"

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

	Solid(const PHYData &data, size_t offset);
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

	auto GetByteSize() const noexcept -> int;

private:
	size_t dataOffset;
	Format::compactsurfaceheader_t *header;
	Format::moppsurfaceheader_t *moppHeader;
	Format::ivpcompactsurface_t *surface;
	Format::ivpcompactmopp_t *mopp;

	std::vector<Triangle> triangles;

	auto ParseHeader(
		const PHYData &data) const -> Format::compactsurfaceheader_t *;
	auto ParseMOPPHeader(
		const PHYData &data) const -> Format::moppsurfaceheader_t *;
	auto ParseMOPPSurface(
		const PHYData &data) const -> Format::ivpcompactmopp_t *;
	auto ParseSurface(
		const PHYData &data) const -> Format::ivpcompactsurface_t *;
	auto ParseVertices(
		std::vector<Triangle> &triangles) const -> void;
	auto ParseTerminalNodes(std::vector<Triangle> &triangles,
	                        Format::ivpcompactledgenode_t *node) const -> void;
	auto ParseMOPPVertices(std::vector<Triangle> &triangles) const -> void;
	auto ConvertLedgeToVertices(Format::ivpcompactledge_t *node,
	                            std::vector<Triangle> &triangles) const ->
		void;
};

} // Semantics
} // PHYParser

#endif //SOLID_H
