#! /bin/bash

# Author: Josh Bowden, Inria
# Date: 4/2/2022
# Usage:   ./test_damaris_images.sh  DAMARIS_REPO  DAMARIS_VER  EXECMD
#               DAMARIS_REPO  is the repo to clone (damaris or damaris-development)
#               DAMARIS_VER   is the tag or a branch to build. Include prefix with "-damaris-v1.5.0"
#                              or specify "-" as no extra tag string.
#               EXECMD        is the command to run within the container  (e.g. ctest)
# 
# e.g.    Run ctest suite:
#          > sudo ./test_damaris_images.sh  damaris  "-damaris-v1.5.0"  "ctest -Q"
#         Test the base image that (contains Paraview only):
#          > sudo ./test_damaris_images.sh  damaris-devlopment  "-"  "cmake --version"
#         Get a bash prompt in base image:
#         (change PV_VER_ARRAY and DOCKERFILE_ARRAY to select version to run):
#          > sudo ./test_damaris_images.sh  damaris-devlopment  "-"  "/bin/bash"
#


#            > docker login registry.gitlab.inria.fr

if [[ "$1" != "" ]] ; then
  export DAMARIS_REPO=$1
else
  export DAMARIS_REPO=damaris
fi

if [[ "$2" == "-" ]] ; then
  export DAMARIS_VER=""
elif [[ "$2" != "" ]] ; then
  export DAMARIS_VER="$2"
else
  export DAMARIS_VER="v1.5.0"
fi

if [[ "$3" != "" ]] ; then
  export EXECMD=$3
else
  export EXECMD="ctest -Q"
fi


# Set up arrays of Paraview versions and base images:
PYTHON_VER_ARRAY=( python nodeps)
DOCKERFILE_ARRAY=(ubuntu20 ubuntu21 debian10 debian11 centos8 archlinux opensuse)


# To test a subset of the containers specify smaller arrays
# PV_VER_ARRAY=( v5.9.1)
# DOCKERFILE_ARRAY=(ubuntu20)


