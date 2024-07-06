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
}

#endif //VECTOR_H
