#pragma once
#include "pch.h"

inline uint64_t MurmurHash64A(const unsigned char* key, size_t len, uint64_t seed) {
    const uint64_t m = 0xc6a4a7935bd1e995ull;
    const int32_t r = 47;

    uint64_t h = seed ^ (len * m);

    const unsigned char* end = key + 8 * (len / 8);

    while (key != end) {
        uint64_t k;
        std::memcpy(&k, key, sizeof(uint64_t));
        key += 8;

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    switch (len & 7) {
    case 7: h ^= uint64_t(key[6]) << 48; [[fallthrough]];
    case 6: h ^= uint64_t(key[5]) << 40; [[fallthrough]];
    case 5: h ^= uint64_t(key[4]) << 32; [[fallthrough]];
    case 4: h ^= uint64_t(key[3]) << 24; [[fallthrough]];
    case 3: h ^= uint64_t(key[2]) << 16; [[fallthrough]];
    case 2: h ^= uint64_t(key[1]) << 8; [[fallthrough]];
    case 1:
        h ^= uint64_t(key[0]);
        h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

template <typename... Args>
inline void hashRecursiveCopy(char* buf, Args...);

template <>
inline void hashRecursiveCopy(char* buf) {}

template <typename T, typename... Args>
inline void hashRecursiveCopy(char* buf, T v, Args... args) {
    memcpy(buf, &v, sizeof(T));
    hashRecursiveCopy(buf + sizeof(T), args...);
}

template <typename... Args>
inline uint64_t Hash(Args... args) {
    constexpr size_t sz = (sizeof(Args) + ... + 0);
    constexpr size_t n = (sz + 7) / 8;
    uint64_t buf[n];
    hashRecursiveCopy((char*)buf, args...);
    return MurmurHash64A((const unsigned char*)buf, sz, 0);
}
