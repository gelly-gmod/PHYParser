#ifndef UNSUPPORTEDMODELTYPE_H
#define UNSUPPORTEDMODELTYPE_H
#include "format/modeltype.h"

#include <stdexcept>

namespace PHYParser {
class UnsupportedModelType : public std::exception {
public:
	explicit UnsupportedModelType(Format::ModelType modelType) noexcept
		: modelType(modelType) {
	}

	[[nodiscard]] auto what() const noexcept -> const char * override {
		return modelType == Format::ModelType::IVPCompactSurface
			       ? "IVPCompactSurface is not supported"
			       : "IVPMOPP is not supported";
	}

	[[nodiscard]] auto GetModelType() const noexcept -> Format::ModelType {
		return modelType;
	}

private:
	Format::ModelType modelType;
};
}
#endif //UNSUPPORTEDMODELTYPE_H
