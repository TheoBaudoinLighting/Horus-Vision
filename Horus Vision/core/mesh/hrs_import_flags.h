#pragma once

#include <vector>
#include <assimp/Importer.hpp>

struct ConfigFlagPair {
    bool* Config;
    unsigned int Flag; 
};

inline void ApplyConfigFlags(int& Flags, const std::vector<ConfigFlagPair>& ConfigFlags) {
    for (const auto& kConfigFlag : ConfigFlags) {
        if (*(kConfigFlag.Config)) {
            Flags |= kConfigFlag.Flag;
        }
    }
}