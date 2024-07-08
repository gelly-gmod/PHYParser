#ifndef VECTOR_H
#define VECTOR_H

namespace PHYParser::Format {
struct Vector {
	float x, y, z;
};

struct VectorAlign16 {
	float x, y, z;
	float _padding;
};

static_assert(sizeof(Vector) == 12, "Vector size is not 12 bytes, bailing as parsing will most likely fail");
static_assert(sizeof(VectorAlign16) == 16,
              "VectorAlign16 size is not 16 bytes, bailing as parsing will most likely fail");
}

#endif //VECTOR_H
