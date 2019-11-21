#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include <vtkDataSetMapper.h>
#include <vtkImageActor.h>
#include <vtkImageViewer2.h>
#include <vtkXMLImageDataReader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

static const double min = -1.5;
static const double max = 1.5;
static const double range = max-min;
static const double numColors = 100;

static const std::string files[] = {
    "../../data/cli/cli_10.vti_scaled.vti",
    "../../data/cli/cli_20.vti_scaled.vti",
    "../../data/cli/cli_30.vti_scaled.vti",
    "../../data/clw/clw_10.vti_scaled.vti",
    "../../data/clw/clw_20.vti_scaled.vti",
    "../../data/clw/clw_30.vti_scaled.vti",
    "../../data/pres/pres_10.vti_scaled.vti",
    "../../data/pres/pres_20.vti_scaled.vti",
    "../../data/pres/pres_30.vti_scaled.vti",
    "../../data/qr/qr_10.vti_scaled.vti",
    "../../data/qr/qr_20.vti_scaled.vti",
    "../../data/qr/qr_30.vti_scaled.vti",
    "../../data/ua/ua_10.vti_scaled.vti",
    "../../data/ua/ua_20.vti_scaled.vti",
    "../../data/ua/ua_30.vti_scaled.vti",
    "../../data/va/va_10.vti_scaled.vti",
    "../../data/va/va_20.vti_scaled.vti",
    "../../data/va/va_30.vti_scaled.vti",
    "../../data/wa/wa_10.vti_scaled.vti",
    "../../data/wa/wa_20.vti_scaled.vti",
    "../../data/wa/wa_30.vti_scaled.vti",
    
};

void readImageData(vtkSmartPointer<vtkXMLImageDataReader> &reader, std::string fileName);
void getColorCorrespondingTovalue(double val, double &r, double &g, double &b);

int main(int argc, char* argv[]) {
    // Verify number of input arguments
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " Filename.vti" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Reading the file
    std::string fileName = argv[1];
    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    readImageData(reader, fileName);
    
    
    
    
    // Visualising
    vtkSmartPointer<vtkDataSetMapper> mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputConnection(reader->GetOutputPort());
    
    // Create actor for mapper
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetRepresentationToWireframe();
    
    // Create renderer, add actor to it, set background
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->ResetCamera();
    renderer->SetBackground(1, 1, 1);
    
    // Create render window to show renderer
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    
    // Create render window interactor to interact with renderer
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();
    
    // Start the window
    renderWindowInteractor->Start();
    
    return EXIT_SUCCESS;
}

void readImageData(vtkSmartPointer<vtkXMLImageDataReader> &reader, std::string fileName) {
    reader->SetFileName(fileName.c_str());
    reader->Update();
}

void getColorCorrespondingTovalue(double val, double &r, double &g, double &b) {
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
