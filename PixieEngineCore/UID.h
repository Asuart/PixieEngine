#ifndef PIXIE_ENGINE_CORE_UID
#define PIXIE_ENGINE_CORE_UID

#include "pch.h"

class UID {
	static std::atomic<uint64_t> counter;
	const uint64_t value;
public:
	UID();

	bool operator==(const UID& other);
	bool operator!=(const UID& other);
};


#endif // PIXIE_ENGINE_CORE_UID