cwlVersion: v1.1
class: CommandLineTool
baseCommand: ["bash"]
arguments:
  - position: 1
    valueFrom: '$(inputs.script)'
  - position: 2
    valueFrom: '$(inputs.python_source)'
  - position: 3
    valueFrom: '$(inputs.scheduler_file_input)'

inputs:
  sim_ended_file:
    type: File[]
  script:
    type: File
  python_source:
    type: File
  scheduler_file_input:
    type: File

outputs:
  results_report:
    type: File
    outputBinding:
      glob: "report.txt"
  scheduler_file:
    type: File
    outputBinding:
      glob: scheduler_file_input