
#--------------------------------------------------------------

# Global timestep output options
timeStepToStartOutputAt=1
forceOutputAtFirstCall=False

# Global screenshot output options
imageFileNamePadding=3
rescale_lookuptable=False

# Whether or not to request specific arrays from the adaptor.
requestSpecificArrays=False

# a root directory under which all Catalyst output goes
# /home/jbowden/C_S/saturne_examples/case_mesh_3d_64/PARAVIEW_EXPORTS
import os
import sys
catalyst_output_dir = os.environ.get('CATALYST_OUTPUT_DIR')
if catalyst_output_dir == 'None':
  print('WARNING: Catalyst Coprocessing Script: '+os.path.basename(__file__))
  print(' The CATALYST_OUTPUT_DIR environment variable was not set!')
  catalyst_output_dir='/home/jbowden/C_S/saturne_examples/case_mesh_3d_64/PARAVIEW_EXPORTS'
  print(' The CATALYST output directed to: ' + catalyst_output_dir)
  # sys.exit('exiting due to no CATALYST_OUTPUT_DIR env variable')
catalyst_output_dir='/home/jbowden/C_S/saturne_examples/case_mesh_3d_64/PARAVIEW_EXPORTS'
rootDirectory=catalyst_output_dir

# makes a cinema D index table
make_cinema_table=False

#--------------------------------------------------------------
# Code generated from cpstate.py to create the CoProcessor.
# paraview version 5.8.0
#--------------------------------------------------------------

from paraview.simple import *
from paraview import coprocessing


# ----------------------- CoProcessor definition -----------------------

def CreateCoProcessor():
  def _CreatePipeline(coprocessor, datadescription):
    class Pipeline:
      # state file generated using paraview version 5.8.0

      # ----------------------------------------------------------------
      # setup the data processing pipelines
      # ----------------------------------------------------------------

      # trace generated using paraview version 5.8.0
      #
      # To ensure correct image size when batch processing, please search 
      # for and uncomment the line `# renderView*.ViewSize = [*,*]`

      #### disable automatic camera reset on 'Show'
      paraview.simple._DisableFirstRenderCameraReset()

      # https://stackoverflow.com/questions/48068641/paraview-get-points-data-from-integrate-variables
      # https://www.paraview.org/Wiki/ParaView/Python_Scripting
      # https://kitware.github.io/vtk-examples/site/Python/CompositeData/MultiBlockDataSet/
      # or
      # https://kitware.github.io/vtk-examples/site/Cxx/CompositeData/MultiBlockDataSet/

      
      rESULTS_FLUID_DOMAINcase = coprocessor.CreateProducer(datadescription, 'input')
      # l = dir(rESULTS_FLUID_DOMAINcase.GetDataInformation())      
      # print('\n'.join(map(str, l))) # GetDataInformation
     
      try:        
        #  mkey = list(GetSources().keys())[1]
        # print(mkey)
        # bnds = GetSources().get(mkey).GetDataInformation().DataInformation.GetBounds()
        bnds = rESULTS_FLUID_DOMAINcase.GetDataInformation().DataInformation.GetBounds()
        

        pt1=[(bnds[1]-bnds[0])/2,(bnds[3]-bnds[2])/2,(bnds[5]-bnds[4])/2]


        # x_range =  bnds[0]
        print('DAMARIS bnds   :'+str(bnds[0])+', '+str(bnds[1])+', '+str(bnds[2]) +', '+str(bnds[3]) +', '+str(bnds[4]) +', '+str(bnds[5])  )
        print('DAMARIS ctr    :'+str(pt1[0])+', '+str(pt1[1])+', '+str(pt1[2])   )
        # print('DAMARIS ctr    :'+str(pt2[0])+', '+str(pt2[1])+', '+str(pt2[2])   )


        # create a 'Slice'for xz plane
        slice1 = Slice(Input=rESULTS_FLUID_DOMAINcase)
        slice1.SliceType = 'Plane'
        slice1.HyperTreeGridSlicer = 'Plane'
        slice1.Triangulatetheslice = 0
        slice1.Mergeduplicatedpointsintheslice = 0
        slice1.SliceOffsetValues = [0.0]
        slice1.SliceType.Normal  = [0.0, 1.0, 0.0]
        slice1.SliceType.Origin  = [pt1[0], pt1[1], pt1[2]]
        slice1.HyperTreeGridSlicer.Origin = [pt1[0], pt1[1], pt1[2]]
        # ----------------------------------------------------------------
        # finally, restore active source
        # SetActiveSource(slice1)
        # ----------------------------------------------------------------
        
        # Prepared data for catalyst writers.         
        # PassArrays() reduces the variables selected
        slice1_mpi_rank_id = PassArrays(Input=rESULTS_FLUID_DOMAINcase, PointDataArrays=[], CellDataArrays=['fields/mpi_rank_id'])  # not FieldDataArrays=['fields/velocity']
        # N.B. MergeBlocks is essential for the catalyst implementation used by (MultiBlockDataset + MultiPieceDataset)
        slice1_mpi_rank_id_merged = MergeBlocks(Input=slice1_mpi_rank_id) 
        # All variables will be present ion the slice
        slice1_merged = MergeBlocks(Input=slice1 )

        cSVWriter1 = servermanager.writers.CSVWriter(Input=slice1_merged, FieldAssociation='Cell Data')
        coprocessor.RegisterWriter(cSVWriter1, filename='allfields_merged_xz'+'_%t.csv', freq=2, paddingamount=3, DataMode='None', HeaderType='None', EncodeAppendedData=None, CompressorType='None', CompressionLevel='None')
        
        cSVWriter2 = servermanager.writers.CSVWriter(Input=slice1_mpi_rank_id_merged, FieldAssociation='Cell Data')
        coprocessor.RegisterWriter(cSVWriter2, filename='mpi_rank_id_3d_merged_xy'+'_%t.csv', freq=200, paddingamount=3, DataMode='None', HeaderType='None', EncodeAppendedData=None, CompressorType='None', CompressionLevel='None')

        # create a 'Slice' for the xy plane
        """
        slice2 = Slice(Input=rESULTS_FLUID_DOMAINcase)
        slice2.SliceType = 'Plane'
        slice2.HyperTreeGridSlicer = 'Plane'
        slice2.Triangulatetheslice = 0
        slice2.Mergeduplicatedpointsintheslice = 0
        slice2.SliceOffsetValues = [0.0]
        slice2.SliceType.Normal  = [0.0, 0.0, 1.0]
        slice2.SliceType.Origin  = [pt1[0], pt1[1], pt1[2]]
        slice2.HyperTreeGridSlicer.Origin = [pt1[0], pt1[1], pt1[2]]
        # finally, restore active source
        # SetActiveSource(slice2)
        # slice2_arrays = PassArrays(Input=slice2, PointDataArrays=[], CellDataArrays=['fields/velocity'])
        # cSVWriter2 = servermanager.writers.CSVWriter(Input=slice2_arrays, FieldAssociation='Cell Data')
        # coprocessor.RegisterWriter(cSVWriter2, filename='input_velocity_xy'+'_%t.csv', freq=2, paddingamount=3, DataMode='None', HeaderType='None', EncodeAppendedData=None, CompressorType='None', CompressionLevel='None')
        """
              
      except (IndexError, NameError):
        print('DAMARIS Pipeline: No data sources this iteration. We will try the next iteration')
        print(mynon_existant_variable)
            
    return Pipeline()            


  class CoProcessor(coprocessing.CoProcessor):
    def CreatePipeline(self, datadescription):
      self.Pipeline = _CreatePipeline(self, datadescription)

  coprocessor = CoProcessor()
  # these are the frequencies at which the coprocessor updates.
  freqs = {'input': [1]}
  coprocessor.SetUpdateFrequencies(freqs)
  if requestSpecificArrays:
    arrays = [['fields/velocity', 1]]
    coprocessor.SetRequestedArrays('input', arrays)
  coprocessor.SetInitialOutputOptions(timeStepToStartOutputAt,forceOutputAtFirstCall)

  if rootDirectory:
      coprocessor.SetRootDirectory(rootDirectory)

  if make_cinema_table:
      coprocessor.EnableCinemaDTable()

  return coprocessor


