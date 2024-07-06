#include "PHY.h"

#include <stdexcept>

namespace PHYParser {
PHY::PHY(ParserInput &&input) noexcept
	: input(std::move(input)) {
	data = GetPHYData();
	header = ParseHeader(data);
	for (size_t i = 0; i < GetSolidCount(); ++i) {
		solids.push_back(ParseSolid(data, i));
	}
}

auto PHY::GetSolidCount() const noexcept -> int {
	return header.solidCount;
}

auto PHY::GetSolid(int index) const -> Semantics::Solid {
	if (index < 0 || index >= GetSolidCount()) {
		throw std::out_of_range("Index out of range");
	}

	return solids[index];
}

auto PHY::GetPHYData() noexcept -> PHYData {
	return {std::move(input.data)};
}

auto PHY::ParseHeader(const PHYData &data) const -> Format::phyheader_t {
	return *ViewOffsetAsSection<Format::phyheader_t>(data, 0);
}

auto PHY::ParseSolid(const PHYData &data,
                     size_t index) const -> Semantics::Solid {
	return {data, index};
}
}