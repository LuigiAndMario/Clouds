// Display
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkActor.h>
#include <vtkImageActor.h>
#include <vtkPolyData.h>
#include <vtkCoordinate.h>
#include <vtkSphereSource.h>
#include <vtkButtonWidget.h>
#include <vtkTexturedButtonRepresentation2D.h>

// General
#include <vtkSmartPointer.h>
#include <vtkMetaImageReader.h>
#include <vtkXMLImageDataReader.h>
#include <vtkMarchingCubes.h>
#include <vtkImageData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkTextProperty.h>

// Image to PointSet
#include <vtkImageDataToPointSet.h>

// Volume
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
#include <vtkVolumeProperty.h>
#include "vtkEasyTransfer.hpp"



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

// Forward declarations
void readImageData(vtkSmartPointer<vtkXMLImageDataReader> &reader, std::string fileName);
static void CreateImage(vtkSmartPointer<vtkImageData> image, unsigned char *color1, unsigned char *color2);

//
// FROM prog_03
//
// Callback for the slider interaction
class vtkSliderCallback : public vtkCommand {
public:
    static vtkSliderCallback *New() {
        return new vtkSliderCallback;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*) {
        vtkSliderWidget *sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
        if (MarchingCubes)
            MarchingCubes->SetValue(0, static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
    }
    vtkSliderCallback() : MarchingCubes(0) {}
    vtkSmartPointer<vtkMarchingCubes> MarchingCubes;
};

// Callback for the button interaction
class vtkButtonCallback : public vtkCommand {
public:
    static vtkButtonCallback *New() {
        return new vtkButtonCallback;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*) {
        vtkSmartPointer<vtkButtonWidget> buttonWidget = reinterpret_cast<vtkButtonWidget*>(caller);
        int state = buttonWidget->GetSliderRepresentation()->GetState();
        // TASK 1 screen
        if (state == 0) {
            // disable TASK2
            renderWindowInteractor->SetInteractorStyle(styleTrackball);
            renderer->RemoveActor(volume);
            renderWindow->RemoveRenderer(transferRenderer);
            
            // add actor, renders and slider
            sliderWidget->EnabledOn();
            renderer->AddActor(actor);
            renderWindow->AddRenderer(whiteRender);
        }
        // TASK 2 screen
        else {
            // disable TASK1
            sliderWidget->EnabledOff();
            renderer->RemoveActor(actor);
            renderWindow->RemoveRenderer(whiteRender);
            
            // add actor, volume and transfer function interactor
            renderWindowInteractor->SetInteractorStyle(easyTransfer->GetInteractorStyle());
            renderer->AddActor(volume);
            renderWindow->AddRenderer(transferRenderer);
        }
        
    }
    vtkButtonCallback() : sliderWidget(0), renderer(0), actor(0), renderWindow(0), whiteRender(0) , renderWindowInteractor(0) , transferRenderer(0), volume(0), easyTransfer(0), styleTrackball(0){}
    vtkSmartPointer<vtkSliderWidget> sliderWidget;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderer> whiteRender;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<vtkRenderer> transferRenderer;
    vtkSmartPointer<vtkVolume> volume;
    vtkSmartPointer<vtkEasyTransfer> easyTransfer;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> styleTrackball;
};






///////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
    /*
    int lol = 0;
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    for (auto file: files) {
        if (lol++) continue; // Only first
        std::cerr << "Adding " << file << "...";

        // Read data from file
        vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
        readImageData(reader, file);

        // Convert to point set
        vtkSmartPointer<vtkImageDataToPointSet> imageDataToPointSet = vtkSmartPointer<vtkImageDataToPointSet>::New();
        imageDataToPointSet->SetInputData(reader->GetOutput());
        imageDataToPointSet->Update();

        // Visualising point set
        vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
        mapper->SetInputData(imageDataToPointSet->GetOutput());
        
        // Create actor for mapper
        vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetRepresentationToWireframe();
        
        // Create renderer, add actor to it, set background
        renderer->AddActor(actor);
        
        std::cerr << "done" << std::endl;
    }
     
    
    renderer->ResetCamera();
    renderer->SetBackground(1, 1, 1);
    
    renderWindow->AddRenderer(renderer);
    
    // Create render window interactor to interact with renderer
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();
    
    // Start the window
    renderWindowInteractor->Start();
    */
    
    /*
    vtkSmartPointer<vtkImageDataToPointSet> imageDataToPointSet = vtkSmartPointer<vtkImageDataToPointSet>::New();
    imageDataToPointSet->SetInputData(reader->GetOutput());
    */
    
