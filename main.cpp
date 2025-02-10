#include "VtkProcessor.h"
#include "cgalProcessor.h"
#include "lib3mfProcessor.h"

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <limits> 
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filename.vtu> <outer_boundary.stl>" << std::endl;
        return EXIT_FAILURE;
    }
    VtkProcessor vtkProcessor(argv[1]);
    vtkProcessor.showInfo();

    if(!vtkProcessor.LoadAndPrepareData()){
        std::cerr << "Error: LoadAndPrepareData failed." << std::endl;
        return EXIT_FAILURE;
    }

    if(!vtkProcessor.calcAverageStress()){
        std::cerr << "Error: CalcAverageStress failed." << std::endl;
        return EXIT_FAILURE;
    }

    vtkProcessor.prepareStressValues();
    auto dividedMeshes =  vtkProcessor.divideMesh();

    for (int i = 0; i < dividedMeshes.size(); ++i) {
        float minValue = vtkProcessor.getStressValues()[i];
        float maxValue = vtkProcessor.getStressValues()[i + 1];
        std::string fileName = "dividedMesh" + std::to_string(i) + "_" + std::to_string(minValue) + "_" + std::to_string(maxValue) + ".stl";
        vtkProcessor.savePolyDataAsSTL(dividedMeshes[i], fileName);
    }
    int dividedMeshNum = dividedMeshes.size();
    Lib3mfProcessor lib3mfProcessor;
    lib3mfProcessor.getMeshes();
    lib3mfProcessor.setMetaData();
    //std::string stlName = "./.temp/iso/dividedMesh0_15.183294_20015.183594.stl";
    // std::string stlName = "surface.stl";
    //lib3mfProcessor.getStl(stlName);
    

    // vtkProcessor.stressDisplay();
    // vtkProcessor.generateIsoSurface();

    // //vtkProcessor.deleteSmallIsosurface(vtkProcessor.isoSurfaces, 5000); // 面積が5000未満の等値面を削除

    // //TODO:isoSurface[0]にはメッシュデータがないので、１からにしている。deleteSmallIsosurfaceを修正して、これも省くようにする。
    // std::vector<vtkSmartPointer<vtkPolyData>> isoSurfaces= vtkProcessor.getIsoSurfaces();
    // int isoSurfaceNum = vtkProcessor.getIsoSurfaceNum();
    // for (int i = 1; i < isoSurfaceNum; ++i) {
    //     auto expandedSurface = vtkProcessor.scalePolyData(isoSurfaces[i], 1.01);
    //     auto smoothedSurface = vtkProcessor.makePolyDataSmooth(expandedSurface);
    //     auto reversedSurface = vtkProcessor.reversePolyDataOrientation(smoothedSurface);
    //     vtkProcessor.polyDataDisplay(smoothedSurface, vtkProcessor.getRenderer());
    //     vtkProcessor.savePolyDataAsSTL(smoothedSurface, std::to_string(i) + ".stl");
    // }
    //vtkProcessor.startRnederAndInteraction();

    // CGALProcessor cgalProcessor;
    // cgalProcessor.getFileNames();
    // cgalProcessor.prepareMeshes(argv[2]);
    // cgalProcessor.divideMeshes();
    
    // int dividedMeshNum = cgalProcessor.getDivideMeshNum();

    // Lib3mfProcessor lib3mfProcessor;
    // lib3mfProcessor.getMeshes(dividedMeshNum);
    // lib3mfProcessor.setMetaData();
    lib3mfProcessor.save3mf("merged_data.3mf");

    return EXIT_SUCCESS;
} 