#ifndef PHY_H
#define PHY_H
#include "format/compactsurfaceheader_t.h"
#include "format/phyheader_t.h"
#include "semantics/Solid.h"

#include <memory>
#include <vector>

namespace PHYParser {
/**
 * Exposes access to the parsed structures in the given PHY file.
 */
class PHY {
public:
	/**
	 * Structure representing the input to the parser. You should fill this in with the data you want to parse. As this
	 * requires a unique pointer, the parser will take ownership of the data and free it when it is done, so don't attempt to hold onto it.
	 */
	struct ParserInput {
		std::unique_ptr<char[]> data;
		size_t size;

		ParserInput() = delete;
		ParserInput(const ParserInput &other) = delete;
		ParserInput &operator=(const ParserInput &other) = delete;

		ParserInput(std::unique_ptr<char[]> &&data,
		            size_t size) noexcept
			: data(std::move(data)), size(size) {
		}

		ParserInput(ParserInput &&other) noexcept
			: data(std::move(other.data)),
			  size(other.size) {
		}
	};

	PHY(ParserInput &&input) noexcept;
	~PHY() = default;

	[[nodiscard]] auto GetSolidCount() const noexcept -> int;
	[[nodiscard]] auto GetSolid(int index) const -> Semantics::Solid;

private:
	ParserInput input;
	Format::phyheader_t header;
	std::vector<Semantics::Solid> solids;
	PHYData data;

	auto GetPHYData() noexcept -> PHYData;
	auto ParseHeader(const PHYData &data) const -> Format::phyheader_t;
	auto ParseSolid(const PHYData &data,
	                size_t index) const -> Semantics::Solid;
};
}

#endif //PHY_H
