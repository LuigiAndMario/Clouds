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

// Callback for the slider interaction
class vtkSliderCallback : public vtkCommand {
public:
    static vtkSliderCallback *New() {
        return new vtkSliderCallback;
    }
    virtual void Execute(vtkObject *caller, unsigned long, void*) {
        vtkSliderWidget *sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
        if (sliceMapper)
            sliceMapper->SetSliceNumber(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());
    }
    vtkSliderCallback() : sliceMapper(0) {}
    vtkSmartPointer<vtkImageSliceMapper> sliceMapper;
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
        if (state == 0) {
            // disable time _30
            renderer->RemoveActor(imageSlices[2]);
            renderer->RemoveActor(vectorActors[2]);
            renderer->RemoveActor2D(texts[2]);
            sliderWidgets[2]->EnabledOff();

            // enable time _10
            renderer->AddActor(imageSlices[0]);
            renderer->AddActor(vectorActors[0]);
            renderer->AddActor2D(texts[0]);
            sliderWidgets[0]->EnabledOn();
        } else if (state == 1) {
            // disable time _10
            renderer->RemoveActor(imageSlices[0]);
            renderer->RemoveActor(vectorActors[0]);
            renderer->RemoveActor2D(texts[0]);
            sliderWidgets[0]->EnabledOff();

            // enable time _20
            renderer->AddActor(imageSlices[1]);
            renderer->AddActor(vectorActors[1]);
            renderer->AddActor2D(texts[1]);
            sliderWidgets[1]->EnabledOn();
        } else {
            // disable time _20
            renderer->RemoveActor(imageSlices[1]);
            renderer->RemoveActor(vectorActors[1]);
            renderer->RemoveActor2D(texts[1]);
            sliderWidgets[1]->EnabledOff();

            // enable time _30
            renderer->AddActor(imageSlices[2]);
            renderer->AddActor(vectorActors[2]);
            renderer->AddActor2D(texts[2]);
            sliderWidgets[2]->EnabledOn();
        }

    }
    vtkButtonCallback() : sliderWidgets(std::vector<vtkSmartPointer<vtkSliderWidget>>(0)), imageSlices(std::vector<vtkSmartPointer<vtkImageSlice>>(0)), vectorActors(std::vector<vtkSmartPointer<vtkActor>>(0)), renderer(0), actor(0), renderWindow(0), renderWindowInteractor(0), styleTrackball(0), texts(std::vector<vtkSmartPointer<vtkTextActor>>(0)){}
    std::vector<vtkSmartPointer<vtkSliderWidget>> sliderWidgets;
    std::vector<vtkSmartPointer<vtkImageSlice>> imageSlices;
    std::vector<vtkSmartPointer<vtkActor>> vectorActors;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> styleTrackball;
    std::vector<vtkSmartPointer<vtkTextActor>> texts;
};

static const std::string files[3][3] = {
    {
        "../../cloud_data/ua/ua_10.vti_scaled.vti",
        "../../cloud_data/va/va_10.vti_scaled.vti",
        "../../cloud_data/wa/wa_10.vti_scaled.vti",
    },
    {
        "../../cloud_data/ua/ua_20.vti_scaled.vti",
        "../../cloud_data/va/va_20.vti_scaled.vti",
        "../../cloud_data/wa/wa_20.vti_scaled.vti",
    },
    {
        "../../cloud_data/ua/ua_30.vti_scaled.vti",
        "../../cloud_data/va/va_30.vti_scaled.vti",
        "../../cloud_data/wa/wa_30.vti_scaled.vti",
    }
};

void CreateImage(vtkSmartPointer<vtkImageData> image, unsigned char* color1, unsigned char* color2, unsigned char* color3)
{
    // Specify the size of the image data
    image->SetDimensions(15, 15, 1);
    image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
    int* dims = image->GetDimensions();

    // Fill the image with
    for (int y = 0; y < dims[1]; y++) {
        for (int x = 0; x < dims[0]; x++) {
            unsigned char* pixel = static_cast<unsigned char*>(image->GetScalarPointer(x, y, 0));
            if (x < 5) {
                pixel[0] = color1[0];
                pixel[1] = color1[1];
                pixel[2] = color1[2];
            } else if (x < 10) {
                pixel[0] = color2[0];
                pixel[1] = color2[1];
                pixel[2] = color2[2];
            } else {
                pixel[0] = color3[0];
                pixel[1] = color3[1];
                pixel[2] = color3[2];
            }
        }
    }
}

