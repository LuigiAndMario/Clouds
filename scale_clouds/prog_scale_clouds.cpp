#include "vtkXMLImageDataReader.h"
#include "vtkXMLImageDataWriter.h"
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include <string> 

int main(int argc, char* argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		std::string pathIn = argv[i];
		std::string pathOut = std::string(argv[i]) + "_scaled.vti";

		std::cout << "processing " << pathIn << std::endl;
		// Read the data set
		std::cout << "\treading file..." << std::flush;
		vtkSmartPointer<vtkXMLImageDataReader> reader =
			vtkSmartPointer<vtkXMLImageDataReader>::New();
		reader->SetFileName(pathIn.c_str());
		reader->Update();
		std::cout << "done" << std::endl;

		// Scale the z-spacing
		std::cout << "\tscaling data..." << std::flush;
		vtkImageData* data = reader->GetOutput();
		double* bounds = data->GetSpacing();
		bounds[2] *= 0.0001;		// scale the z spacing down
		data->SetSpacing(bounds);
		std::cout << "done" << std::endl;
		
		// Write the data
		std::cout << "\twriting file..." << std::flush;
		vtkSmartPointer<vtkXMLImageDataWriter> writer =
			vtkSmartPointer<vtkXMLImageDataWriter>::New();
		writer->SetFileName(pathOut.c_str());
		writer->SetInputData(data);
		writer->Update();
		std::cout << "done" << std::endl;
	}
	return 0;
}
