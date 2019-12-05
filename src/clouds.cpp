#define DEBUG (false)

#include <vtkSmartPointer.h>

// Reading
#include <vtkXMLImageDataReader.h>

// Interaction
#include <vtkInteractorStyleTrackballCamera.h>

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkAssignAttribute.h>
#include <vtkCamera.h>
#include <vtkExtractEdges.h>
#include <vtkGlyph3D.h>
#include <vtkGradientFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGridReader.h>


static const std::string files[] = {
    "../../cloud_data/cli/cli_10.vti_scaled.vti",
    "../../cloud_data/cli/cli_20.vti_scaled.vti",
    "../../cloud_data/cli/cli_30.vti_scaled.vti",
    "../../cloud_data/clw/clw_10.vti_scaled.vti",
    "../../cloud_data/clw/clw_20.vti_scaled.vti",
    "../../cloud_data/clw/clw_30.vti_scaled.vti",
    "../../cloud_data/pres/pres_10.vti_scaled.vti",
    "../../cloud_data/pres/pres_20.vti_scaled.vti",
    "../../cloud_data/pres/pres_30.vti_scaled.vti",
    "../../cloud_data/qr/qr_10.vti_scaled.vti",
    "../../cloud_data/qr/qr_20.vti_scaled.vti",
    "../../cloud_data/qr/qr_30.vti_scaled.vti",
    "../../cloud_data/ua/ua_10.vti_scaled.vti",
    "../../cloud_data/ua/ua_20.vti_scaled.vti",
    "../../cloud_data/ua/ua_30.vti_scaled.vti",
    "../../cloud_data/va/va_10.vti_scaled.vti",
    "../../cloud_data/va/va_20.vti_scaled.vti",
    "../../cloud_data/va/va_30.vti_scaled.vti",
    "../../cloud_data/wa/wa_10.vti_scaled.vti",
    "../../cloud_data/wa/wa_20.vti_scaled.vti",
    "../../cloud_data/wa/wa_30.vti_scaled.vti"
};


int main(int, char *[]) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the renderer and window interactor
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(1500, 1000);
    
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetBackground(1, 1, 1);
    renderWindow->AddRenderer(renderer);
    
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->SetBackground(1, 1, 1);
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Reading the file
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::string file = files[6];
    cerr << "Reading file " << file << "...";
    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    reader->SetFileName(file.c_str());
    reader->Update();
    cerr << " done" << endl;

    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the edges and tubes
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Creating the edges and tubes...";
    // Creating edges
    vtkSmartPointer<vtkExtractEdges> edges = vtkSmartPointer<vtkExtractEdges>::New();
    edges->SetInputConnection(reader->GetOutputPort());
    
    // Creating tubes (from edges)
    vtkSmartPointer<vtkTubeFilter> tubes = vtkSmartPointer<vtkTubeFilter>::New();
    tubes->SetInputConnection(edges->GetOutputPort());
    tubes->SetRadius(0.0625);
    tubes->SetVaryRadiusToVaryRadiusOff();
    tubes->SetNumberOfSides(32);
    
    // Creating mapper and actor
    vtkSmartPointer<vtkPolyDataMapper> tubesMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    tubesMapper->SetInputConnection(tubes->GetOutputPort());
    tubesMapper->SetScalarRange(0.0, 26.0);
    
    vtkSmartPointer<vtkActor> tubesActor = vtkSmartPointer<vtkActor>::New();
    tubesActor->SetMapper(tubesMapper);
    cerr << " done" << endl;
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the gradients
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Creating the gradients...";
    // Creating the gradients
    vtkSmartPointer<vtkGradientFilter> gradients = vtkSmartPointer<vtkGradientFilter>::New();
    gradients->SetInputConnection(reader->GetOutputPort());
    
    vtkSmartPointer<vtkAssignAttribute> vectors = vtkSmartPointer<vtkAssignAttribute>::New();
    vectors->SetInputConnection(gradients->GetOutputPort());
    vectors->Assign("Gradients", vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA);
    cerr << " done" << endl;
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the glyphs
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Creating the glyphs...";
    vtkSmartPointer<vtkArrowSource> arrow = vtkSmartPointer<vtkArrowSource>::New();
    
    vtkSmartPointer<vtkGlyph3D> glyphs = vtkSmartPointer<vtkGlyph3D>::New();
    glyphs->SetInputConnection(0, vectors->GetOutputPort());
    glyphs->SetInputConnection(1, arrow->GetOutputPort());
    glyphs->ScalingOn();
    glyphs->SetScaleModeToScaleByVector();
    glyphs->SetScaleFactor(0.25);
    glyphs->OrientOn();
    glyphs->ClampingOff();
    glyphs->SetVectorModeToUseVector();
    glyphs->SetIndexModeToOff();
    
    vtkSmartPointer<vtkPolyDataMapper> glyphMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    glyphMapper->SetInputConnection(glyphs->GetOutputPort());
    glyphMapper->ScalarVisibilityOff();
    
    vtkSmartPointer<vtkActor> glyphActor = vtkSmartPointer<vtkActor>::New();
    glyphActor->SetMapper(glyphMapper);
    
    renderer->AddActor(tubesActor);
    renderer->AddActor(glyphActor);
    renderer->SetBackground(0.328125, 0.347656, 0.425781);
    cerr << " done";
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Display
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    renderWindow->AddRenderer(renderer);
    
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);
    
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();
    renderWindowInteractor->Start();
    

    return EXIT_SUCCESS;
}
