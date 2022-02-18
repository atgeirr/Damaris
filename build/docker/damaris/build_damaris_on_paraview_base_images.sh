#! /bin/bash

# Author: Josh Bowden, Inria
# Date: 4/2/2022
# Usage:   ./build_damaris_on_paraview_base_images.sh  DAMARIS_REPO  DAMARIS_VER   MY_CI_READ_REPO_PWD
#               DAMARIS_VER   is the tag or a branch to build
#               DAMARIS_REPO  is the repo to clone (damaris or damaris-development)
#
#          Requires sudo if docker commands need sudo privlages
#          Script will try to build docker images from all availabe prebuilt 
#          images (available in registry.gitlab.inria.fr) by substituting
#          the FROM part of the file Dockerfile.${DOCKERFILE}.paraview .
#
# Requirements: 
#         A set of files in directory:  ../bases/Dockerfile.${DOCKERFILE}.paraview
#         A set of built base images in registry.gitlab.inria.fr/damaris/damaris-development that
#         correspond to the Dockerfiles (Dockerfile.${DOCKERFILE}.paraview)
#         A file: Docker._BASEWITHPARAVIEW_.paraview that has template strings to replace using sed:
#           _BASEWITHPARAVIEW_  _DISTLIB_  _HDF_PATH_  _MPI_BIN_PATH_  _PV_SHORT_DOT_  _INSTALL_GFORT_
#
# Outputs: 
#        Docker images built from the base images that are stored in :
#           registry.gitlab.inria.fr/damaris/$DAMARIS_REPO:${BASEIMAGETAG}-damaris 
#        where BASEIMAGETAG == $(echo $BASE_IMAGE_SHORT-p${PVSHORT})
#        and BASE_IMAGE_SHORT is derived from the FROM tag in the base Dockerfiles 
#        (i.e. derived from  ../bases/Dockerfile.${DOCKERFILE}.paraview using function get_tag_name() below)
#       /bin/bash -c
#            > docker login registry.gitlab.inria.fr

if [[ "$1" != "" ]] ; then
  export DAMARIS_REPO=$1
else
  export DAMARIS_REPO=damaris
fi

if [[ "$2" != "" ]] ; then
  export DAMARIS_VER=$2
else
  export DAMARIS_VER=v1.5.0
fi

if [[ "$3" != "" ]] ; then
  export MY_CI_READ_REPO_PWD=$3
else
  export MY_CI_READ_REPO_PWD=""
fi



PV_VER_ARRAY=(v5.8.0 v5.8.1 v5.9.0 v5.9.1 v5.10.0)
DOCKERFILE_ARRAY=(ubuntu20 ubuntu21 debian10 debian11 centos8 archlinux opensuse)
LIBNAME_ARRAY=(lib      lib      lib      lib      lib64    lib      lib64)
HDF_PATH_INDEX=(0       0         0        0        1       1        1)
HDF_PATH_ARRAY=("-DHDF5_ROOT=/usr/lib/x86_64-linux-gnu/hdf5/openmpi"  "")
MPI_BIN_PATH_ARRAY=(""   ""   ""   ""     "/usr/lib64/openmpi/bin/"  "/usr/bin/"    "/usr/lib64/mpi/gcc/openmpi4/bin/")
GFORT_ARRAY=("" "" "" "" "" "" "RUN zypper -n install gcc-fortran libboost*_75_0-devel")
# "sed -i  's|\${_paraview_vtk_components_to_request_required})|\"\${_paraview_vtk_components_to_request_required}\")|g'  /home/docker/local/lib/cmake/paraview-5.10/paraview-config.cmake"

