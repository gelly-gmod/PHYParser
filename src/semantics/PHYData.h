#ifndef PHYDATA_H
#define PHYDATA_H
#include <memory>

namespace PHYParser {
using PHYData = std::shared_ptr<char[]>;

template <typename T>
concept PHYDataSection = requires(T t)
{
	// ensure it's a POD
	std::is_pod_v<T>;
};

template <typename PHYDataSection>
PHYDataSection *ViewOffsetAsSection(const PHYData &data, size_t offset) {
	return reinterpret_cast<PHYDataSection *>(data.get() + offset);
}
}

#endif //PHYDATA_H
