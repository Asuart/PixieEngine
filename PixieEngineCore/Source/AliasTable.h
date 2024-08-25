#pragma once
#include "pch.h"
#include "RTMath.h"

class AliasTable {
public: 
	AliasTable() = default;
    AliasTable(const std::vector<Float> weights);

    int32_t Sample(Float u, Float* pmf = nullptr, Float* uRemapped = nullptr) const;
    size_t size() const;
    Float PMF(int32_t index) const;

protected:
    struct Bin {
        double q, p;
        int32_t alias;
    };
    std::vector<Bin> m_bins;
};
