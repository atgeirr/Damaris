cwlVersion: v1.1
class: CommandLineTool
baseCommand: ["cat"]
arguments:
  - position: 1
    valueFrom: '$(inputs.scheduler_file_input)'
stdout: scheduler.json

inputs:
  scheduler_file_input:
    type: File

outputs:
  scheduler_file:
    type: stdout