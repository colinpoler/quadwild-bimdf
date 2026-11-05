#pragma once
// Exact field order from QRemeshify/lib/data.py (Windows x64, MSVC).
struct QRCompatParameters {
    bool remesh;
    float sharpAngle;
    float alpha;
    float scaleFact;
    bool hasFeature;
    bool hasField;
};

struct QRCompatPatchParameters {
    bool useFlowSolver;
    const char* flow_config_filename;
    const char* satsuma_config_filename;
    bool initialRemeshing;
    double initialRemeshingEdgeFactor;
    bool reproject;
    bool splitConcaves;
    bool finalSmoothing;
    int ilpMethod;
    double alpha;
    bool isometry;
    bool regularityQuadrilaterals;
    bool regularityNonQuadrilaterals;
    double regularityNonQuadrilateralsWeight;
    bool alignSingularities;
    double alignSingularitiesWeight;
    bool repeatLosingConstraintsIterations;
    bool repeatLosingConstraintsQuads;
    bool repeatLosingConstraintsNonQuads;
    bool repeatLosingConstraintsAlign;
    bool feasibilityFix;
    bool hardParityConstraint;
    double timeLimit;
    double gapLimit;
    double minimumGap;
    const float* callbackTimeLimit;
    const float* callbackGapLimit;
    int chartSmoothingIterations;
    int quadrangulationFixedSmoothingIterations;
    int quadrangulationNonFixedSmoothingIterations;
    bool doubletRemoval;
    int resultSmoothingIterations;
    double resultSmoothingNRing;
    int resultSmoothingLaplacianIterations;
    double resultSmoothingLaplacianNRing;
};
