#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLegendBoxActor.h>
#include <vtkCubeSource.h>

#include "vtkhelper.hpp"
#include "math.hpp"
#include "flow.hpp"

typedef vtkSmartPointer<vtkActor> vtkActorPtr;

// create a simple test vector field
class center : public vfield2d {
public:
	virtual vec2f sample(const vec2f& pos) const
	{ 
		return vec2f(-pos.y(), pos.x());	// returns a center flow
	}
	vec2f dmin() const { return vec2f(-8, -8); }	// min corner of the domain
	vec2f dmax() const { return vec2f(8, 8); }		// max corner of the domain
};

int main(int, char *[])
{
	// create the vector field
	center flow;

	// ----------------------------------------------------------------
	// streamline tracing experiments
	// ----------------------------------------------------------------

	// set parameters
	vec2f seed(1, 0);		// seed point
	float dt = 1;			// integration step size
	float tau = 2 * PI;		// integration duration

	// trace streamlines (tangent curves of a steady vector field)
	std::vector<vec2f> streamlineEuler, streamlineRK2, streamlineRK4;
	flow.tangentCurve(seed, dt, tau, Integrator_Euler, streamlineEuler);
	flow.tangentCurve(seed, dt, tau, Integrator_RK2, streamlineRK2);
	flow.tangentCurve(seed, dt, tau, Integrator_RK4, streamlineRK4);
	
	// create a VTK actor for each streamline
	vtkActorPtr actorEuler = createLineActor(streamlineEuler, vec3f(0.8f, 0.2f, 0.2f));
	vtkActorPtr actorRK2 = createLineActor(streamlineRK2, vec3f(0.2f, 0.8f, 0.2f));
	vtkActorPtr actorRK4 = createLineActor(streamlineRK4, vec3f(0.2f, 0.2f, 0.8f));

	// ----------------------------------------------------------------
	// create a legend
	// ----------------------------------------------------------------

	vtkSmartPointer<vtkLegendBoxActor> legend = vtkSmartPointer<vtkLegendBoxActor>::New();
	legend->SetNumberOfEntries(3);
	legend->BorderOff();
	// add the entries
	vtkSmartPointer<vtkCubeSource> legendBox = vtkSmartPointer<vtkCubeSource>::New();
	legendBox->Update();
	double clrEuler[] = { 0.8, 0.2, 0.2 };
	double clrRK2[] = { 0.2, 0.8, 0.2 };
	double clrRK4[] = { 0.2, 0.2, 0.8 };
	legend->SetEntry(0, legendBox->GetOutput(), "Euler", clrEuler);
	legend->SetEntry(1, legendBox->GetOutput(), "RK2", clrRK2);
	legend->SetEntry(2, legendBox->GetOutput(), "RK4", clrRK4);
	// place legend in lower right
	legend->GetPositionCoordinate()->SetCoordinateSystemToView();
	legend->GetPositionCoordinate()->SetValue(.5, -1.0);
	legend->GetPosition2Coordinate()->SetCoordinateSystemToView();
	legend->GetPosition2Coordinate()->SetValue(1.0, -0.5);

	// ----------------------------------------------------------------
	// fill with background streamlines
	// ----------------------------------------------------------------
	const int numLines = 10;
	std::vector<vtkActorPtr> lineActors;
	for (int i = 0; i < numLines; ++i)
	{
		std::vector<vec2f> streamline;
		float dt = 0.1f;
		vec2f seed(i / (numLines - 1.0f) * flow.dmax().x(), 0);
		flow.tangentCurve(seed, dt, tau, Integrator_RK4, streamline);
		vtkActorPtr actor = createLineActor(streamline, vec3f(0.7f, 0.7f, 0.7f));
		lineActors.push_back(actor);
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
	for (auto& actor : lineActors)
		renderer->AddActor(actor);
	renderer->AddActor(actorEuler);
	renderer->AddActor(actorRK2);
	renderer->AddActor(actorRK4);
	renderer->AddActor(legend);
	// ---------
	renderWindow->Render();
	renderWindowInteractor->Start();

	return EXIT_SUCCESS;
}