#include <raylib.h>
#include "PHY.h"

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
};

auto RenderFrame(const RenderingContext &ctx) -> void {
	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode3D(ctx.camera);
	DrawGrid(10, 1.0f);
	EndMode3D();

	DrawText("PHYParser Test", 10, 10, 20, WHITE);
	EndDrawing();
}

auto InitializeRendering() -> void {
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetTraceLogLevel(LOG_ALL);
	SetTraceLogCallback([](int logLevel, const char *text, va_list args) {
		// format text
		char buffer[1024];
		vsnprintf(buffer, sizeof(buffer), text, args);

		printf("[raylib] %s\n", buffer);
	});

	InitializeRaylib();
}

auto CreateDefaultRenderingContext() -> RenderingContext {
	return {
		.camera = {
			.position = {10.0f, 10.0f, 10.0f},
			.target = {0.0f, 0.0f, 0.0f},
			.up = {0.0f, 1.0f, 0.0f},
			.fovy = 45.0f,
			.projection = CAMERA_PERSPECTIVE
		},
	};
}

auto main(int argc, char **argv) -> int {
	if (argc < 2) {
		printf("Usage: %s <input PHY file>\n", argv[0]);
		return -1;
	}

	InitializeRendering();

	char *inputFile = argv[1];
	printf("Parsing PHY file: %s\n", inputFile);
	try {
		const auto phy = LoadPHYFile(inputFile);
		printf("Successfully loaded PHY file\n");
		printf("Solid count: %d\n", phy.GetSolidCount());

		for (auto i = 0; i < phy.GetSolidCount(); i++) {
			printf("Solid %d surface size: %d\n", i,
			       phy.GetSolidSurfaceSize(i));

			char id[5];
			const auto vphysicsID = phy.GetSolidID(i);
			memcpy(id, &vphysicsID, 4);
			id[4] = '\0';

			printf("Solid %d ID: '%s'\n", i, id);
		}

		const auto ctx = CreateDefaultRenderingContext();
		while (!WindowShouldClose()) {
			RenderFrame(ctx);
		}
	} catch (const std::exception &e) {
		printf("Error: %s\n", e.what());
		return -1;
	}

	return 0;
}