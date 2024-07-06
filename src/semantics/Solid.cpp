#include "Solid.h"

#include "format/phyheader_t.h"

namespace PHYParser {
namespace Semantics {
constexpr size_t SURFACE_SIZE = sizeof(Format::ivpcompactsurface_t) + sizeof(
	                                Format::compactsurfaceheader_t);

Solid::Solid(const PHYData &data, const size_t index)
	: dataOffset(sizeof(Format::phyheader_t) + index * SURFACE_SIZE),
	  header(ParseHeader(data)),
	  surface(ParseSurface(data)) {
}

auto Solid::GetVertexCount() const noexcept -> int {
	return header.size;
}

auto Solid::GetMagic(char *magic) const noexcept -> void {
	surface.GetMagic(magic);
}

auto Solid::ParseHeader(
	const PHYData &data) const -> Format::compactsurfaceheader_t {
	return *ViewOffsetAsSection<Format::compactsurfaceheader_t>(
		data, dataOffset);
}

auto Solid::ParseSurface(
	const PHYData &data) const -> Format::ivpcompactsurface_t {
	return *ViewOffsetAsSection<Format::ivpcompactsurface_t>(
		data, dataOffset + sizeof(Format::compactsurfaceheader_t));
}

} // Semantics
} // PHYParser