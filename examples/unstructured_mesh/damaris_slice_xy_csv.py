
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

# rootDirectory='_TEMPLATE_ROOT_DIR_CATALYST_OUTPUT_'

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

      # create a new 'EnSight Reader'
      # create a producer from a simulation input
      
      # coprocessor_name_template = ''
      timestep = datadescription.GetTimeStep()
      print('timestep:1 '+str(timestep)   )
      
      if (timestep == 0):
          print(mynon_existant_variable)
      
      if (timestep != 0):
         rESULTS_FLUID_DOMAINcase = coprocessor.CreateProducer(datadescription, 'input')
      
         # https://stackoverflow.com/questions/48068641/paraview-get-points-data-from-integrate-variables
         # https://www.paraview.org/Wiki/ParaView/Python_Scripting
         # https://kitware.github.io/vtk-examples/site/Python/CompositeData/MultiBlockDataSet/
         # or
         # https://kitware.github.io/vtk-examples/site/Cxx/CompositeData/MultiBlockDataSet/
         # as1 = paraview.simple.GetSources()
         # as1.keys()
         # as1.get('PassArrays1')
         # iv1 = IntegrateVariables(Input=as1.get('PassArrays1'))
         
         # integrateVariables1 = integrateVariables(Input=slice1)
         # DataSliceFile = paraview.servermanager.Fetch(integrateVariables1)
         # numCells = DataSliceFile.GetNumberOfCells()

         # list(GetSources().keys()) [list(GetSources().values()).index(GetActiveSource())][0]

         vel_arrays = PassArrays(Input=rESULTS_FLUID_DOMAINcase, PointDataArrays=[], CellDataArrays=['fields/velocity'])  
         # di         = vel_arrays.GetDataInformation()
         # pointInfo  = di.GetPointDataInformation()
         # bnds       = di.DataInformation.GetBounds()
         # um_data_slice1 = MergeBlocks(Input=vel_arrays)
         # bnds = um_data_slice1.GetDataInformation().DataInformation.GetBounds()
         
         #iv1 = IntegrateVariables(Input=vel_arrays)
         # GetSources().get('IntegrateVariables1').GetDataInformation().DataInformation.GetBounds()
         
         # bnds = GetSources()[('IntegrateVariables1', '5662')].GetDataInformation().DataInformation.GetBounds()
         # bnds = GetSources()['IntegrateVariables1'].GetDataInformation().DataInformation.GetBounds()
         # bnds = list(paraview.simple.GetSources().values())[1].GetDataInformation().DataInformation.GetBounds()
         iv1 = IntegrateVariables(Input=vel_arrays)
         dsf = paraview.servermanager.Fetch(iv1)
         pt1 = dsf.GetPoint(0)
         # bnds = dsf.GetBounds()
         # ctr = dsf.GetCenter()
         # bnds = dsf.GetDataInformation().DataInformation.GetBounds()

         
         
         #list(GetSources().keys())[1]
         ## ('IntegrateVariables1', '5662')
         mkey = list(GetSources().keys())[1]
         print(mkey)
         # GetSources().get(mkey)
         ## <paraview.servermanager.IntegrateVariables object at 0x7fbe0e44d090>
         bnds = GetSources().get(mkey).GetDataInformation().DataInformation.GetBounds()

         # dsf = paraview.servermanager.Fetch(iv1)
         # glyph   = Glyph(iv1)
         #di         = iv1.GetDataInformation()
         #pointInfo  = di.GetPointDataInformation()
         #bnds       = di.DataInformation.GetBounds()
         # bnds    = di.DataInformation.GetBounds()
         #  iv1 = IntegrateVariables(Input=as1.get('IntegrateVariables1'))
         # as1.get('PassArrays1')
         
         # glyph   = Glyph(vel_arrays)
         # glyph.UpdatePipeline()
         # di      = glyph.GetDataInformation()
         # bnds    = di.DataInformation.GetBounds()
         # x_range =  bnds[0]
         print('bnds   :'+str(bnds[0])+', '+str(bnds[1])+', '+str(bnds[2]) +', '+str(bnds[3]) +', '+str(bnds[4]) +', '+str(bnds[5])  )
         print('ctr    :'+str(pt1[0])+', '+str(pt1[1])+', '+str(pt1[2])   )
         
         
         # intf = paraview.servermanager.Fetch(iv1)
         # print intf.GetPoint(0)
         # x_range2 = intf.GetPoint(0)[1] - intf.GetPoint(0)[0]
         # print('x_range2:'+str(intf.GetPoint(0)))

         # bounds    = pointInfo.GetBounds() 

     
      
      
          # create a new 'Slice'
         slice1 = Slice(Input=rESULTS_FLUID_DOMAINcase)
         slice1.SliceType = 'Plane'
         slice1.HyperTreeGridSlicer = 'Plane'
         slice1.Triangulatetheslice = 0
         slice1.Mergeduplicatedpointsintheslice = 0
         slice1.SliceOffsetValues = [0.0]
         
         
         # slice_z_origin = _TEMPLATE_SLICE_Z_ORIGIN_
         # /home/jbowden/C_S/saturne_examples/case_mesh_3d_64/PARAVIEW_EXPORTS
         #slice_z_origin = os.environ.get('SLICE_Z_ORIGIN')
         #if slice_z_origin == 'None':
         #print('WARNING: Catalyst Coprocessing Script: '+os.path.basename(__file__))
         #print(' The SLICE_Z_ORIGIN environment variable was not set!')
         #slice_z_origin=0.5
         # sys.exit('exiting due to no SLICE_Z_ORIGIN env variable')
         slice_z_origin=0.5
         
         
         slice1.SliceType.Normal = [0.0, 0.0, 1.0]
         # init the 'Plane' selected for 'SliceType'
         # slice1.SliceType.Origin = [0.0, 0.0, 0.0]
         #slice1.SliceType.Origin = [pt1[0]/2.0,pt1[1]/2.0, pt1[2]/2.0]
         # slice1.SliceType.Origin = [iv1[0], iv1[1], iv1[2]]

         # init the 'Plane' selected for 'HyperTreeGridSlicer'
         # slice1.HyperTreeGridSlicer.Origin = [0.0, 0.0, 0.0]
         # slice1.HyperTreeGridSlicer.Origin = [pt1[0]/2.0,pt1[1]/2.0, pt1[2]/2.0]
         print('timestep:2 '+str(timestep)   )
         slice1.SliceType.Origin = [pt1[0], pt1[1], pt1[2]]
         slice1.HyperTreeGridSlicer.Origin = [pt1[0], pt1[1], pt1[2]]
          
         # ----------------------------------------------------------------
         # finally, restore active source
         SetActiveSource(slice1)
         # ----------------------------------------------------------------

         # Now any catalyst writers
      
      
         slice1_arrays = PassArrays(Input=slice1, PointDataArrays=[], CellDataArrays=['fields/velocity'])  # not FieldDataArrays=['fields/velocity']
         # cSVWriter1 = servermanager.writers.CSVWriter(Input=slice1_arrays)
         # um_data_slice1 = MergeBlocks(Input=slice1_arrays)
         # cSVWriter1 = servermanager.writers.CSVWriter(Input=um_data_slice1, CellDataArrays=['fields/velocity'])
         cSVWriter1 = servermanager.writers.CSVWriter(Input=slice1_arrays, FieldAssociation='Cell Data')
         coprocessor.RegisterWriter(cSVWriter1, filename='input_velocity'+'_%t.csv', freq=2, paddingamount=2, DataMode='None', HeaderType='None', EncodeAppendedData=None, CompressorType='None', CompressionLevel='None')

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

    # Write output data, if appropriate.
    coprocessor.WriteData(datadescription);

    # Write image capture (Last arg: rescale lookup table), if appropriate.
    coprocessor.WriteImages(datadescription, rescale_lookuptable=rescale_lookuptable,
        image_quality=0, padding_amount=imageFileNamePadding)

    # Live Visualization, if enabled.
    coprocessor.DoLiveVisualization(datadescription, "localhost", 22222)
