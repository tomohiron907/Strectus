#include "VtkProcessor.h"
#include "cgalProcessor.h"
#include "lib3mfProcessor.h"



#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <limits> // for std::numeric_limits
#include <iostream>


namespace PMP = CGAL::Polygon_mesh_processing;
using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Mesh   = CGAL::Surface_mesh<Kernel::Point_3>;



int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename.vtu> <outer_boundary.stl>" << std::endl;
        return EXIT_FAILURE;
    }
    VtkProcessor vtkProcessor(argv[1]);
    vtkProcessor.showInfo();

    if(!vtkProcessor.LoadAndPrepareData(argv[1], vtkProcessor.vtuData, vtkProcessor.lookupTable, vtkProcessor.stressRange)){
        std::cerr << "Error: LoadAndPrepareData failed." << std::endl;
        return EXIT_FAILURE;
    }

    vtkProcessor.stressDisplay(vtkProcessor.vtuData, vtkProcessor.lookupTable, vtkProcessor.stressRange, vtkProcessor.renderer);
    vtkProcessor.prepareStressValues(vtkProcessor.minStress, vtkProcessor.maxStress);
    vtkProcessor.generateIsoSurface(vtkProcessor.vtuData, vtkProcessor.stressValues);

    //vtkProcessor.deleteSmallIsosurface(vtkProcessor.isoSurfaces, 5000); // 面積が5000未満の等値面を削除

    //TODO:isoSurface[0]にはメッシュデータがないので、１からにしている。deleteSmallIsosurfaceを修正して、これも省くようにする。
    for (int i = 1; i < vtkProcessor.isoSurfaceNum; ++i) {
        auto expandedSurface = vtkProcessor.scalePolyData(vtkProcessor.isoSurfaces[i], 1.01);
        auto smoothedSurface = vtkProcessor.makePolyDataSmooth(expandedSurface);
        auto reversedSurface = vtkProcessor.reversePolyDataOrientation(smoothedSurface);
        vtkProcessor.polyDataDisplay(smoothedSurface, vtkProcessor.renderer);
        vtkProcessor.savePolyDataAsSTL(smoothedSurface, std::to_string(i) + ".stl");
    }

    CGALProcessor cgalProcessor;
    cgalProcessor.getFileNames();
    cgalProcessor.prepareMeshes(argv[2]);
    cgalProcessor.divideMeshes();
    
    int dividedMeshNum = cgalProcessor.getDivideMeshNum();

    Lib3mfProcessor lib3mfProcessor;
    
    lib3mfProcessor.getMeshes(dividedMeshNum);
    lib3mfProcessor.setMetaData();
    lib3mfProcessor.save3mf("merged_data.3mf");


    return EXIT_SUCCESS;
} 