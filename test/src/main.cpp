#include <raylib.h>
#include "PHY.h"
#include "open-phy-file-dialog.h"
#include "exceptions/UnsupportedModelType.h"

#include <cstdio>
#include <optional>
#include <stdexcept>
#include <chrono>

int screenWidth = 800;
int screenHeight = 600;

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
	std::vector<Model> models;
	const char *phyFileName;
	bool showWireframe = true;
	float loadTimeMs = 0.0f;
};

auto RenderFrame(const RenderingContext &ctx) -> void {
	// dimensions may have changed
	screenWidth = GetScreenWidth();
	screenHeight = GetScreenHeight();

	BeginDrawing();
	ClearBackground(BLACK);

	BeginMode3D(ctx.camera);
	DrawGrid(10, 1.0f);

	for (const auto &model : ctx.models) {
		if (ctx.showWireframe) {
			DrawModelWires(model, {0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
		} else {
			DrawModel(model, {0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
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

	DrawText("Press SPACE to toggle wireframe", 10, screenHeight - 30, 20,
	         WHITE);
	DrawText(TextFormat("Load time: %.2fms", ctx.loadTimeMs), 10,
	         screenHeight - 60, 20, WHITE);

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
	std::vector<PHYParser::Semantics::Solid> solids,
	float loadTimeMs) -> RenderingContext {
	RenderingContext ctx = {
		.camera = {
			.position = {10.0f, -10.0f, 10.0f},
			.target = {0.0f, 0.0f, 0.0f},
			.up = {0.0f, -1.0f, 0.0f},
			.fovy = 45.0f,
			.projection = CAMERA_PERSPECTIVE
		},
		.solids = std::move(solids),
		.phyFileName = nullptr,
		.showWireframe = true,
		.loadTimeMs = loadTimeMs
	};

	for (const auto &solid : ctx.solids) {
		Mesh mesh = {0};
		mesh.vertexCount = solid.GetVertexCount();
		mesh.triangleCount = solid.GetTriangleCount();
		mesh.vertices = new float[mesh.vertexCount * 3];
		mesh.texcoords = nullptr;
		mesh.normals = nullptr;
		mesh.tangents = nullptr;
		mesh.colors = new unsigned char[mesh.vertexCount * 4];
		mesh.indices = new unsigned short[mesh.triangleCount * 3];

		for (size_t i = 0; i < mesh.vertexCount; i += 3) {
			Vector3 v0 = {solid.GetTriangles()[i / 3].vertices[0].x,
			              solid.GetTriangles()[i / 3].vertices[0].y,
			              solid.GetTriangles()[i / 3].vertices[0].z};

			Vector3 v1 = {solid.GetTriangles()[i / 3].vertices[1].x,
			              solid.GetTriangles()[i / 3].vertices[1].y,
			              solid.GetTriangles()[i / 3].vertices[1].z};

			Vector3 v2 = {solid.GetTriangles()[i / 3].vertices[2].x,
			              solid.GetTriangles()[i / 3].vertices[2].y,
			              solid.GetTriangles()[i / 3].vertices[2].z};

			auto *vertices = reinterpret_cast<Vector3 *>(mesh.vertices);
			vertices[i] = v0;
			vertices[i + 1] = v1;
			vertices[i + 2] = v2;

			mesh.indices[i] = i;
			mesh.indices[i + 1] = i + 1;
			mesh.indices[i + 2] = i + 2;

			for (int j = 0; j < 3; ++j) {
				mesh.colors[i * 4 + j * 4] = GetRandomValue(0, 255);
				mesh.colors[i * 4 + j * 4 + 1] = GetRandomValue(0, 255);
				mesh.colors[i * 4 + j * 4 + 2] = GetRandomValue(0, 255);
				mesh.colors[i * 4 + j * 4 + 3] = 255;
			}
		}

		UploadMesh(&mesh, false);
		delete[] mesh.vertices;
		delete[] mesh.indices;
		delete[] mesh.colors;
		auto model = LoadModelFromMesh(mesh);
		ctx.models.push_back(model);
	}

	return ctx;
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
		const auto start = std::chrono::high_resolution_clock::now();
		const auto phy = LoadPHYFile(inputFile);
		const auto end = std::chrono::high_resolution_clock::now();
		const auto loadTimeMs = std::chrono::duration<float, std::milli>(
			end - start).count();

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

		auto ctx = CreateDefaultRenderingContext(std::move(solids), loadTimeMs);
		ctx.phyFileName = inputFile;
		while (!WindowShouldClose()) {
			// update camera
			UpdateCamera(&ctx.camera, CAMERA_ORBITAL);
			RenderFrame(ctx);

			if (IsKeyPressed(KEY_SPACE)) {
				ctx.showWireframe = !ctx.showWireframe;
			}
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