    // ----------------------------------------------------------------
    // create the renderer and window interactor
    // ----------------------------------------------------------------
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->SetSize(768, 400);
    
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->SetViewport(0, 0, 0.66666, 1.0);
    renderer->SetBackground(1, 1, 1);
    renderWindow->AddRenderer(renderer);
    
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderer->SetBackground(1, 1, 1);
    
    // ----------------------------------------------------------------
    // create image button to change screens
    // ----------------------------------------------------------------
    // Create two images for texture
    unsigned char green[3] = { 145,207,96 };
    unsigned char gray[3] = { 153,153,153 };
    vtkSmartPointer<vtkImageData> image1 = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> image2 = vtkSmartPointer<vtkImageData>::New();
    CreateImage(image1, green, gray);
    CreateImage(image2, gray, green);
    
    // Create the widget and its representation
    vtkSmartPointer<vtkTexturedButtonRepresentation2D> buttonRepresentation = vtkSmartPointer<vtkTexturedButtonRepresentation2D>::New();
    buttonRepresentation->SetNumberOfStates(2);
    buttonRepresentation->SetButtonTexture(0, image1);
    buttonRepresentation->SetButtonTexture(1, image2);
    
    vtkSmartPointer<vtkButtonWidget> buttonWidget = vtkSmartPointer<vtkButtonWidget>::New();
    buttonWidget->SetInteractor(renderWindowInteractor);
    buttonWidget->SetRepresentation(buttonRepresentation);
    
    
    // Place the widget. Must be done after a render so that the viewport is defined.
    // Here the widget placement is in normalized display coordinates
    vtkSmartPointer<vtkCoordinate> upperLeft = vtkSmartPointer<vtkCoordinate>::New();
    upperLeft->SetCoordinateSystemToNormalizedDisplay();
    upperLeft->SetValue(0, 1.0);
    
    double bds[6];
    double sz = 50.0;
    bds[0] = upperLeft->GetComputedDisplayValue(renderer)[0] - sz;
    bds[1] = bds[0] + sz;
    bds[2] = upperLeft->GetComputedDisplayValue(renderer)[1] - sz;
    bds[3] = bds[2] + sz;
    bds[4] = bds[5] = 0.0;
    
    // Scale to 1, default is .5
    buttonRepresentation->SetPlaceFactor(1);
    buttonRepresentation->PlaceWidget(bds);
    buttonWidget->On();
    
    // ----------------------------------------------------------------
    // read the data set
    // ----------------------------------------------------------------
    std::string file = files[0];
    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    readImageData(reader, file);
    
    // ----------------------------------------------------------------
    // Isosurface extractor and sliding bar
    // ----------------------------------------------------------------
    // read the volume data set
    vtkSmartPointer<vtkImageData> volumeData = reader->GetOutput();
    
    // extract the surface with vtkMarchingCubes
    vtkSmartPointer<vtkMarchingCubes> mc = vtkSmartPointer<vtkMarchingCubes>::New();
    mc->SetInputConnection(reader->GetOutputPort());
    mc->ComputeNormalsOn();
    mc->ComputeGradientsOn();
    mc->SetValue(0, 40); // Second value is threshold
    
    
    // apply a vtkPolyDataMapper to the output of marching cubes
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(mc->GetOutputPort());
    mapper->ScalarVisibilityOff();
    
    // create a vtkActor and assign the mapper
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->GetProperty()->SetColor(1,1,1);
    actor->SetMapper(mapper);
    
    
    // create a 2D slider
    vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
    sliderRep->SetMinimumValue(0);
    sliderRep->SetMaximumValue(255);
    sliderRep->SetValue(40);
    sliderRep->SetTitleText("Isovalue");
    // set color properties
    sliderRep->GetSliderProperty()->SetColor(0.2, 0.2, 0.6);    // Change the color of the knob that slides
    sliderRep->GetTitleProperty()->SetColor(0, 0, 0);            // Change the color of the text indicating what the slider controls
    sliderRep->GetLabelProperty()->SetColor(0, 0, 0.4);            // Change the color of the text displaying the value
    sliderRep->GetSelectedProperty()->SetColor(0.4, 0.8, 0.4);    // Change the color of the knob when the mouse is held on it
    sliderRep->GetTubeProperty()->SetColor(0.7, 0.7, 0.7);        // Change the color of the bar
    sliderRep->GetCapProperty()->SetColor(0.7, 0.7, 0.7);        // Change the color of the ends of the bar
    // set position of the slider
    sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToDisplay();
    sliderRep->GetPoint1Coordinate()->SetValue(40, 40);
    sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToDisplay();
    sliderRep->GetPoint2Coordinate()->SetValue(100, 40);
    vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidget->SetInteractor(renderWindowInteractor);
    sliderWidget->SetRepresentation(sliderRep);
    sliderWidget->SetAnimationModeToAnimate();
    
