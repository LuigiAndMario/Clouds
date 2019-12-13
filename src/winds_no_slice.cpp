#define DEBUG (true)

// STL
#include <vector>

// VTK general
#include <vtkVersion.h>
#include <vtkXMLImageDataReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkDataSetMapper.h>
#include <vtkImageMapper.h>
#include <vtkImageSlice.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

// Colour
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkImageProperty.h>

// Vector Field
#include <vtkArrowSource.h>
#include <vtkCellArray.h>
#include <vtkGlyph2D.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkGradientFilter.h>
#include <vtkAssignAttribute.h>

#include <vtkGlyph3DMapper.h>
// Slicing
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>

// Slider
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>

// Button
#include <vtkButtonWidget.h>
#include <vtkTexturedButtonRepresentation2D.h>

// Text
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

// ImageData to PolyData
#include <vtkImageDataGeometryFilter.h>
#include <vtkPolyData.h>


static const std::string files[3] = {
        "../../cloud_data/winds/winds_10.vti_scaled.vti",
        "../../cloud_data/winds/winds_20.vti_scaled.vti",
        "../../cloud_data/winds/winds_30.vti_scaled.vti"
};


double seconds(clock_t time) {
    return double(clock() - time) / CLOCKS_PER_SEC;
}

int main(int, char *[]) {
    cout << "This program will display the wind data" << endl << std::flush;
    clock_t time;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Reading the files
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<vtkSmartPointer<vtkXMLImageDataReader>> readers(3);
    for (int i = 0 ; i < 3 ; i++) {
        std::string file = files[i];
        cerr << "Reading file " << file << "...";
        time = clock();
        vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
        reader->SetFileName(file.c_str());
        reader->Update();

        readers[i] = reader;
        cerr << " done (" << seconds(time) << " s)" << endl;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the images
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<vtkSmartPointer<vtkImageData>> vector_fields(3);
    // For each timestep
    for (int i = 0; i < vector_fields.size(); i++) {
        vtkSmartPointer<vtkImageData> vector_field = vtkSmartPointer<vtkImageData>::New();
        vector_field = readers[i]->GetOutput();
        vector_field->GetPointData()->SetActiveVectors(vector_field->GetPointData()->GetScalars()->GetName());
        vector_fields[i] = vector_field;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the glyph3D mappers
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<vtkSmartPointer<vtkGlyph3DMapper>> glyph_mappers(3);
    for (int i = 0; i < 3; i++) {
        vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
        vtkSmartPointer<vtkGlyph3DMapper> glyph3Dmapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
        glyph3Dmapper->SetSourceConnection(arrowSource->GetOutputPort());
        glyph3Dmapper->SetInputData(vector_fields[i]);
        glyph3Dmapper->Update();

        glyph_mappers[i] = glyph3Dmapper;
    }

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(glyph_mappers[0]);

    // Create a renderer, render window, and interactor
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    // Add the actor to the scene
    renderer->AddActor(actor);

    // Render and interact
    renderWindow->Render();
    renderWindowInteractor->Start();


    return EXIT_SUCCESS;
}
