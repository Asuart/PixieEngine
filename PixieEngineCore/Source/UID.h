#pragma once
#include "headers.h"

class UID {
	static std::atomic<uint64_t> counter;
	const uint64_t value;
public:
	UID();

	bool operator==(const UID& other);
	bool operator!=(const UID& other);
};
