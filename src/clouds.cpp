#define DEBUG (true)

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

#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkImageProperty.h>

#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>

// Slider
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkProperty2D.h>
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
        {0.6980, 0.0941, 0.1686}, // Red
        {0.8392, 0.3765, 0.3020},
        {0.9569, 0.6471, 0.5098},
        {0.9922, 0.8588, 0.7804},
        {0.9686, 0.9686, 0.9686}, // White
        {0.8196, 0.8980, 0.9412},
        {0.5725, 0.7725, 0.8706},
        {0.2627, 0.5765, 0.7647},
        {0.1294, 0.4000, 0.6745}  // Blue
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
    std::string file = files[0];
    cerr << "Reading file " << file << "...";
    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    reader->SetFileName(file.c_str());
    reader->Update();
    cerr << " done" << endl;

    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the image and lookup table
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Creating the image and lookup table...";
    vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
    image = reader->GetOutput();
    image->AllocateScalars(VTK_FLOAT, 3);

    // Get image values range
    if (DEBUG) {
        cerr << endl;
        image->Print(cerr);
    }
    float valuesRange[2];
    /// WARNING: Following only works for .cli files
    vtkFloatArray::SafeDownCast(image->GetPointData()->GetAbstractArray("cli"))->GetValueRange(valuesRange);

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
    lookupTable->Build();
    cerr << " done" << endl;

    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Slicing
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Slicing...";
    vtkSmartPointer<vtkImageSliceMapper> imageSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
    imageSliceMapper->SetInputData(image);
    imageSliceMapper->SetSliceNumber(74); // Starting with a nice slice
    imageSliceMapper->Update();
    if (DEBUG) {
        cerr << endl;
        imageSliceMapper->Print(cerr);
    }
    
    vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
    imageSlice->SetMapper(imageSliceMapper);
    imageSlice->GetProperty()->SetLookupTable(lookupTable);
//    imageSlice->ForceTranslucentOn();
    imageSlice->Update();
    cerr << " done" << endl;
    

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Creating the slider
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Creating the slider...";
    vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
    sliderRep->SetMinimumValue(imageSliceMapper->GetSliceNumberMinValue());
    sliderRep->SetMaximumValue(imageSliceMapper->GetSliceNumberMaxValue());
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
    sliderRep->GetPoint2Coordinate()->SetValue(240, 40);
    vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidget->SetInteractor(renderWindowInteractor);
    sliderWidget->SetRepresentation(sliderRep);
    sliderWidget->SetAnimationModeToAnimate();
    
    // create the callback
    vtkSmartPointer<vtkSliderCallback> callback = vtkSmartPointer<vtkSliderCallback>::New();
    callback->sliceMapper = imageSliceMapper;
    sliderWidget->AddObserver(vtkCommand::InteractionEvent, callback);
    
    sliderWidget->EnabledOn();
    cerr << " done" << endl;
    
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Setting up the renderers
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Setting up the renderers...";
    renderer->AddViewProp(imageSlice);
    renderer->AddActor(imageSlice);
    renderer->ResetCamera();
    renderer->SetBackground(1, 1, 1);

    // Setup render window
    renderWindow->AddRenderer(renderer);

    // Setup render window interactor
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);
    cerr << " done" << endl;

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
