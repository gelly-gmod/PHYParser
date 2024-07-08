# PHYParser

A simple C++20 library for parsing Source Engine PHY files and PhysCollide data in maps.

## Compiling

This project uses CMake, and builds a test program when `BUILD_PHYPARSER_TEST` is equal to `ON`.
To add it, just use `add_subdirectory(path/to/PHYParser)`, and add an include directory to `src/`.

### Support

This library should work on both Windows and Linux, but has only been tested on 64-bit Windows.

Known good compilers:

- Clang 18.1.4 and later
- MSVC 19.36.32532 and later (although it should work on older versions)

It should work on GCC, but it has not been tested.

## Usage

### PHY parsing

```c++
#include <PHY.h>

int main() {
	std::unique_ptr<char[]> phyFileData = ...;
	size_t phyFileSize = ...;
	
	PHYParser::PHY phy({std::move(phyFileData), phyFileSize});
	// Do anything with the PHY file
	
	for (int solidIndex = 0; solidIndex < phy.GetSolidCount(); solidIndex++) {
		PHYParser::Semantics::Solid solid = phy.GetSolid(solidIndex);
		// Do anything with the solid
		printf("Solid %d has %d triangles\n", solidIndex, solid.GetTriangleCount());
	}
	
	return 0;
}	
```

### BSP parsing

```c++
#include <BSP.h>

int main() {
	std::unique_ptr<char[]> bspFileData = ...;
	size_t bspFileSize = ...;
	
	PHYParser::BSP::BSP bsp({std::move(bspFileData), bspFileSize});
	// Do anything with the BSP file
	
	for (int modelIndex = 0; modelIndex < bsp.GetModelCount(); modelIndex++) {
		PHYParser::BSP::BSP::Model model = bsp.GetModel(modelIndex);
		// Do anything with the model
		printf("Model %d has %d solids\n", model.index, model.solids.size());
		
		for (const auto& solids : model.solids) {
			printf("Solid has %d triangles\n", solid.triangles.size());
		}
	}
	
	return 0;
}
```

### Testing

There's no specific CMake config required, so you may use whatever your IDE provides.
Since the test program is built by default, you should be able to run the `PHYParserTest` target after configuring the
project.