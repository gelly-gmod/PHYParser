#ifndef PHY_H
#define PHY_H
#include "format/compactsurfaceheader_t.h"
#include "format/phyheader_t.h"

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

	[[nodiscard]] auto GetSolidCount() const -> int;
	[[nodiscard]] auto GetSolidSurfaceSize(size_t solidIndex) const -> int;
	/**
	 * Useful for verifying that a solid is specified in new or old format.
	 */
	[[nodiscard]] auto GetSolidID(size_t solidIndex) const -> int;

private:
	ParserInput input;
	Format::phyheader_t header;
	std::vector<Format::compactsurfaceheader_t> surfaceHeaders;

	[[nodiscard]] auto ParseHeader() const -> Format::phyheader_t;
	[[nodiscard]] auto ParseSurfaceHeader(
		size_t solidIndex) const -> Format::compactsurfaceheader_t;
};
}

#endif //PHY_H