# To rebuild a subset of the containers specify smaller arrays
# PV_VER_ARRAY=(v5.9.1 v5.10.0)
# DOCKERFILE_ARRAY=(opensuse)
# LIBNAME_ARRAY=(lib64)
# HDF_PATH_INDEX=( 1)
# HDF_PATH_ARRAY=("-DHDF5_ROOT=/usr/lib/x86_64-linux-gnu/hdf5/openmpi"  "")
# MPI_BIN_PATH_ARRAY=(   "/usr/lib64/mpi/gcc/openmpi4/bin/")
# GFORT_ARRAY=( "RUN  zypper -n install gcc-fortran libboost*_75_0-devel")

# This function gets a simplified version of the basename used in a Dockerfile
# i.e. It grabs the first "FROM" declaration of the Dockerfile and processes it.
# e.g. If the line is : FROM <base-name:tag/tag-base>
#      it will return : base-name-tag-tag-base
# N.B. it only removes 2 forward slashes, any more will be included.
# N.B. It must match the same function in ../bases/build_base_containers.sh
get_tag_name () {
  PATH_AND_DOCKERFILENAME=$1 
  BASE_IMAGE=$(cat ${PATH_AND_DOCKERFILENAME} | grep ^[[:space:]]*FROM | head -n 1 |  awk -F ' ' '{print $2}')
  # Remove : and / from the strings
  BASE_IMAGE_SHORT=${BASE_IMAGE/:/-}
  BASE_IMAGE=${BASE_IMAGE_SHORT/\//-}
  BASE_IMAGE_SHORT=${BASE_IMAGE/\//-}
  echo $BASE_IMAGE_SHORT
}


#######################################################
## Setting up the markdown table
#######################################################

MAX=1
LEN_ARRAY=()
# Get the length of each row header string and find the max of them all
for DOCKERFILE in ${DOCKERFILE_ARRAY[@]};
do
    BASE_IMAGE_SHORT=$(get_tag_name "../bases/Dockerfile.${DOCKERFILE}.paraview")
    CURRENT_NUM=${#BASE_IMAGE_SHORT}  # string length
    if [[ "$CURRENT_NUM" -gt "$MAX" ]]; then
       MAX="$CURRENT_NUM"
    fi
    LEN_ARRAY+=($CURRENT_NUM)
done

echo "GRAPH: Repository: $DAMARIS_REPO  branch:$DAMARIS_VER"
TABLE_HEAD="GRAPH: |   "
TABLE_BASE="GRAPH: |---"
# Make the lines all matching length
for STEP in $(seq 1 $MAX);  do
    TABLE_HEAD+=" ";
    TABLE_BASE+="-";
done  
# Add the columns for each Paraview install
for PV_VERSION in ${PV_VER_ARRAY[@]};
do
  TABLE_HEAD+="| $PV_VERSION "
  TABLE_BASE+="|--------"
done
echo "$TABLE_HEAD|"
echo "$TABLE_BASE|"


#######################################################
##  Loop through docker images and build damaris
#######################################################

# docker login registry.gitlab.inria.fr
DOCKER_IMAGE_BASENAME=registry.gitlab.inria.fr/damaris/damaris-development
DOCKER_IMAGE_OUTPUTNAME=registry.gitlab.inria.fr/damaris/$DAMARIS_REPO
i=0
for DOCKERFILE in ${DOCKERFILE_ARRAY[@]};
do
    BASE_IMAGE_SHORT=$(get_tag_name "../bases/Dockerfile.${DOCKERFILE}.paraview")
    
    # Get variables for replacing in Dockerfile._BASEWITHPARAVIEW_.paraview templated file
    LIB64=${LIBNAME_ARRAY[$i]}
    WHICH_HDFPATH=${HDF_PATH_INDEX[$i]}
    HDF_PATH=${HDF_PATH_ARRAY[${WHICH_HDFPATH}]}
    MPI_BIN_PATH=${MPI_BIN_PATH_ARRAY[$i]}
    GFORT=${GFORT_ARRAY[$i]}
    LEN=${LEN_ARRAY[$i]}
    TABLE_ROW="GRAPH: | $BASE_IMAGE_SHORT "
    
    # Make the GRAPH: lines all matching length
    for STEP in $(seq $LEN $MAX);  do
        TABLE_ROW+=" ";
    done    
    
    # echo "DOCKERFILE=$DOCKERFILE    LIB64 = $LIB64  WHICH_HDFPATH=$WHICH_HDFPATH  HDF_PATH=$HDF_PATH  " 
    if [[ "$BASE_IMAGE_SHORT" != "" ]] ; then
        for PV_VERSION in ${PV_VER_ARRAY[@]};
        do
          echo ""
          PVSHORT=${PV_VERSION//./}
          PV_SHORT_DOT=${PV_VERSION:1:-2}
          BASEIMAGETAG=$(echo $BASE_IMAGE_SHORT-p${PVSHORT})

          # Check if the image exists in the repository
          TMPVAR=$(docker manifest inspect $DOCKER_IMAGE_BASENAME:${BASEIMAGETAG} 2> /dev/null) 
          BUILD_IMAGE=$(echo $?)
          #  echo "BUILD_IMAGE= $BUILD_IMAGE"
          if [[ "$BUILD_IMAGE" == "0" ]] ; then
              # The base container exists in the repository   
                cp Dockerfile._BASEWITHPARAVIEW_.paraview  Dockerfile.out            
                sed -i "s|_BASEWITHPARAVIEW_|${DOCKER_IMAGE_BASENAME}:${BASEIMAGETAG}|g" Dockerfile.out
                sed -i "s|_DISTLIB_|${LIB64}|g" Dockerfile.out
                sed -i "s|_HDF_PATH_|${HDF_PATH}|g" Dockerfile.out
                sed -i "s|_MPI_BIN_PATH_|${MPI_BIN_PATH}|g" Dockerfile.out
                sed -i "s|_PV_SHORT_DOT_|${PV_SHORT_DOT}|g" Dockerfile.out
                sed -i "s|_INSTALL_GFORT_|${GFORT}|g" Dockerfile.out
                # echo "Building: $DOCKER_IMAGE_OUTPUTNAME:${BASEIMAGETAG}"
                DOCKER_BUILDKIT=1 docker build --no-cache -t \
                  ${DOCKER_IMAGE_OUTPUTNAME}:${BASEIMAGETAG}-damaris-${DAMARIS_VER} \
                  --secret id=thepassword,src=$MY_CI_READ_REPO_PWD \
                   --build-arg INPUT_damaris_ver=${DAMARIS_VER} \
                   --build-arg INPUT_repo=${DAMARIS_REPO} \
                  -f ./Dockerfile.out . 
            if [[ $? -eq 0 ]] ; then
               docker push "$DOCKER_IMAGE_OUTPUTNAME:${BASEIMAGETAG}-damaris-${DAMARIS_VER}"
               echo "INFO: ${DOCKER_IMAGE_OUTPUTNAME}:${BASEIMAGETAG}-damaris-${DAMARIS_VER}  built"
               TABLE_ROW+="|   d    "
               # echo ""
            else 
               echo "ERROR: ${DOCKER_IMAGE_OUTPUTNAME}:${BASEIMAGETAG}-damaris-${DAMARIS_VER} could not be built"
               TABLE_ROW+="|   x    "
              # echo ""
            fi
            rm ./Dockerfile.out
         else
           echo "INFO: The base image ${DOCKER_IMAGE_BASENAME}:${BASEIMAGETAG} does not exist "
           TABLE_ROW+="|   pf   "
        fi
        done
    else
      echo "ERROR: Dockerfile.${DOCKERFILE}.paraview does not exist - check the names given in DOCKERFILE_ARRAY"
    fi
    echo "$TABLE_ROW|"
    i=$((i+1))
done
echo "GRAPH: Legend:  "
echo "GRAPH: d  : damaris built  "
echo "GRAPH: x  : damaris not built  "
echo "GRAPH: pf : paraview base not built  "
echo "GRAPH:   "
