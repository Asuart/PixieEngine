#pragma once
#include "pch.h"

class UID {
public:
	UID();

	std::string ToString();

	bool operator==(const UID& other);
	bool operator!=(const UID& other);

protected:
	static std::atomic<uint64_t> counter;
	const uint64_t m_value;
};
