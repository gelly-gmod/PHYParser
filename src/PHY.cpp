#include "PHY.h"

#include <stdexcept>

namespace PHYParser {
PHY::PHY(ParserInput &&input) noexcept
	: input(std::move(input)), header(ParseHeader()) {
	for (auto i = 0; i < header.solidCount; i++) {
		surfaceHeaders.push_back(ParseSurfaceHeader(i));
	}
}

auto PHY::GetSolidCount() const -> int {
	return header.solidCount;
}

auto PHY::GetSolidSurfaceSize(
	size_t solidIndex) const -> int {
	return surfaceHeaders.at(solidIndex).surfaceSize;
}

auto PHY::GetSolidID(
	size_t solidIndex) const -> int {
	return surfaceHeaders.at(solidIndex).vphysicsID;
}

auto PHY::ParseHeader() const -> Format::phyheader_t {
	const auto headerSize = sizeof(Format::phyheader_t);
	if (input.size < headerSize) {
		throw std::runtime_error("PHY file is too small to contain a header");
	}

	const auto header = *reinterpret_cast<const Format::phyheader_t *>(input.
		data.get());

	return header;
}

auto PHY::ParseSurfaceHeader(
	size_t solidIndex) const -> Format::compactsurfaceheader_t {
	const auto surfaceHeaderSize = sizeof(Format::compactsurfaceheader_t);
	const auto offset = sizeof(Format::phyheader_t) + solidIndex *
	                    surfaceHeaderSize;
	if (input.size < offset + surfaceHeaderSize) {
		throw std::runtime_error(
			"PHY file is too small to contain the requested surface header");
	}

	const auto surfaceHeader = *reinterpret_cast<const
		Format::compactsurfaceheader_t *>(input.
		                                  data.get() + offset);

	return surfaceHeader;
}
}