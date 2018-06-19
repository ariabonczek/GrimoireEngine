#pragma once

#include <stdint.h>

struct SizeAlign
{
	SizeAlign() {}
	SizeAlign(uint32_t size, uint32_t align) :size(size), align(align) {}

	uint32_t size;
	uint32_t align;
};