#--------------------------------------------------------------
# Global variable that will hold the pipeline for each timestep
# Creating the CoProcessor object, doesn't actually create the ParaView pipeline.
# It will be automatically setup when coprocessor.UpdateProducers() is called the
# first time.
coprocessor = CreateCoProcessor()

#--------------------------------------------------------------
# Enable Live-Visualizaton with ParaView and the update frequency
coprocessor.EnableLiveVisualization(True, 1)

# ---------------------- Data Selection method ----------------------

def RequestDataDescription(datadescription):
  "Callback to populate the request for current timestep"
  global coprocessor

  # setup requests for all inputs based on the requirements of the
  # pipeline.
  coprocessor.LoadRequestedData(datadescription)

# ------------------------ Processing method ------------------------

def DoCoProcessing(datadescription):
  "Callback to do co-processing for current timestep"
  global coprocessor

  # Update the coprocessor by providing it the newly generated simulation data.
  # If the pipeline hasn't been setup yet, this will setup the pipeline.
  coprocessor.UpdateProducers(datadescription)

  #timestep = datadescription.GetTimeStep()
  #print('DAMARIS timestep: '+str(timestep)   )

  #if (timestep > 0):

  # Write output data, if appropriate.
  coprocessor.WriteData(datadescription);

  # Write image capture (Last arg: rescale lookup table), if appropriate.
  # coprocessor.WriteImages(datadescription, rescale_lookuptable=rescale_lookuptable,
  #   image_quality=0, padding_amount=imageFileNamePadding)

  # Live Visualization, if enabled.
  coprocessor.DoLiveVisualization(datadescription, "localhost", 22222)
