#include "abi.h"
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>

// Reuse exactly the upstream quadrangulation implementation. Only its C++
// main symbol is renamed; actual_main remains callable from this translation unit.
#define main quadpatches_original_cli_main
#include "../components/quad_from_patches/main.cpp"
#undef main

namespace {
namespace fs = std::filesystem;
std::atomic<unsigned long long> nextId{0};

// The upstream parser consumes a strict ordered, line-oriented setup file.
// Eight callback values are defined by QRemeshify's Blender properties.
void writeSetup(const fs::path& file, const QRCompatPatchParameters& p,
                float scale, int fixedClusters) {
    std::ofstream f(file, std::ios::binary);
    if (!f) throw std::runtime_error("cannot create temporary setup file");
    f << std::setprecision(9) << std::fixed;
    f << "alpha " << p.alpha << '\n';
    f << "ilpMethod " << (p.ilpMethod == 2 ? 0 : 1) << '\n';
    f << "timeLimit " << p.timeLimit << '\n';
    f << "gapLimit " << p.gapLimit << '\n';
    f << "callbackTimeLimit 8";
    if (!p.callbackTimeLimit) throw std::runtime_error("callbackTimeLimit is null");
    for (int i = 0; i < 8; ++i) f << ' ' << p.callbackTimeLimit[i];
    f << '\n' << "callbackGapLimit 8";
    if (!p.callbackGapLimit) throw std::runtime_error("callbackGapLimit is null");
    for (int i = 0; i < 8; ++i) f << ' ' << p.callbackGapLimit[i];
    f << '\n';
    f << "minimumGap " << p.minimumGap << '\n';
    f << "isometry " << int(p.isometry) << '\n';
    f << "regularityQuadrilaterals " << int(p.regularityQuadrilaterals) << '\n';
    f << "regularityNonQuadrilaterals " << int(p.regularityNonQuadrilaterals) << '\n';
    f << "regularityNonQuadrilateralsWeight " << p.regularityNonQuadrilateralsWeight << '\n';
    f << "alignSingularities " << int(p.alignSingularities) << '\n';
    f << "alignSingularitiesWeight " << p.alignSingularitiesWeight << '\n';
    f << "repeatLosingConstraintsIterations " << int(p.repeatLosingConstraintsIterations) << '\n';
    f << "repeatLosingConstraintsQuads " << int(p.repeatLosingConstraintsQuads) << '\n';
    f << "repeatLosingConstraintsNonQuads " << int(p.repeatLosingConstraintsNonQuads) << '\n';
    f << "repeatLosingConstraintsAlign " << int(p.repeatLosingConstraintsAlign) << '\n';
    f << "hardParityConstraint " << int(p.hardParityConstraint) << '\n';
    f << "scaleFact " << scale << '\n';
    f << "fixedChartClusters " << fixedClusters << '\n';
    f << "useFlowSolver " << int(p.useFlowSolver) << '\n';
    auto validatePath = [](const char* s) -> std::string {
        if (!s || !*s) throw std::runtime_error("missing solver configuration filename");
        std::string v(s);
        if (v.find('"') != std::string::npos || v.size() > 999)
            throw std::runtime_error("invalid solver configuration filename");
        return v;
    };
    f << "flow_config_filename \"" << validatePath(p.flow_config_filename) << "\"\n";
    f << "satsuma_config_filename \"" << validatePath(p.satsuma_config_filename) << "\"\n";
    if (!f) throw std::runtime_error("error writing setup file");
}

struct TempSetup {
    fs::path path;
    explicit TempSetup() {
        const auto tick = std::chrono::steady_clock::now().time_since_epoch().count();
        const auto id = ++nextId;
        path = fs::temp_directory_path() /
               ("qremeshify_" + std::to_string(tick) + "_" + std::to_string(id) + ".txt");
    }
    ~TempSetup() { std::error_code ec; fs::remove(path, ec); }
};
} // namespace

extern "C" int quadPatches(const char* meshFile,
                           const QRCompatPatchParameters* api,
                           float scaleFact, int fixedChartClusters,
                           bool /*enableSmoothing*/) noexcept {
    if (!meshFile || !api) return 1;
    try {
        TempSetup setup;
        writeSetup(setup.path, *api, scaleFact, fixedChartClusters);
        // Upstream actual_main uses argv[1] OBJ, argv[2] output index,
        // argv[3] ordered setup text. QRemeshify expects output index zero.
        std::string mesh(meshFile), index("0"), setupName = setup.path.string();
        char executable[] = "quad_from_patches";
        char* argv[] = {executable, mesh.data(), index.data(), setupName.data(), nullptr};
        return actual_main(4, argv);
    } catch (const std::exception& e) {
        std::cerr << "quadPatches: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "quadPatches: unknown exception\n";
        return 1;
    }
}
