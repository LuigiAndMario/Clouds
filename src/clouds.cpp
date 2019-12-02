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
#include <vtkImageResliceMapper.h>
#include <vtkImageSlice.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

#include <vtkSphereSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <vtkImageResize.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>

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
		0.6980, 0.0941, 0.1686, // Red
		0.8392, 0.3765, 0.3020,
		0.9569, 0.6471, 0.5098,
		0.9922, 0.8588, 0.7804,
		0.9686, 0.9686, 0.9686, // White
		0.8196, 0.8980, 0.9412,
		0.5725, 0.7725, 0.8706,
		0.2627, 0.5765, 0.7647,
		0.1294, 0.4000, 0.6745  // Blue
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
    // Read file
    std::cerr << "Reading file " << files[0] << "...";
    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    reader->SetFileName(files[0].c_str());
    reader->Update();
    std::cerr << " done" << std::endl;

    vtkSmartPointer<vtkImageData> image = vtkSmartPointer<vtkImageData>::New();
    image = reader->GetOutput();
    //image->AllocateScalars(VTK_FLOAT, 3);

    // Get image values range
    image->Print(std::cerr);
    float valuesRange[2];
    vtkFloatArray::SafeDownCast(image->GetPointData()->GetAbstractArray("cli"))->GetValueRange(valuesRange);

    double min = valuesRange[0];
    double max = valuesRange[1];
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

    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(reader->GetOutput());
    mapper->SetLookupTable(lookupTable);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetRepresentationToWireframe();

    /*
    std::cerr << "Slicing...";
    vtkSmartPointer<vtkImageSliceMapper> imageSliceMapper = vtkSmartPointer<vtkImageSliceMapper>::New();
    imageSliceMapper->SetInputData(image);
    imageSliceMapper->SetLookupTable(lookupTable);
    imageSliceMapper->Update();
//    imageSliceMapper->SetSliceNumber(74); // Trying out some specific slice
//    imageSliceMapper->Print(std::cerr);
    
    vtkSmartPointer<vtkImageSlice> imageSlice = vtkSmartPointer<vtkImageSlice>::New();
    imageSlice->SetMapper(imageSliceMapper);
    imageSlice->Update();
    std::cerr << " done" << std::endl;
    */
    

    // Setup renderers
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
//    renderer->AddViewProp(imageSlice);
    // renderer->AddActor(imageSlice);
    renderer->AddActor(actor);
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

    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetInteractorStyle(style);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
