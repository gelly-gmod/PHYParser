#include "Solid.h"

#include "exceptions/UnsupportedModelType.h"
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
	// atleast for now, we don't support IVPMOPP
	if (header->modelType == static_cast<short>(Format::ModelType::IVPMOPP)) {
		moppHeader = ParseMOPPHeader(data);
		mopp = ParseMOPPSurface(data);
		ParseMOPPVertices(triangles);
	} else if (header->modelType == static_cast<short>(Format::ModelType::IVPCompactSurface)) {
		ParseVertices(triangles);
	} else {
		throw UnsupportedModelType(Format::ModelType::Unknown);
	}
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
	return header->modelType == static_cast<short>(Format::ModelType::IVPMOPP) ? moppHeader->size : header->size;
}

auto Solid::ParseHeader(
	const PHYData &data) const -> Format::compactsurfaceheader_t * {
	return ViewOffsetAsSection<Format::compactsurfaceheader_t>(
		data, dataOffset);
}

auto Solid::ParseMOPPHeader(
	const PHYData &data) const -> Format::moppsurfaceheader_t * {
	return ViewOffsetAsSection<Format::moppsurfaceheader_t>(
		data, dataOffset);
}

auto Solid::ParseMOPPSurface(
	const PHYData &data) const -> Format::ivpcompactmopp_t * {
	return ViewOffsetAsSection<Format::ivpcompactmopp_t>(
		data, dataOffset + sizeof(Format::moppsurfaceheader_t));
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

auto Solid::ParseMOPPVertices(std::vector<Triangle> &triangles) const -> void {
	// MOPPs are optimized structures for dump and load meshes--typically for displacements. It's old, but
	// still present in iconic maps so we really can't ignore it, even if it's no longer used.

	// MOPPs dump ledges and don't use the typical tree structure, so we can just iterate through the ledges until we hit the point array.
	// Once we hit the point array, we can just iterate through the triangles.

	// unfortunately though, MOPPs are made for speed and therefore a lot of this is byte-level manipulation.
	char *currentLedge = reinterpret_cast<char *>(mopp) + mopp->offset_ledges + mopp->size_convex_hull;
	char *pointArray = currentLedge + reinterpret_cast<Format::ivpcompactledge_t *>(currentLedge)->c_point_offset;

	// run through the ledge dump until we hit the point array
	while (currentLedge < pointArray) {
		auto *ledge = reinterpret_cast<Format::ivpcompactledge_t *>(currentLedge);
		if (ledge->unknown != 0) {
			// According to some file format RE, there's a good chance that if this is not 0 we're in the wrong place.
			break;
		}

		ConvertLedgeToVertices(ledge, triangles);
		currentLedge += ledge->size_div_16 * 16;
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

	if (triangleCount <= 0) {
		return;
	}

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