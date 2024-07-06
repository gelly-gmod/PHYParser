#include "Solid.h"

#include "format/ivpcompactledgenode_t.h"
#include "format/ivpcompacttriangle_t.h"
#include "format/phyheader_t.h"

#include <stack>

namespace PHYParser::Format {
struct ivpcompacttriangle_t;
}

namespace PHYParser {
namespace Semantics {
Solid::Solid(const PHYData &data, size_t offset)
	: dataOffset(offset),
	  header(ParseHeader(data)),
	  surface(ParseSurface(data)) {
	ParseVertices(triangles);
}

auto Solid::GetVertexCount() const noexcept -> size_t {
	return triangles.size() * 3;
}

auto Solid::GetTriangleCount() const noexcept -> size_t {
	return triangles.size();
}

auto Solid::GetTriangles() const noexcept -> const std::vector<Triangle> & {
	return triangles;
}

auto Solid::GetMagic(char *magic) const noexcept -> void {
	surface->GetMagic(magic);
}

auto Solid::GetByteSize() const noexcept -> int {
	return header->size;
}

auto Solid::ParseHeader(
	const PHYData &data) const -> Format::compactsurfaceheader_t * {
	return ViewOffsetAsSection<Format::compactsurfaceheader_t>(
		data, dataOffset);
}

auto Solid::ParseSurface(
	const PHYData &data) const -> Format::ivpcompactsurface_t * {
	return ViewOffsetAsSection<Format::ivpcompactsurface_t>(
		data, dataOffset + sizeof(Format::compactsurfaceheader_t));
}

auto Solid::ParseVertices(
	std::vector<Triangle> &triangles) const -> void {
	auto *node = ViewOffsetFromSection<
		Format::ivpcompactledgenode_t>(
		surface, surface->offset_ledgetree_root);

	std::stack<Format::ivpcompactledgenode_t *> nodeStack;
	nodeStack.push(node);

	while (!nodeStack.empty()) {
		node = nodeStack.top();
		nodeStack.pop();

		if (node->IsTerminal()) {
			ConvertLedgeToVertices(node->FetchCompactNode(), triangles);
		} else {
			nodeStack.push(node->FetchLeftNode());
			nodeStack.push(node->FetchRightNode());
		}
	}
}

auto Solid::ConvertLedgeToVertices(Format::ivpcompactledge_t *node,
                                   std::vector<Triangle> &triangles) const ->
	void {
	const auto triangleCount = node->n_triangles;
	auto *vertexPointArray = ViewOffsetFromSection<Format::VectorAlign16>(
		node, node->c_point_offset);
	const auto *triangleArray = reinterpret_cast<const
		Format::ivpcompacttriangle_t *>
	(
		node + 1);

	for (int i = 0; i < triangleCount; i++) {
		Triangle triangle = {};
		const auto *triangleData = triangleArray + i;

		triangle.vertices[0] = vertexPointArray[triangleData->edges[0].
			start_point_index];
		triangle.vertices[1] = vertexPointArray[triangleData->edges[1].
			start_point_index];
		triangle.vertices[2] = vertexPointArray[triangleData->edges[2].
			start_point_index];

		triangles.push_back(triangle);
	}
}


} // Semantics
} // PHYParser