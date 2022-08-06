#!/usr/bin/env cwl-runner
cwlVersion: v1.1
class: Workflow
$namespaces:
  sf: "https://streamflow.org/cwl#"

inputs:
  scheduler_file_name: File
  sim_exec: File
  sim_xml: File
  sim_python_source: File
  num_simulations: int[]
  stats_get_results: File
  script_launch_sim: File
  gather_results_script: File

outputs:
  results_report:
    type: File
    outputSource: gather_results/results_report
  # scheduler_file:
  #   type: File
  #   outputSource: init_dask/scheduler_file
  # sim_ended:
  #   type: File[]
  #   outputSource: launch_simulation/sim_ended

requirements:
  ScatterFeatureRequirement: {}
  EnvVarRequirement:
    envDef:
      PYTHONPATH: /home/pviviani/opt/damarispy

steps:
  init_dask:
    run: clt/init_dask.cwl
    doc: |
      This step launches the dask scheduler and waits for it to come online.
    in:
      scheduler_file_input: scheduler_file_name
    out: [scheduler_file]
  launch_simulation:
    run: clt/launch_simulation.cwl
    doc: |
      This step launches the simulation
    scatter: sim_input
    in:
      script: script_launch_sim
      executable: sim_exec
      xml_file: sim_xml
      sim_input: num_simulations
      sim_python: sim_python_source
      scheduler_file_input: init_dask/scheduler_file
    out: [sim_ended]
  gather_results:
    run: clt/gather_results.cwl
    doc: |
      This step runs the script to gather statistics and stores results
    in:
      sim_ended_file: launch_simulation/sim_ended
      python_source: stats_get_results
      scheduler_file_input: init_dask/scheduler_file
      script: gather_results_script
    out: [results_report, scheduler_file]