# This function gets a simplified version of the basename used in a Dockerfile
# i.e. It grabs the first "FROM" declaration of the Dockerfile and processes it.
# e.g. If the line is : FROM <base-name:tag/tag-base>
#      It will return : base-name-tag-tag-base
# N.B. It only removes 2 forward slashes, any more will be included.
# N.B. It must match the same function in ../bases/build_base_containers.sh
# The derived string is used for a 'tag' of the built image
get_tag_name () {
  PATH_AND_DOCKERFILENAME=$1 
  BASE_IMAGE=$(cat ${PATH_AND_DOCKERFILENAME} | grep ^[[:space:]]*FROM | head -n 1 |  awk -F ' ' '{print $2}')
  # Remove : and / from the strings
  BASE_IMAGE_SHORT=${BASE_IMAGE/:/-}
  BASE_IMAGE=${BASE_IMAGE_SHORT/\//-}
  BASE_IMAGE_SHORT=${BASE_IMAGE/\//-}
  echo $BASE_IMAGE_SHORT
}


# docker login registry.gitlab.inria.fr
DOCKER_IMAGE_BASENAME=registry.gitlab.inria.fr/damaris/$DAMARIS_REPO
# DOCKER_IMAGE_OUTPUTNAME=registry.gitlab.inria.fr/damaris/$DAMARIS_REPO

#######################################################
## Setting up the markdown table
#######################################################

MAX=1
LEN_ARRAY=()
# Get the length of each row header string and find the max of them all
for DOCKERFILE in ${DOCKERFILE_ARRAY[@]};
do
    BASE_IMAGE_SHORT=$(get_tag_name "../bases/Dockerfile.${DOCKERFILE}.python")
    CURRENT_NUM=${#BASE_IMAGE_SHORT}  # string length
    if [[ "$CURRENT_NUM" -gt "$MAX" ]]; then
       MAX="$CURRENT_NUM"
    fi
    LEN_ARRAY+=($CURRENT_NUM)
done


echo "GRAPH: Docker image base : $DOCKER_IMAGE_BASENAME<O.S.><ParaView>$DAMARIS_VER  "
echo "GRAPH: Command Tested    : $EXECMD  "
TABLE_HEAD="GRAPH: |   "
TABLE_BASE="GRAPH: |---"
# Make the lines all matching length
for STEP in $(seq 1 $MAX);  do
    TABLE_HEAD+=" ";
    TABLE_BASE+="-";
done  
# Add the columns for each Python install
for PY_VERSION in ${PYTHON_VER_ARRAY[@]};
do
  TABLE_HEAD+="| $PV_VERSION "
  TABLE_BASE+="|--------"
done
echo "$TABLE_HEAD|"
echo "$TABLE_BASE|"

#######################################################
##  Loop through docker images and run tests
#######################################################


i=0
for DOCKERFILE in ${DOCKERFILE_ARRAY[@]};
do
    LEN=${LEN_ARRAY[$i]}
    BASE_IMAGE_SHORT=$(get_tag_name "../bases/Dockerfile.${DOCKERFILE}.python")
   
    TABLE_ROW="GRAPH: | $BASE_IMAGE_SHORT "
    # Make the GRAPH: lines all matching length
    for STEP in $(seq $LEN $MAX);  do
        TABLE_ROW+=" ";
    done    

    # echo "DOCKERFILE=$BASE_IMAGE_SHORT  " 
    if [[ "$BASE_IMAGE_SHORT" != "" ]] ; then        
        for PYTHON_VERSION in ${PYTHON_VER_ARRAY[@]};
        do
          
          # echo ""
          BASEIMAGETAG=$(echo $BASE_IMAGE_SHORT-python)
          if [[ "$PYTHON_VERSION" == "nodeps" ]] ; then 
            BASEIMAGETAG+="-py-off-damaris-${DAMARIS_VER}"
          else
            BASEIMAGETAG+="-py-on-damaris-${DAMARIS_VER}"
          fi
          # echo "BASEIMAGETAG= $DOCKER_IMAGE_BASENAME:$BASEIMAGETAG  ${EXECMD}"
          # Check if the image exists in the repository
          TMPVAR=$(docker manifest inspect $DOCKER_IMAGE_BASENAME:${BASEIMAGETAG} 2> /dev/null) 
          BUILD_IMAGE=$(echo $?)
          #  echo "BUILD_IMAGE= $BUILD_IMAGE"
          if [[ "$BUILD_IMAGE" == "0" ]] ; then 
              # The base container exists in the repository                  
              # echo "Building: $DOCKER_IMAGE_OUTPUTNAME:${BASEIMAGETAG}"
              # Run the command within the Docker image:
              docker run  --rm -v /dev/shm:/dev/shm -p 22222:22222 \
                  ${DOCKER_IMAGE_BASENAME}:${BASEIMAGETAG} ${EXECMD}
            if [[ $? -eq 0 ]] ; then
               # docker push "$DOCKER_IMAGE_OUTPUTNAME:${BASEIMAGETAG}-damaris-${DAMARIS_VER}"
               # echo "INFO: ${DOCKER_IMAGE_OUTPUTNAME}:${BASEIMAGETAG}-damaris-${DAMARIS_VER} $EXECMD Completeded successfully"
               TABLE_ROW+="|  pass  "
               # echo ""
            else 
               # echo "ERROR: ${DOCKER_IMAGE_OUTPUTNAME}:${BASEIMAGETAG}-damaris-${DAMARIS_VER} $EXECMD failed"
               TABLE_ROW+="|  fail  "
              # echo ""
            fi
            # rm ./Dockerfile.out
         else
           # echo "INFO: The base image ${DOCKER_IMAGE_BASENAME}:${BASEIMAGETAG} does not exist "
           TABLE_ROW+="|  nbc  "
        fi
        done
    else
      echo "ERROR: Dockerfile.${DOCKERFILE}.python does not exist - check the names given in DOCKERFILE_ARRAY"
    fi
    echo "$TABLE_ROW|"
    i=$((i+1))
done
echo "GRAPH: Legend:  "
echo "GRAPH: pass  : command completed successfully  "
echo "GRAPH: fail  : command failed  "
echo "GRAPH: nbc   : damaris base not built  "
echo "GRAPH:   "