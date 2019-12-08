#define DEBUG (false)

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
            renderer->RemoveActor2D(texts[2]);
            sliderWidgets[2]->EnabledOff();
            
            // enable time _10
            renderer->AddActor(imageSlices[0]);
            renderer->AddActor2D(texts[0]);
            sliderWidgets[0]->EnabledOn();
        } else if (state == 1) {
            // disable time _10
            renderer->RemoveActor(imageSlices[0]);
            renderer->RemoveActor2D(texts[0]);
            sliderWidgets[0]->EnabledOff();
            
            // enable time _20
            renderer->AddActor(imageSlices[1]);
            renderer->AddActor2D(texts[1]);
            sliderWidgets[1]->EnabledOn();
        } else {
            // disable time _20
            renderer->RemoveActor(imageSlices[1]);
            renderer->RemoveActor2D(texts[1]);
            sliderWidgets[1]->EnabledOff();
            
            // enable time _30
            renderer->AddActor(imageSlices[2]);
            renderer->AddActor2D(texts[2]);
            sliderWidgets[2]->EnabledOn();
        }
        
    }
    vtkButtonCallback() : sliderWidgets(std::vector<vtkSmartPointer<vtkSliderWidget>>(0)), imageSlices(std::vector<vtkSmartPointer<vtkImageSlice>>(0)), renderer(0), actor(0), renderWindow(0), renderWindowInteractor(0), styleTrackball(0), texts(std::vector<vtkSmartPointer<vtkTextActor>>(0)){}
    std::vector<vtkSmartPointer<vtkSliderWidget>> sliderWidgets;
    std::vector<vtkSmartPointer<vtkImageSlice>> imageSlices;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkRenderWindow> renderWindow;
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> styleTrackball;
    std::vector<vtkSmartPointer<vtkTextActor>> texts;
};

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


void getColorCorrespondingToValue(double min, double max, double range, double numColors,
                                  double val, double &r, double &g, double &b) {
    static const int numColorNodes = 9;
	double color[numColorNodes][3] = {
        {0.9373, 0.9529, 1.0000}, // White
        {0.7765, 0.8588, 0.9373},
        {0.6196, 0.7922, 0.8824},
        {0.4196, 0.6824, 0.8392},
        {0.1922, 0.5098, 0.7412},
        {0.0314, 0.3176, 0.6118} // Blue
	};

	for (int i = 0; i < (numColorNodes - 1); i++) {
		double currFloor = min + ((double)i / (numColorNodes - 1)) * range;
		double currCeil = min + ((double)(i + 1) / (numColorNodes - 1)) * range;

		if ((val >= currFloor) && (val <= currCeil)) {
			double currFraction = (val - currFloor) / (currCeil - currFloor);
			r = color[i][0] * (1.0 - currFraction) + color[i + 1][0] * currFraction;
			g = color[i][1] * (1.0 - currFraction) + color[i + 1][1] * currFraction;
			b = color[i][2] * (1.0 - currFraction) + color[i + 1][2] * currFraction;
		}
	}
}

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

int main(int, char *[]) {
    cout << "This program will display the cloud water data" << endl << std::flush;
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
    cerr << " done" << endl;
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Reading the files
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<vtkSmartPointer<vtkXMLImageDataReader>> readers(3);
    for (int i = 3 ; i < 6 ; i++) {
        std::string file = files[i];
        cerr << "Reading file " << file << "...";
        vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
        reader->SetFileName(file.c_str());
        reader->Update();
        
        readers[i - 3] = reader;
        cerr << " done" << endl;
    }
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the images and lookup tables
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Creating the images and lookup table...";
    std::vector<vtkSmartPointer<vtkImageData>> images(3);
    std::vector<vtkSmartPointer<vtkLookupTable>> lookupTables(3);
    for (int i = 0 ; i < 3 ; i++) {
        vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
        image = readers[i]->GetOutput();
        image->GetPointData()->SetActiveScalars("clw");
        
        if (DEBUG) {
            cerr << endl;
            image->Print(cerr);
        }
        images[i] = image;

        // Get image values range
        float valuesRange[2];
        /// WARNING: Following only works for .clw files
        vtkFloatArray::SafeDownCast(image->GetPointData()->GetAbstractArray("clw"))->GetValueRange(valuesRange);

        double min = valuesRange[0];
        double max = valuesRange[1];
        if (DEBUG) {
            cerr << "min = " << min << ", max = " << max << endl;
        }
        double range = max-min;
        double numColors = 100;

        // Create color lookup table
        vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
        lookupTable->SetScaleToLinear();
        lookupTable->SetNumberOfTableValues(numColors);
        double r, g, b;
        for (int i = 0; i < numColors; i++) {
            double val = min + ((double) i / numColors) * range;
            getColorCorrespondingToValue(min, max, range, numColors, val, r, g, b);
            lookupTable->SetTableValue(i, r, g, b);
        }
        lookupTable->SetRange(min, max);
        lookupTable->Build();
        
        if (DEBUG) {
            cerr << endl << endl;
            lookupTable->Print(cerr);
        }
        lookupTables[i] = lookupTable;
    }
    cerr << " done" << endl;

    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Slicing
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Slicing...";
    std::vector<vtkSmartPointer<vtkImageSliceMapper>> imageSliceMappers(3);
    std::vector<vtkSmartPointer<vtkImageSlice>> imageSlices(3);
    for (int i = 0 ; i < 3 ; i++) {
        vtkSmartPointer<vtkImageSliceMapper> imageSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
        imageSliceMapper->SetInputData(images[i]);
        imageSliceMapper->SetSliceNumber(74); // Starting with a nice slice
        imageSliceMapper->Update();
        
        if (DEBUG) {
            cerr << endl;
            imageSliceMapper->Print(cerr);
        }
        imageSliceMappers[i] = imageSliceMapper;
        
        vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
        imageSlice->SetMapper(imageSliceMapper);
        imageSlice->GetProperty()->SetLookupTable(lookupTables[i]);
        imageSlice->GetProperty()->UseLookupTableScalarRangeOn();
        imageSlice->Update();
        
        if (DEBUG) {
            cerr << endl;
            imageSlice->Print(cerr);
        }
        imageSlices[i] = imageSlice;
    }
    cerr << " done" << endl;
    

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the slice sliders
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Creating the sliders...";
    std::vector<vtkSmartPointer<vtkSliderRepresentation2D>> sliderReps(3);
    std::vector<vtkSmartPointer<vtkSliderWidget>> sliderWidgets(3);
    for (int i = 0 ; i < 3 ; i++) {
        vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
        sliderRep->SetMinimumValue(imageSliceMappers[i]->GetSliceNumberMinValue());
        sliderRep->SetMaximumValue(imageSliceMappers[i]->GetSliceNumberMaxValue());
        sliderRep->SetValue(74); // Starting with the same nice slice as before
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
    
    cerr << " done" << endl;
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Setting up the text
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Setup the text and add it to the renderer
    cerr << "Setting up the text...";
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
    cerr << " done" << endl;
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Setting up the renderers
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Setting up the renderers...";
    renderer->AddViewProp(imageSlices[0]);
    renderer->AddActor(imageSlices[0]);
    renderer->AddActor2D(textActors[0]);
    renderer->ResetCamera();
    renderer->SetBackground(1, 1, 1);

    // Setup render window
    renderWindow->AddRenderer(renderer);

    // Setup render window interactor
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);
    cerr << " done" << endl;
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Enabling the time button
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    vtkSmartPointer<vtkButtonCallback> callbackButton = vtkSmartPointer<vtkButtonCallback>::New();
    callbackButton->imageSlices = imageSlices;
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
