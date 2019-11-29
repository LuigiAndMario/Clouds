#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkImageActor.h>
#include <vtkImageViewer2.h>
#include <vtkXMLImageDataReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkImageDataGeometryFilter.h>

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

static void CreateColorImage(vtkImageData*);

int main(int, char *[]) {
    // Read file
    std::cout << "reading data...";
    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    reader->SetFileName(files[0].c_str());
    reader->Update();
    std::cout << " [done]" << std::endl;

    /*
    // Visualize
    vtkSmartPointer<vtkDataSetMapper> mapper =
        vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    */

    // Covert image to polydata
    std::cout << "converting to polydata";
    vtkSmartPointer<vtkImageDataGeometryFilter> imageDataGeometryFilter = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
    imageDataGeometryFilter->SetInputConnection(reader->GetOutputPort());
    imageDataGeometryFilter->Update();
    std::cout << " [done]" << std::endl;

    // Create mapper and actor
    std::cout << "creating poly data mapper";
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(imageDataGeometryFilter->GetOutputPort());
    std::cout << " [done]" << std::endl;

    vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer =
        vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->SetBackground(1,1,1);

    vtkSmartPointer<vtkRenderWindow> renderWindow =
        vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(1500, 1000);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;

/*
    vtkSmartPointer<vtkImageData> colorImage = vtkSmartPointer<vtkImageData>::New();
    // CreateColorImage(colorImage);
    // colorImage->Print(std::cerr);
    colorImage = reader->GetOutput();
    colorImage->AllocateScalars(VTK_FLOAT, 3);
    colorImage->Print(std::cerr);

    vtkSmartPointer<vtkImageResliceMapper> imageResliceMapper = vtkSmartPointer<vtkImageResliceMapper>::New();
    imageResliceMapper->SetInputData(colorImage);

    vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
    imageSlice->SetMapper(imageResliceMapper);

    // Setup renderers
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddViewProp(imageSlice);
    renderer->ResetCamera();
    renderer->SetBackground(1, 1, 1);

    // Setup render window
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(1500, 1000);
    renderWindow->AddRenderer(renderer);

    // Setup render window interactor
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
        vtkSmartPointer<vtkRenderWindowInteractor>::New();

    vtkSmartPointer<vtkInteractorStyleImage> style =
        vtkSmartPointer<vtkInteractorStyleImage>::New();

    renderWindowInteractor->SetInteractorStyle(style);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
*/
}
