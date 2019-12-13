// VTK general
#include <vtkVersion.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>

#include <vtkImageResize.h>

#include <vtkDoubleArray.h>
#include <vtkImageShiftScale.h>

static const std::string in_files[3][3] = {
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

static const std::string out_files[3] = {
    "../../cloud_data/winds/winds_10.vti_scaled.vti",
    "../../cloud_data/winds/winds_20.vti_scaled.vti",
    "../../cloud_data/winds/winds_30.vti_scaled.vti"
};

/*
static const std::string in_files[3][3] = {
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

static const std::string out_files[3] = {
    "../../cloud_data/winds/winds_10.vti_scaled.vti",
    "../../cloud_data/winds/winds_20.vti_scaled.vti",
    "../../cloud_data/winds/winds_30.vti_scaled.vti"
};
*/

double seconds(clock_t time) {
    return double(clock() - time) / CLOCKS_PER_SEC;
}

int main(int, char *[]) {
    cout << "This program will create a downsampled .vti_scaled.vti file containing the wind data (extracted from the ua, va and wa files)" << endl << std::flush;
    clock_t time;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Reading the images
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // For each timestep
    for (int i = 0; i < 3; i++) {
        vtkSmartPointer<vtkImageData> vector_field = vtkSmartPointer<vtkImageData>::New();

        // Read in vector field values
        vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
        vtkSmartPointer<vtkImageData> data = vtkSmartPointer<vtkImageData>::New();

        // For each component
        int NUM_COMPONENTS = 3;
        for (int j = 0; j < NUM_COMPONENTS; j++) {
            std::string in_file = in_files[i][j];
            cerr << "Reading vector field component from " << in_file << "...";
            time = clock();
            reader->SetFileName(in_file.c_str());
            reader->Update();

            // Get the read data
            data = reader->GetOutput();
            data->GetPointData()->SetActiveScalars(data->GetPointData()->GetArrayName(0));

            // Get dimensions
            int dims[3];
            data->GetDimensions(dims);

            const int downsampling_rate = 2;
            for (int d = 0; d < 3; d++) {
                dims[d] /= downsampling_rate;
            }

            // Set dimension equal to the data (only if hasn't been done yet)
            if (j == 0) {
                vector_field->SetDimensions(dims);
                vector_field->AllocateScalars(VTK_FLOAT, NUM_COMPONENTS);
            }
            
            // Getting the dimensions
            float valuesRange[2];
            vtkFloatArray::SafeDownCast(data->GetPointData()->GetAbstractArray(data->GetPointData()->GetArrayName(0)))
            ->GetValueRange(valuesRange);

            // Copy data from read file to appropriate component in the vector field
            for (int x = 0; x < dims[0]; x++){
                for (int y = 0; y < dims[1]; y++) {
                    for (int z = 0; z < dims[2]; z++) {
                        float component_value = data->GetScalarComponentAsFloat(
                            x * downsampling_rate,
                            y * downsampling_rate,
                            z * downsampling_rate,
                            0);
                        component_value -= valuesRange[0];
                        component_value /= (valuesRange[1] - valuesRange[0]);
                        vector_field->SetScalarComponentFromFloat(x, y, z, j, component_value);
                    }
                }
            }
            cerr << " done (" << seconds(time) << " s)" << endl;
        }

        // Write resulting vector field to a file
        vtkSmartPointer<vtkXMLImageDataWriter> writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
        std::string out_file = out_files[i];
        cerr << "Writing vector field to " << out_file << "...";
        time = clock();
        writer->SetFileName(out_file.c_str());
        writer->SetInputData(vector_field);
        writer->Write();
        cerr << " done (" << seconds(time) << " s)" << endl;
    }

    return EXIT_SUCCESS;
}
