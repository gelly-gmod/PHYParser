#include "BSP.h"

#include <raylib.h>
#include "PHY.h"
#include "open-phy-file-dialog.h"
#include "prompt.h"
#include "exceptions/UnsupportedModelType.h"

#include <cstdio>
#include <optional>
#include <stdexcept>
#include <chrono>

int screenWidth = 800;
int screenHeight = 600;

auto LoadPHYFile(const char *filepath) -> PHYParser::PHY::ParserInput {
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

	return {std::move(data), fileSize};
}

auto InitializeRaylib() -> void {
	InitWindow(screenWidth, screenHeight, "PHYParser Test");
	SetTargetFPS(144);
}

struct RenderingContext {
	Camera3D camera;
	std::vector<PHYParser::BSP::BSP::Model> bmodels;
	std::vector<PHYParser::Semantics::Solid> solids;
	std::vector<Model> models;
	const char *phyFileName;
	bool showWireframe = true;
	float loadTimeMs = 0.0f;
	int totalTriangles = 0;
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

	auto modelInfoY = solidInfoY + 30;
	auto modelCount = 0;
	for (const auto &model : ctx.bmodels) {
		char modelInfo[1024] = {0};
		snprintf(modelInfo, sizeof(modelInfo),
		         "Model '*%d': %llu solids", modelCount++, model.solids.size());

		DrawText(modelInfo, 10, modelInfoY, 20, WHITE);
		modelInfoY += 30;
	}

	DrawText("Press SPACE to toggle wireframe", 10, screenHeight - 30, 20,
	         WHITE);
	DrawText(TextFormat("Load time: %.2fms | Total triangles: %d triangles", ctx.loadTimeMs, ctx.totalTriangles),
	         10,
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
	DisableCursor();
}

auto CreateDefaultRenderingContext(
	std::vector<PHYParser::BSP::BSP::Model> models,
	std::vector<PHYParser::Semantics::Solid> solids,
	float loadTimeMs) -> RenderingContext {
	RenderingContext ctx = {
		.camera = {
			.position = {10.0f, -10.0f, 10.0f},
			.target = {0.0f, 0.0f, 0.0f},
			.up = {0.0f, -1.0f, 0.0f},
			.fovy = 65.0f,
			.projection = CAMERA_PERSPECTIVE
		},
		.bmodels = std::move(models),
		.solids = std::move(solids),
		.phyFileName = nullptr,
		.showWireframe = true,
		.loadTimeMs = loadTimeMs
	};

	std::vector<PHYParser::Semantics::Solid> mergedSolids;
	mergedSolids.reserve(ctx.bmodels.size() + ctx.solids.size());

	// copy each solid from the BSP models
	for (const auto &model : ctx.bmodels) {
		mergedSolids.insert(mergedSolids.end(), model.solids.begin(), model.solids.end());
	}

	mergedSolids.insert(mergedSolids.end(), ctx.solids.begin(), ctx.solids.end());

	for (const auto &solid : mergedSolids) {
		Mesh mesh = {0};
		mesh.vertexCount = solid.GetVertexCount();
		mesh.triangleCount = solid.GetTriangleCount();
		mesh.vertices = new float[mesh.vertexCount * 3];
		mesh.texcoords = nullptr;
		mesh.normals = nullptr;
		mesh.tangents = nullptr;
		mesh.colors = new unsigned char[mesh.vertexCount * 4];
		mesh.indices = nullptr;

		size_t currentVertex = 0;
		for (size_t i = 0; i < mesh.triangleCount; i++) {
			ctx.totalTriangles++;

			Vector3 v0 = {solid.GetTriangles()[i].vertices[0].x,
			              solid.GetTriangles()[i].vertices[0].y,
			              solid.GetTriangles()[i].vertices[0].z};

			Vector3 v1 = {solid.GetTriangles()[i].vertices[1].x,
			              solid.GetTriangles()[i].vertices[1].y,
			              solid.GetTriangles()[i].vertices[1].z};

			Vector3 v2 = {solid.GetTriangles()[i].vertices[2].x,
			              solid.GetTriangles()[i].vertices[2].y,
			              solid.GetTriangles()[i].vertices[2].z};

			auto *vertices = reinterpret_cast<Vector3 *>(mesh.vertices);
			vertices[currentVertex++] = v0;
			vertices[currentVertex++] = v1;
			vertices[currentVertex++] = v2;

			for (int j = 0; j < 3; ++j) {
				mesh.colors[(currentVertex - 3 + j) * 4 + 0] = GetRandomValue(0, 255);
				mesh.colors[(currentVertex - 3 + j) * 4 + 1] = GetRandomValue(0, 255);
				mesh.colors[(currentVertex - 3 + j) * 4 + 2] = GetRandomValue(0, 255);
				mesh.colors[(currentVertex - 3 + j) * 4 + 3] = 255;
			}
		}

		UploadMesh(&mesh, false);
		delete[] mesh.vertices;
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
		if (const auto ext = GetFileExtension(inputFile); strcmp(ext, ".phy") != 0 && strcmp(ext, ".bsp") != 0) {
			throw std::runtime_error("Invalid file extension: " + std::string(ext));
		}

		std::vector<PHYParser::Semantics::Solid> solids;
		std::vector<PHYParser::BSP::BSP::Model> bmodels;

		const auto loadStart = std::chrono::high_resolution_clock::now();
		if (strcmp(GetFileExtension(inputFile), ".phy") == 0) {
			const auto phy = PHYParser::PHY{LoadPHYFile(inputFile)};

			for (int i = 0; i < phy.GetSolidCount(); ++i) {
				solids.push_back(phy.GetSolid(i));
			}
		} else {
			const auto bsp = PHYParser::BSP::BSP{LoadPHYFile(inputFile)};
			for (int i = 0; i < bsp.GetModelCount(); ++i) {
				bmodels.push_back(bsp.GetModel(i));
			}
		}
		const auto loadEnd = std::chrono::high_resolution_clock::now();
		const auto loadTimeMs = std::chrono::duration<float, std::milli>(loadEnd - loadStart).count();

		if (bmodels.size() > 1) {
			// ask the user if they want to load all models
			if (!util::Prompt("Multiple models detected",
			                  "Load all models? If you do, you may visualize unrelated models like func_occluders or func_illusionary instances which can clog up the visualizer. Continue?")) {
				bmodels.resize(1);
			}
		}

		auto ctx = CreateDefaultRenderingContext(std::move(bmodels), std::move(solids), loadTimeMs);
		ctx.phyFileName = inputFile;

		while (!WindowShouldClose()) {
			// update camera
			UpdateCamera(&ctx.camera, CAMERA_FREE);
			RenderFrame(ctx);
			// lock mouse to window

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