#ifndef PARAVIEWHEADERS_HPP
#define PARAVIEWHEADERS_HPP

//Base objects
#include <vtkDataObject.h>
#include <vtkDataSet.h>

//Mpi
#include <vtkMPI.h>
#include <vtkMPICommunicator.h>

//CoProcessing
#include <vtkCPDataDescription.h>
#include <vtkCPInputDataDescription.h>
#include <vtkCPProcessor.h>
#include <vtkCPPythonScriptPipeline.h>

//Grid types
#include <vtkImageData.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiPieceDataSet.h>
#include <vtkUnstructuredGrid.h>

//Data Arrays
#include <vtkShortArray.h>
#include <vtkLongArray.h>
#include <vtkIntArray.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkSOADataArrayTemplate.h>

//Misc.
#include <vtkNew.h>
#include <vtkCellType.h>
#include <vtkPoints.h>

#endif // PARAVIEWHEADERS_HPP
