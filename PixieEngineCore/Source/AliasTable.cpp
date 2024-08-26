#include "pch.h"
#include "AliasTable.h"

AliasTable::AliasTable(const std::vector<Float> weights)
    : m_bins(weights.size()) {
    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    for (size_t i = 0; i < weights.size(); i++) {
        m_bins[i].p = weights[i] / sum;
    }

    struct Outcome {
        double pHat;
        size_t index;
    };
    std::vector<Outcome> under, over;
    for (size_t i = 0; i < m_bins.size(); i++) {
        double pHat = m_bins[i].p * m_bins.size();
        if (pHat < 1) {
            under.push_back(Outcome{ pHat, i });
        }
        else {
            over.push_back(Outcome{ pHat, i });
        }
    }

    while (!under.empty() && !over.empty()) {
        Outcome un = under.back(), ov = over.back();
        under.pop_back();
        over.pop_back();

        m_bins[un.index].q = un.pHat;
        m_bins[un.index].alias = ov.index;

        double pExcess = un.pHat + ov.pHat - 1;
        if (pExcess < 1) {
            under.push_back(Outcome{ pExcess, ov.index });
        }
        else {
            over.push_back(Outcome{ pExcess, ov.index });
        }
    }

    while (!over.empty()) {
        Outcome ov = over.back();
        over.pop_back();
        m_bins[ov.index].q = 1;
        m_bins[ov.index].alias = -1;
    }
    while (!under.empty()) {
        Outcome un = under.back();
        under.pop_back();
        m_bins[un.index].q = 1;
        m_bins[un.index].alias = -1;
    }
}

int32_t AliasTable::Sample(Float u, Float* pmf, Float* uRemapped) const {
    int32_t offset = std::min<int32_t>((int32_t)(u * m_bins.size()), m_bins.size() - 1);
    double up = std::min<double>(u * m_bins.size() - offset, OneMinusEpsilon);

    if (up < m_bins[offset].q) {
        if (pmf) {
            *pmf = m_bins[offset].p;
        }
        if (uRemapped) {
            *uRemapped = std::min<Float>(up / m_bins[offset].q, OneMinusEpsilon);
        }
        return offset;
    }
    else {
        int32_t alias = m_bins[offset].alias;
        if (pmf) {
            *pmf = m_bins[alias].p;
        }
        if (uRemapped) {
            *uRemapped = std::min<Float>((up - m_bins[offset].q) / (1.0f - m_bins[offset].q), OneMinusEpsilon);
        }
        return alias;
    }
}

size_t AliasTable::size() const {
	return m_bins.size(); 
}

Float AliasTable::PMF(int32_t index) const {
	return (Float)m_bins[index].p; 
}