    // create the callback
    vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
    callback->Execute(mc, 0, NULL);
    sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);
    
    // get renderer for the white background and interctor style
    vtkSmartPointer<vtkRenderer> whiteRender = vtkSmartPointer<vtkRenderer>::New();
    whiteRender->SetViewport(0.66666, 0, 1, 1);
    whiteRender->SetBackground(1, 1, 1);
    
    // get mouse style for interactor
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> styleTrackball = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    
    // ----------------------------------------------------------------
    // Volume rendering and transfer function editing
    // ----------------------------------------------------------------
    // create OpenGL volume renderer
    vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper> openGLMapper = vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper>::New();
    openGLMapper->SetInputData(volumeData);
    
    // create transfer function
    vtkSmartPointer<vtkEasyTransfer> easyTransfer = vtkSmartPointer<vtkEasyTransfer>::New();
    easyTransfer->SetColormapHeat();        // set initial color map
    easyTransfer->SetColorRange(50, 255);    // set the value range that is mapped to color
    easyTransfer->SetOpacityRange(50, 255);    // set the value range that is mapped to opacity
    easyTransfer->RefreshImage();
    
    // assign transfer function to volume properties
    vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
    volumeProperty->SetColor(easyTransfer->GetColorTransferFunction());
    volumeProperty->SetScalarOpacity(easyTransfer->GetOpacityTransferFunction());
    
    // create volume actor and assign mapper and properties
    vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
    // TODO: set our openGL volume render object as mapper
    // TODO: set volumeProperty as property
    
    // get renderer for the transfer function editor or a white background
    vtkSmartPointer<vtkRenderer> transferRenderer = easyTransfer->GetRenderer();
    transferRenderer->SetViewport(0.66666, 0, 1, 1);
    
    // ----------------------------------------------------------------
    // Start screen
    // ----------------------------------------------------------------
    // create the button callback to change screen view
    vtkSmartPointer<vtkButtonCallback> callbackButton = vtkSmartPointer<vtkButtonCallback>::New();
    callbackButton->sliderWidget = sliderWidget;
    callbackButton->renderer = renderer;
    callbackButton->actor = actor;
    callbackButton->renderWindow = renderWindow;
    callbackButton->whiteRender = whiteRender;
    callbackButton->renderWindowInteractor = renderWindowInteractor;
    callbackButton->transferRenderer = transferRenderer;
    callbackButton->volume = volume;
    callbackButton->easyTransfer = easyTransfer;
    callbackButton->styleTrackball = styleTrackball;
    
    buttonWidget->AddObserver(vtkCommand::StateChangedEvent, callbackButton);
    
    // isosurface slider
    sliderWidget->EnabledOn();
    
    // add actor and renders
    renderer->AddActor(actor);
    renderWindow->AddRenderer(whiteRender);
    
    // enter the rendering loop
    renderWindow->Render();
    renderWindowInteractor->Start();
    
    return EXIT_SUCCESS;
}
///////////////////////////////////////////////////////////////////////////////////////////////






void readImageData(vtkSmartPointer<vtkXMLImageDataReader> &reader, std::string fileName) {
    std::cerr << "Reading image " << fileName << "...";
    reader->SetFileName(fileName.c_str());
    reader->Update();
    cerr << " done" << endl;
}

void CreateImage(vtkSmartPointer<vtkImageData> image, unsigned char* color1, unsigned char* color2) {
    // Specify the size of the image data
    image->SetDimensions(10, 10, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
    int* dims = image->GetDimensions();
    
    // Fill the image with
    for (int y = 0; y < dims[1]; y++) {
        for (int x = 0; x < dims[0]; x++) {
            unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
            if (x < 5)  {
                pixel[0] = color1[0];
                pixel[1] = color1[1];
                pixel[2] = color1[2];
            } else {
                pixel[0] = color2[0];
                pixel[1] = color2[1];
                pixel[2] = color2[2];
            }
        }
    }
}
