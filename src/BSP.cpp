#include "BSP.h"

#include "format/bsp/dphysmodel_t.h"

namespace PHYParser {
namespace BSP {
BSP::BSP(PHY::ParserInput &&input)
	: input(std::move(input)) {
	data = GetBSPData();
	header = ParseHeader(data);
	ParseModels(data, header.lumps[Format::LUMP_PHYSCOLLIDE], models);
}

auto BSP::GetModelCount() const noexcept -> int {
	return models.size();
}

auto BSP::GetModel(int index) const -> const Model & {
	if (index < 0 || index >= GetModelCount()) {
		throw std::out_of_range("Index out of range");
	}

	return models[index];
}

auto BSP::ParseHeader(
	const PHYData &data) const -> Format::dheader_t {
	return *ViewOffsetAsSection<Format::dheader_t>(data, 0);
}

auto BSP::ParseModels(const PHYData &data,
                      const Format::lump_t &physCollideLump,
                      std::vector<Model> &models) const -> void {
	// we can't parse models in the typical way since all solid data is stored contiguously, so we must iterate through *every* model either way
	auto *model = ViewOffsetAsSection<Format::dphysmodel_t>(
		data, physCollideLump.fileofs);

	while (model->modelIndex != -1) {
		Model parsedModel = {};
		parsedModel.index = model->modelIndex;
		parsedModel.solids.reserve(model->solidCount);

		auto solidOffset = reinterpret_cast<size_t>(model + 1);
		solidOffset -= reinterpret_cast<size_t>(data.get());
		// convert it to a relative offset (solids assume the origin is data.get())

		// model is a pointer to the actual file data, so we can simply cast it to a size_t
		for (int i = 0; i < model->solidCount; i++) {
			parsedModel.solids.emplace_back(data, solidOffset);
			solidOffset += parsedModel.solids.back().GetByteSize() + 4;
		}

		models.push_back(parsedModel);
		model = ViewOffsetFromSection<Format::dphysmodel_t>(
			model, sizeof(Format::dphysmodel_t) + model->dataSize + model->keyDataSize);
	}
}


auto BSP::GetBSPData() noexcept -> PHYData {
	return {std::move(input.data)};
}

} // BSP
} // PHYParser