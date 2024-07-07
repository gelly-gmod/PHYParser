#include <raylib.h>
#include "PHY.h"
#include "open-phy-file-dialog.h"
#include "exceptions/UnsupportedModelType.h"

#include <cstdio>
#include <optional>
#include <stdexcept>

constexpr int screenWidth = 800;
constexpr int screenHeight = 600;

auto LoadPHYFile(const char *filepath) -> PHYParser::PHY {
	FILE *phyFile;
	fopen_s(&phyFile, filepath, "rb");

	if (!phyFile) {
		throw std::runtime_error("Failed to open PHY file");
	}

	fseek(phyFile, 0, SEEK_END);
	size_t fileSize = ftell(phyFile);
	fseek(phyFile, 0, SEEK_SET);

	auto data = std::make_unique<char[]>(fileSize);

	fread(data.get(), 1, fileSize, phyFile);
	fclose(phyFile);

	return {{std::move(data), fileSize}};
}

auto InitializeRaylib() -> void {
	InitWindow(screenWidth, screenHeight, "PHYParser Test");
	SetTargetFPS(144);
}

struct RenderingContext {
	Camera3D camera;
	std::vector<PHYParser::Semantics::Solid> solids;
	const char *phyFileName;
};

auto RenderFrame(const RenderingContext &ctx) -> void {
	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode3D(ctx.camera);
	DrawGrid(10, 1.0f);

	for (const auto &solid : ctx.solids) {
		const auto &triangles = solid.GetTriangles();
		for (const auto &triangle : triangles) {
			Vector3 v0 = {triangle.vertices[0].x, triangle.vertices[0].y,
			              triangle.vertices[0].z};
			Vector3 v1 = {triangle.vertices[1].x, triangle.vertices[1].y,
			              triangle.vertices[1].z};
			Vector3 v2 = {triangle.vertices[2].x, triangle.vertices[2].y,
			              triangle.vertices[2].z};

			// random color
			Color color;
			color.r = GetRandomValue(0, 255);
			color.g = GetRandomValue(0, 255);
			color.b = GetRandomValue(0, 255);
			color.a = 255;
			DrawTriangle3D(v0, v1, v2, color);
		}
	}
	EndMode3D();

	DrawText("PHYParser Test", 10, 10, 20, WHITE);
	DrawText(ctx.phyFileName, 10, 40, 20, WHITE);

	auto solidInfoY = 70;
	auto solidCount = 0;
	for (const auto &solid : ctx.solids) {
		char solidInfo[1024] = {0};
		snprintf(solidInfo, sizeof(solidInfo),
		         "Solid %d: %llu vertices, %llu triangles",
		         solidCount++, solid.GetVertexCount(),
		         solid.GetTriangleCount());

		DrawText(solidInfo, 10, solidInfoY, 20, WHITE);
		solidInfoY += 30;
	}

	EndDrawing();
}

auto InitializeRendering() -> void {
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
	SetTraceLogLevel(LOG_ALL);
	SetTraceLogCallback([](int _, const char *text, va_list args) {
		// format text
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), text, args);

		printf("[raylib] %s\n", buffer);
	});

	InitializeRaylib();
}

auto CreateDefaultRenderingContext(
	std::vector<PHYParser::Semantics::Solid> solids) -> RenderingContext {
	return {
		.camera = {
			.position = {10.0f, -10.0f, 10.0f},
			.target = {0.0f, 0.0f, 0.0f},
			.up = {0.0f, -1.0f, 0.0f},
			.fovy = 45.0f,
			.projection = CAMERA_PERSPECTIVE
		},
		.solids = std::move(solids),
		.phyFileName = nullptr
	};
}

auto main() -> int {
	InitializeRendering();
	char inputFile[1024] = {0};

	if (const auto result = util::OpenPhyFileDialog(
		inputFile, sizeof(inputFile)); !result) {
		printf("Failed to open file dialog\n");
		return -1;
	}

	printf("Parsing PHY file: %s\n", inputFile);
	try {
		const auto phy = LoadPHYFile(inputFile);
		printf("Successfully loaded PHY file\n");
		printf("Solid count: %d\n", phy.GetSolidCount());

		std::vector<PHYParser::Semantics::Solid> solids;
		for (int i = 0; i < phy.GetSolidCount(); ++i) {
			auto solid = phy.GetSolid(i);
			char magic[5] = {0};
			solid.GetMagic(magic);
			printf("Solid %d: %s\n", i, magic);
			printf("Vertex count: %llu\n", solid.GetVertexCount());
			printf("Triangle count: %llu\n", solid.GetTriangleCount());
			solids.push_back(std::move(solid));
			printf("Added to rendering context!\n");
		}

		auto ctx = CreateDefaultRenderingContext(std::move(solids));
		ctx.phyFileName = inputFile;
		while (!WindowShouldClose()) {
			// update camera
			UpdateCamera(&ctx.camera, CAMERA_ORBITAL);
			RenderFrame(ctx);
		}
	} catch (const PHYParser::UnsupportedModelType &e) {
		printf("Unsupported model type: %d\nPHYParser error: '%s'\n",
		       e.GetModelType(), e.what());
		CloseWindow();
		return -1;
	} catch (const std::exception &e) {
		printf("Generic error: %s\n", e.what());
		CloseWindow();
		return -1;
	}

	CloseWindow();
	return 0;
}