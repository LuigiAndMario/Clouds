#include <vtkVersion.h>
#include <vtkXMLImageDataReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>
#include <vtkImageMapper.h>
#include <vtkImageResliceMapper.h>
#include <vtkImageSlice.h>

#include <vtkSphereSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <vtkImageResize.h>
#include <vtkImageSliceMapper.h>

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
    "../../cloud_data/wa/wa_30.vti_scaled.vti",
    
};

int main(int, char *[]) {
    // Read file
    std::cerr << "Reading file " << files[0] << "...";
    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    reader->SetFileName(files[0].c_str());
    reader->Update();
    std::cerr << " done" << std::endl;

    vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
//    image->Print(std::cerr);
    image = reader->GetOutput();
    image->AllocateScalars(VTK_FLOAT, 3);

    std::cerr << "Slicing...";
    vtkSmartPointer<vtkImageSliceMapper> imageSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
    imageSliceMapper->SetInputData(image);
    imageSliceMapper->Update();
//    imageSliceMapper->SetSliceNumber(74); // Trying out some specific slice
//    imageSliceMapper->Print(std::cerr);
    
    vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
    imageSlice->SetMapper(imageSliceMapper);
    imageSlice->Update();
    std::cerr << " done" << std::endl;
    

    // Setup renderers
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
//    renderer->AddViewProp(imageSlice);
    renderer->AddActor(imageSlice);
    renderer->ResetCamera();
    renderer->SetBackground(0, 0, 0);
    
    ////////////////////////////////////
    /// Debugging by displaying a sphere
    // Create a sphere
//    vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();
//
//    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
//    sphereSource->SetCenter(0.0, 0.0, 0.0);
//    sphereSource->SetRadius(1.0);
//    // Make the surface smooth.
//    sphereSource->SetPhiResolution(100);
//    sphereSource->SetThetaResolution(100);
//
//    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//    mapper->SetInputConnection(sphereSource->GetOutputPort());
//
//    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
//    actor->SetMapper(mapper);
//    actor->GetProperty()->SetColor(colors->GetColor3d("Cornsilk").GetData());
//
//    renderer->AddActor(actor);
//    renderer->SetBackground(colors->GetColor3d("DarkGreen").GetData());
    /////////////////////////////////////

    // Setup render window
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(1500, 1000);
    renderWindow->AddRenderer(renderer);

    // Setup render window interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();

    vtkSmartPointer<vtkInteractorStyleImage> style = vtkSmartPointer<vtkInteractorStyleImage>::New();
    style->SetInteractionModeToImageSlicing();
    renderWindowInteractor->SetInteractorStyle(style);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
