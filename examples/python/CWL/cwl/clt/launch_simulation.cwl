cwlVersion: v1.1
class: CommandLineTool
baseCommand: ["bash"]
arguments:
  - position: 1
    valueFrom: '$(inputs.script)'
  - position: 2
    valueFrom: '$(inputs.executable)'
  - position: 3
    valueFrom: '$(inputs.xml_file)'
  - position: 4
    valueFrom: '$(inputs.sim_input)'
  - position: 5
    valueFrom: '$(inputs.scheduler_file_input)'
  - position: 6
    valueFrom: '$(inputs.sim_python)'

inputs:
  script:
    type: File
  executable:
    type: File
  xml_file:
    type: File
  sim_python:
    type: File
  sim_input:
    type: int
  scheduler_file_input:
    type: File

outputs:
  sim_ended:
    type: File
    outputBinding:
      glob: "sim_ended.txt"