#pragma once
#include "PixieEngineCoreHeaders.h"

class UID {
	static std::atomic<uint64_t> counter;
	const uint64_t m_value;

public:
	UID();

	bool operator==(const UID& other);
	bool operator!=(const UID& other);
	std::string ToString();
};
