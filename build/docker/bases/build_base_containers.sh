#! /bin/bash

# Author:  Josh Bowden, Inria
# Date  :  4/2/2022
# Usage :  sudo ./build_base_containers.sh  2> ./build_base.err
#          Requires sudo if docker commands need sudo privlages
#          Script will try to build docker images from all files of form Dockerfile.${DOCKERFILE_ARRAY[]}.paraview 
#          and then push them to the repository under name: DOCKER_IMAGE_BASENAME
#          The user should log in to the docker registry first.
#            > docker login registry.gitlab.inria.fr

PV_VER_ARRAY=(v5.7.0 v5.8.0 v5.8.1 v5.9.0 v5.9.1 v5.10.0)
VISIT_VER_ARRAY=( 3.2.1 )
DOCKERFILE_ARRAY=(ubuntu20 ubuntu21 debian10 debian11 centos8 archlinux opensuse)

# docker login registry.gitlab.inria.fr
DOCKER_IMAGE_BASENAME=registry.gitlab.inria.fr/damaris/damaris-development

# This function gets a simplified version of the basename used in a Dockerfile
# i.e. It grabs the first "FROM" declaration of the Dockerfile and processes it.
# e.g. If the line is : FROM <base-name:tag/tag-base>
#      it will return : base-name-tag-tag-base
# N.B. It only removes 2 forward slashes, any more will be included.
# N.B. It must match the same function in ../damaris/build_damaris_on_paraview_base_images.sh
get_tag_name () {
  PATH_AND_DOCKERFILENAME=$1 
  BASE_IMAGE=$(cat ${PATH_AND_DOCKERFILENAME} | grep ^[[:space:]]*FROM | head -n 1 |  awk -F ' ' '{print $2}')
  # Remove : and / from the strings
  BASE_IMAGE_SHORT=${BASE_IMAGE/:/-}
  BASE_IMAGE=${BASE_IMAGE_SHORT/\//-}
  BASE_IMAGE_SHORT=${BASE_IMAGE/\//-}
  echo $BASE_IMAGE_SHORT
}

echo "Build Paraview Base Images"
for DOCKERFILE in ${DOCKERFILE_ARRAY[@]};
do
    BASE_IMAGE_SHORT=$(get_tag_name "./Dockerfile.${DOCKERFILE}.paraview")
    if [[ "$BASE_IMAGE_SHORT" != "" ]] ; then
        for PV_VERSION in ${PV_VER_ARRAY[@]};
        do
          echo ""
          PVSHORT=${PV_VERSION//./}
          TAG=$(echo $BASE_IMAGE_SHORT-p${PVSHORT})
          # echo $DOCKER_IMAGE_BASENAME:${TAG}
          BUILD_IMAGE=$(sudo docker manifest inspect $DOCKER_IMAGE_BASENAME:${TAG} 2> /dev/null ; echo $?)
          if [[ "$BUILD_IMAGE" == "1" ]] ; then
             echo "Building: $DOCKER_IMAGE_BASENAME:${TAG}"
            #   DOCKER_BUILDKIT=1 docker build -q -t \
            #      ${DOCKER_IMAGE_BASENAME}:${TAG} \
            #      --build-arg INPUT_pv_ver=${PV_VERSION} \
            #      -f ./Dockerfile.$DOCKERFILE.paraview .
            if [[ $? -eq 0 ]] ; then
               echo "Pushing $DOCKER_IMAGE_BASENAME:${TAG}"
               # docker push "$DOCKER_IMAGE_BASENAME:${TAG}"
            else 
               echo "ERROR: Dockerfile.${DOCKERFILE}.paraview PV_VERSION=$PV_VERSION could not be built"
            fi
         else
           echo "INFO: the image $DOCKER_IMAGE_BASENAME:${TAG} already exists"
         fi
        done
    else
      echo "ERROR: Dockerfile.${DOCKERFILE}.paraview does not exist - check the names given in DOCKERFILE_ARRAY"
    fi
done


echo "Build  Visit base images"
for DOCKERFILE in ${DOCKERFILE_ARRAY[@]};
do
    BASE_IMAGE_SHORT=$(get_tag_name "./Dockerfile.${DOCKERFILE}.visit")
    if [[ "$BASE_IMAGE_SHORT" != "" ]] ; then
        for VISIT_VERSION in ${VISIT_VER_ARRAY[@]};
        do
          echo ""
          VISITSHORT=${VISIT_VERSION//./}
          TAG=$(echo $BASE_IMAGE_SHORT-visit${VISITSHORT})
          # echo $DOCKER_IMAGE_BASENAME:${TAG}
          BUILD_IMAGE=$(sudo docker manifest inspect $DOCKER_IMAGE_BASENAME:${TAG} 2> /dev/null ; echo $?)
          if [[ "$BUILD_IMAGE" == "1" ]] ; then
              echo "Building: $DOCKER_IMAGE_BASENAME:${TAG}"
              # DOCKER_BUILDKIT=1 docker build -q -t \
              #    ${DOCKER_IMAGE_BASENAME}:${TAG} \
              #    --build-arg INPUT_pv_ver=${VISIT_VERSION} \
              #    -f ./Dockerfile.$DOCKERFILE.visit .
            if [[ $? -eq 0 ]] ; then
               echo "Pushing $DOCKER_IMAGE_BASENAME:${TAG}"
               # docker push "$DOCKER_IMAGE_BASENAME:${TAG}"
            else
               echo "ERROR: Dockerfile.${DOCKERFILE}.visit VISIT_VERSION=$VISIT_VERSION could not be built"
            fi
         else
           echo "INFO: the image $DOCKER_IMAGE_BASENAME:${TAG} already exists"
         fi
        done
    else
      echo "ERROR: Dockerfile.${DOCKERFILE}.visit does not exist - check the names given in DOCKERFILE_ARRAY"
    fi
done


