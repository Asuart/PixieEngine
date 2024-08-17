#include "UID.h"

std::atomic<uint64_t> UID::counter = 0;

UID::UID() : m_value(UID::counter++) {}

bool UID::operator==(const UID& other) {
	return m_value == other.m_value;
}

bool UID::operator!=(const UID& other) {
	return m_value != other.m_value;
}

std::string UID::ToString() {
	return std::to_string(m_value);
}