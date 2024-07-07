#include "prompt.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace util {
bool Prompt(const char *title, const char *message) {
	return MessageBoxA(nullptr, message, title, MB_YESNO) == IDYES;
}
}