double seconds(clock_t time) {
    return double(clock() - time) / CLOCKS_PER_SEC;
}

int main(int, char *[]) {
    cout << "This program will display the wind data" << endl << std::flush;
    clock_t time;
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
    /// Creating the button to select what time slot to display
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Create two images for texture
    cerr << "Creating the time button...";
    time = clock();
    unsigned char green[3] = { 145,207,96 };
    unsigned char gray[3] = { 153,153,153 };
    unsigned char blue[3] = { 89, 85, 250 };
    vtkSmartPointer<vtkImageData> image1 = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> image2 = vtkSmartPointer<vtkImageData>::New();
    vtkSmartPointer<vtkImageData> image3 = vtkSmartPointer<vtkImageData>::New();
    CreateImage(image1, green, gray, blue);
    CreateImage(image2, gray, blue, green);
    CreateImage(image3, blue, green, gray);

    // Create the widget and its representation
    vtkSmartPointer<vtkTexturedButtonRepresentation2D> buttonRepresentation = vtkSmartPointer<vtkTexturedButtonRepresentation2D>::New();
    buttonRepresentation->SetNumberOfStates(3);
    buttonRepresentation->SetButtonTexture(0, image1);
    buttonRepresentation->SetButtonTexture(1, image2);
    buttonRepresentation->SetButtonTexture(2, image3);

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
    cerr << " done (" << seconds(time) << " s)" << endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the images
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<vtkSmartPointer<vtkImageData>> vector_fields(3);
    // For each timestep
    for (int i = 0; i < vector_fields.size(); i++) {
        vtkSmartPointer<vtkImageData> vector_field = vtkSmartPointer<vtkImageData>::New();

        // For each component
        for (int j = 0; j < 3; j++) {
            std::string file = files[i][j];
            cerr << "Reading vector field component from " << file << "...";
            time = clock();
            vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
            reader->SetFileName(file.c_str());
            reader->Update();

            // Get the read data
            vtkSmartPointer<vtkImageData> data = vtkSmartPointer<vtkImageData>::New();
            data = reader->GetOutput();
            data->GetPointData()->SetActiveScalars(data->GetPointData()->GetArrayName(0));

            // Get dimensions
            int dims[3];
            data->GetDimensions(dims);

            const int downsampling_rate = 128;
            for (int d = 0; d < 3; d++) {
                dims[d] /= downsampling_rate;
            }

            // Set dimension equal to the data (only if hasn't been done yet)
            if (j == 0) {
                vector_field->SetDimensions(dims);
                vector_field->AllocateScalars(VTK_FLOAT, 3);
            }

            // Copy data from read file to appropriate component in the vector field
            for (int x = 0; x < dims[0]; x+=downsampling_rate){
                for (int y = 0; y < dims[1]; y+=downsampling_rate) {
                    for (int z = 0; z < dims[2]; z+=downsampling_rate) {
                        float component_value = data->GetScalarComponentAsFloat(x, y, z, 0);
                        vector_field->SetScalarComponentFromFloat(x, y, z, j, component_value);
                    }
                }
            }
            cerr << " done (" << seconds(time) << " s)" << endl;
        }

        vector_field->GetPointData()->SetActiveVectors(vector_field->GetPointData()->GetScalars()->GetName());
        vector_fields[i] = vector_field;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating glyph filters
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Creating the glyph filters...";
    time = clock();
    std::vector<vtkSmartPointer<vtkGlyph2D>> glyphFilters(3);
    for (int i = 0; i < 3; i++) {
      // Setup the arrows
      vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();
      arrowSource->Update();

      vtkSmartPointer<vtkGlyph2D> glyphFilter = vtkSmartPointer<vtkGlyph2D>::New();
      glyphFilter->SetSourceConnection(arrowSource->GetOutputPort());
      glyphFilter->OrientOn();
      glyphFilter->SetVectorModeToUseVector();
      glyphFilter->SetInputData(vector_fields[i]);
      glyphFilter->Update();

      glyphFilters[i] = glyphFilter;
    }
    cerr << " done (" << seconds(time) << " s)" << endl;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Slicing
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Slicing...";
    time = clock();
    std::vector<vtkSmartPointer<vtkImageSliceMapper>> imageSliceMappers(3);
    std::vector<vtkSmartPointer<vtkImageSlice>> imageSlices(3);
    std::vector<vtkSmartPointer<vtkPolyDataMapper>> vectorMappers(3);
    for (int i = 0 ; i < 3 ; i++) {
      vtkSmartPointer<vtkImageSliceMapper> imageMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
      imageMapper->SetInputData(vector_fields[i]);
      imageSliceMappers[i] = imageMapper;

      vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
      imageSlice->SetMapper(imageMapper);
      imageSlices[i] = imageSlice;

      vtkSmartPointer<vtkPolyDataMapper> vectorMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      vectorMapper->SetInputConnection(glyphFilters[i]->GetOutputPort());
      vectorMappers[i] = vectorMapper;
    }
    cerr << " done (" << seconds(time) << " s)" << endl;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the slice sliders
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Creating the sliders...";
    time = clock();
    std::vector<vtkSmartPointer<vtkSliderRepresentation2D>> sliderReps(3);
    std::vector<vtkSmartPointer<vtkSliderWidget>> sliderWidgets(3);
    for (int i = 0 ; i < 3 ; i++) {
        vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
        sliderRep->SetMinimumValue(imageSliceMappers[i]->GetSliceNumberMinValue());
        sliderRep->SetMaximumValue(imageSliceMappers[i]->GetSliceNumberMaxValue());
        sliderRep->SetValue(1); // Starting with the same nice slice as before
        sliderRep->SetTitleText("Slice selection");
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
        sliderRep->GetPoint2Coordinate()->SetValue(190, 40);

        sliderReps[i] = sliderRep;

        vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
        sliderWidget->SetInteractor(renderWindowInteractor);
        sliderWidget->SetRepresentation(sliderRep);
        sliderWidget->SetAnimationModeToAnimate();

        // create the callback
        vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
        callback->sliceMapper = imageSliceMappers[i];
        sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);

        sliderWidgets[i] = sliderWidget;
    }
    sliderWidgets[0]->EnabledOn();
    
    cerr << " done (" << seconds(time) << " s)" << endl;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Setting up the text
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Setup the text and add it to the renderer
    cerr << "Setting up the text...";
    time = clock();
    std::vector<vtkSmartPointer<vtkTextActor>> textActors(3);
    for (int i = 0 ; i < 3 ; i++) {
        textActors[i] = vtkSmartPointer<vtkTextActor>::New();
        if (i == 0) textActors[i]->SetInput("First timeframe");
        else if (i == 1) textActors[i]->SetInput("Second timeframe");
        else textActors[i]->SetInput("Third timeframe");
        textActors[i]->SetPosition(230, 40);
        textActors[i]->GetTextProperty()->SetFontSize (24);
        textActors[i]->GetTextProperty()->SetColor (1.0, 0.0, 0.0);
    }
    cerr << " done (" << seconds(time) << " s)" << endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Setting up the vector actors
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Setting up the vector actors...";
    time = clock();
    std::vector<vtkSmartPointer<vtkActor>> vectorActors(3);
    for (int i = 0; i < 3; i++) {
      // Create actors
      vtkSmartPointer<vtkActor> vectorActor = vtkSmartPointer<vtkActor>::New();
      vectorActor->SetMapper(vectorMappers[i]);
    }
    cerr << " done (" << seconds(time) << " s)" << endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Setting up the renderers
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Setting up the renderers...";
    time = clock();
    renderer->AddActor(imageSlices[0]);
    renderer->AddActor(vectorActors[0]);
    renderer->AddActor2D(textActors[0]);
    renderer->ResetCamera();
    renderer->SetBackground(1, 1, 1);

    // Setup render window
    renderWindow->AddRenderer(renderer);

    // Setup render window interactor
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);
    cerr << " done (" << seconds(time) << " s)" << endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Enabling the time button
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vtkSmartPointer<vtkButtonCallback> callbackButton = vtkSmartPointer<vtkButtonCallback>::New();
    callbackButton->imageSlices = imageSlices;
    callbackButton->vectorActors = vectorActors;
    callbackButton->sliderWidgets = sliderWidgets;
    callbackButton->renderer = renderer;
    callbackButton->actor = renderer->GetActors()->GetLastActor();
    callbackButton->renderWindow = renderWindow;
    callbackButton->renderWindowInteractor = renderWindowInteractor;
    callbackButton->styleTrackball = style;
    callbackButton->texts = textActors;

    buttonWidget->AddObserver(vtkCommand::StateChangedEvent, callbackButton);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
