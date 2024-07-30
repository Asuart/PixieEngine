#include "pch.h"
#include "UID.h"

std::atomic<uint64_t> UID::counter = 0;

UID::UID() : value(UID::counter++) {}

bool UID::operator==(const UID& other) {
	return value == other.value;
}

bool UID::operator!=(const UID& other) {
	return value != other.value;
}