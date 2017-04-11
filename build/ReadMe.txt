The main aim of the scripts inside this folder is to help developers prepare their working environment for Damaris compilation and execution automatically. Before preparing your environment, make sure that you have updated the first few lines of the env_prep.sh:

- Change the install_path variable to the desired location. 
- Change install_visit to either 1 or 0, indicating if you want to install VisIt or not. 
- Change the desired MPI library. 

Afterwards, if you need VisIt to be installed, run visit-req-*.sh scripts depending on your Linux distribution. This will install all packages required by VisIt. To prepare your environment, then simply run:

$ ./env_prep.sh

This script will download, compile and install CMake, mpich/openmpi, Xerces, XSD, Boost, CppUnit, VisIt and finally Damaris. 

If you want to prepare the environment in a docker container, you can simply run the following command in the current directory (for SUSE distribution):

$ docker build -f Dockerfile.suse -t damaris-suse:1.0 .

This command creates an image, namely damaris-suse with 1.0 as its tag, based on suse and runs the env_prep.sh scrip inside it. So, do no forget to update env_prep.sh script beforehand. 