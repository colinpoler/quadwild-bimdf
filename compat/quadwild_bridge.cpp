#include "abi.h"
#include "../quadwild/functions.h"
#include "../quadwild/trace.h"
#include <exception>
#include <iostream>
#include <string>
#include <type_traits>
#include <cstddef>

static_assert(sizeof(QRCompatParameters) == sizeof(Parameters));
static_assert(offsetof(QRCompatParameters, hasFeature) == offsetof(Parameters, hasFeature));

extern "C" void remeshAndField2(const QRCompatParameters* api,
                                 const char* meshFilename,
                                 const char* sharpFilename,
                                 const char* fieldFilename) noexcept {
    if (!api || !meshFilename || !sharpFilename || !fieldFilename) return;
    try {
        Parameters params;
        params.remesh = api->remesh;
        params.sharpAngle = api->sharpAngle;
        params.alpha = api->alpha;
        params.scaleFact = api->scaleFact;
        params.hasFeature = api->hasFeature;
        params.hasField = api->hasField;
        FieldTriMesh mesh;
        bool allQuad = false;
        if (!mesh.LoadTriMesh(meshFilename, allQuad)) {
            std::cerr << "remeshAndField2: unable to load " << meshFilename << '\n';
            return;
        }
        mesh.UpdateDataStructures();
        remeshAndField(mesh, params, meshFilename, sharpFilename, fieldFilename);
    } catch (const std::exception& e) {
        std::cerr << "remeshAndField2: " << e.what() << '\n';
    } catch (...) {
        std::cerr << "remeshAndField2: unknown exception\n";
    }
}

extern "C" bool trace2(const char* filenamePrefix) noexcept {
    if (!filenamePrefix) return false;
    try {
        TraceMesh mesh;
        return trace(std::string(filenamePrefix), mesh);
    } catch (const std::exception& e) {
        std::cerr << "trace2: " << e.what() << '\n';
    } catch (...) {
        std::cerr << "trace2: unknown exception\n";
    }
    return false;
}
