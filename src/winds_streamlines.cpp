#define DEBUG (true)

#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>
#include <vtkPointData.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLegendBoxActor.h>
#include <vtkCubeSource.h>

#include "vtkhelper.hpp"
#include "math.hpp"
#include "flow.hpp"

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

double seconds(clock_t time) {
    return double(clock() - time) / CLOCKS_PER_SEC;
}

int main(int, char *[]) {
    cout << "This program will display the wind streamlines" << endl << std::flush;
    clock_t time;

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
    std::vector<std::vector<vtkSmartPointer<vtkActor>>> streamline_actors(1);
    for (int i = 0; i < streamline_actors.size(); i++) {
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

        streamline_actors[i] = streamlines;
    }

	// ----------------------------------------------------------------
	// create the renderer and add actors
	// ----------------------------------------------------------------

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	renderer->SetBackground(1,1,1);

	// ---------
	for (auto& actor : streamline_actors[0]) {
		renderer->AddActor(actor);
    }

	// ---------
	renderWindow->Render();
	renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
