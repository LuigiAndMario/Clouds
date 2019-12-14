#define DEBUG (true)

// STL
#include <vector>

// VTK general
#include <vtkVersion.h>
#include <vtkXMLImageDataReader.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkFloatArray.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRenderer.h>

// Colour
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkImageProperty.h>

// Button
#include <vtkButtonWidget.h>
#include <vtkTexturedButtonRepresentation2D.h>

// Text
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

// RK4 integrator
#include "vtkhelper.hpp"
#include "math.hpp"
#include "flow.hpp"

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
            for (auto& actor : streamlineActors[2])
                renderer->RemoveActor(actor);
            renderer->RemoveActor2D(texts[2]);

            // enable time _10
            for (auto& actor : streamlineActors[0])
                renderer->AddActor(actor);
            renderer->AddActor2D(texts[0]);
        } else if (state == 1) {
            // disable time _10
            for (auto& actor : streamlineActors[0])
                renderer->RemoveActor(actor);
            renderer->RemoveActor2D(texts[0]);

            // enable time _20
            for (auto& actor : streamlineActors[1])
                renderer->AddActor(actor);
            renderer->AddActor2D(texts[1]);
        } else {
            // disable time _20
            for (auto& actor : streamlineActors[1])
                renderer->RemoveActor(actor);
            renderer->RemoveActor2D(texts[1]);

            // enable time _30
            for (auto& actor : streamlineActors[2])
                renderer->AddActor(actor);
            renderer->AddActor2D(texts[2]);
        }

    }
        vtkButtonCallback() : streamlineActors(std::vector<std::vector<vtkSmartPointer<vtkActor>>>(0)),
                              renderer(0),
                              texts(std::vector<vtkSmartPointer<vtkTextActor>>(0)){}
    std::vector<std::vector<vtkSmartPointer<vtkActor>>> streamlineActors;
    vtkSmartPointer<vtkRenderer> renderer;
    std::vector<vtkSmartPointer<vtkTextActor>> texts;
};

static const std::string files[3] = {
        "../../cloud_data/winds/winds_10.vti_scaled.vti",
        "../../cloud_data/winds/winds_20.vti_scaled.vti",
        "../../cloud_data/winds/winds_30.vti_scaled.vti"
};

class vtk_vf : public vfield3d {
    private:
        vtkSmartPointer<vtkImageData> vector_field;
    public:
        vtk_vf(vtkSmartPointer<vtkImageData> vf) {
            vector_field = vf;
        }

        virtual vec3f sample(const vec3f& pos) const
        {
            float u = vector_field->GetScalarComponentAsFloat(pos.x(), pos.y(), pos.z(), 0);
            float v = vector_field->GetScalarComponentAsFloat(pos.x(), pos.y(), pos.z(), 1);
            float w = vector_field->GetScalarComponentAsFloat(pos.x(), pos.y(), pos.z(), 2);
            return vec3f(u, v, w);
        }


        virtual void getDimensions(int dims[3])
        {
            vector_field->GetDimensions(dims);
        }

	    // traces a tangent curve and returns all particle positions
        void tangentCurve(vec3f pos, float dt, float tau, EIntegrator integrator, std::vector<vec3f>& curve) const
        {
            curve.clear();
            curve.push_back(pos);
            for (float i = 0; i < tau; i += dt) {
                // Check that position is still within the bounds of the vector field
                int extent[6];
                vector_field->GetExtent(extent);
                if (extent[0] <= pos.x() && pos.x() <= extent[1] &&
                    extent[2] <= pos.y() && pos.y() <= extent[3] &&
                    extent[4] <= pos.z() && pos.z() <= extent[5] ) {
                    pos = step(pos, dt, integrator);
                    curve.push_back(pos);
                } else {
                    break;
                }
            }
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
    /// Creating the vector fields and computing stream lines
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::vector<vtkSmartPointer<vtkActor>>> streamlineActors(3);
    for (int i = 0; i < streamlineActors.size(); i++) {
        vtk_vf wind_vf(vector_fields[i]);

        std::vector<vtkSmartPointer<vtkActor>> streamlines;
        int dims[3];
        wind_vf.getDimensions(dims);
        for (int x = 0; x < dims[0]; x++) {
            for (int y = 0; y < dims[1]; y++) {
                // set parameters
                vec3f seed(x, y, 0);		// seed point
                float dt = 1;			// integration step size
                float tau = 200;		// integration duration

                std::vector<vec3f> streamlineRK4;
                wind_vf.tangentCurve(seed, dt, tau, Integrator_RK4, streamlineRK4);

                vtkSmartPointer<vtkActor> streamlineActor = createLineActor(streamlineRK4, vec3f(0.2f, 0.2f, 0.8f));
                streamlines.push_back(streamlineActor);
            }
        }

        streamlineActors[i] = streamlines;
    }


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
    /// Setting up the renderers
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    cerr << "Setting up the renderers...";
    time = clock();
	for (auto& actor : streamlineActors[0]) {
		renderer->AddActor(actor);
    }
    renderer->AddActor2D(textActors[0]);
    renderer->ResetCamera();
//    renderer->SetBackground(1, 1, 1);

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
    callbackButton->streamlineActors = streamlineActors;
    callbackButton->renderer = renderer;
    callbackButton->texts = textActors;

    buttonWidget->AddObserver(vtkCommand::StateChangedEvent, callbackButton);

    // Render and start interaction
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->Initialize();

    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
