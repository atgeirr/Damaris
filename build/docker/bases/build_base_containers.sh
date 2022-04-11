#! /bin/bash

# Author:  Josh Bowden, Inria
# Date  :  4/2/2022
# Usage :  First ensure you are logged in to the registry:
#          >sudo docker login registry.gitlab.inria.fr
#          Then run the script:
#          >sudo ./build_base_containers.sh  2>&1  ./build_base.log
#           or, to force rebuild of all base containers
#          >sudo ./build_base_containers.sh --force 2>&1 | tee ./build_base.log
# 
#          Requires sudo if docker commands need sudo privlages
#          Script will try to build docker images from all files of form Dockerfile.${DOCKERFILE_ARRAY[]}.paraview 
#          and then push them to the repository under name: DOCKER_IMAGE_BASENAME

PV_VER_ARRAY=(v5.8.0 v5.8.1 v5.9.0 v5.9.1 v5.10.0)
VISIT_VER_ARRAY=( 3.2.1 )
DOCKERFILE_ARRAY=(ubuntu20 ubuntu21 debian10 debian11 centos8 archlinux opensuse)
# DOCKERFILE_ARRAY=(  archlinux  )
# 
DOCKER_IMAGE_BASENAME=registry.gitlab.inria.fr/damaris/damaris-development

# This function gets a simplified version of the basename used in a Dockerfile
# i.e. It grabs the first "FROM" declaration of the Dockerfile and processes it.
# e.g. If the line is : FROM <base-name:tag/tag-base>
#      it will return : base-name-tag-tag-base
# N.B. It only removes 2 forward slashes, any more will be included.
# N.B. It must match the same function in ../damaris/build_damaris_on_paraview_base_images.sh
#                                      and ../damaris/build_damaris_on_visit_base_images.sh
get_tag_name () {
  PATH_AND_DOCKERFILENAME=$1 
  BASE_IMAGE=$(cat ${PATH_AND_DOCKERFILENAME} | grep ^[[:space:]]*FROM | head -n 1 |  awk -F ' ' '{print $2}')
  # Remove : and / from the strings
  BASE_IMAGE_SHORT=${BASE_IMAGE/:/-}
  BASE_IMAGE=${BASE_IMAGE_SHORT/\//-}
  BASE_IMAGE_SHORT=${BASE_IMAGE/\//-}
  echo $BASE_IMAGE_SHORT
} 


echo ""
echo ""
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
          if [[ "$1" == "--force" ]] ; then
            BUILD_IMAGE=1
          else
            BUILD_IMAGE=$(sudo docker manifest inspect $DOCKER_IMAGE_BASENAME:${TAG} 2> /dev/null ; echo $?)
          fi
          if [[ "$BUILD_IMAGE" == "1" ]] ; then
             echo "Building: $DOCKER_IMAGE_BASENAME:${TAG}"
               DOCKER_BUILDKIT=1 docker build --progress=plain -t \
                  ${DOCKER_IMAGE_BASENAME}:${TAG} \
                  --build-arg INPUT_pv_ver=${PV_VERSION} \
                  -f ./Dockerfile.$DOCKERFILE.paraview .
            if [[ $? -eq 0 ]] ; then
               echo "Pushing $DOCKER_IMAGE_BASENAME:${TAG}"
               docker push "$DOCKER_IMAGE_BASENAME:${TAG}"
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

exit 1

echo ""
echo ""
echo "Build  Visit base images"

for VISIT_VERSION in ${VISIT_VER_ARRAY[@]};
do
  echo ""
  VISIT_VER_UNDERSCORE=${VISIT_VERSION//./_}
  # Download the vist_build script locally if it does not exist already
  if [[ ! -f build_visit${VISIT_VER_UNDERSCORE} ]] ; then
    wget  https://github.com/visit-dav/visit/releases/download/v${VISIT_VERSION}/build_visit${VISIT_VER_UNDERSCORE}
    chmod +x build_visit${VISIT_VER_UNDERSCORE}
    # create a version that does not call the functions (i.e.e remove last 3 lines)
    # We will use this to call  initialize_build_visit() that sets VISITARCHTMP, 
    # which is needed to be passed as an argument (INPUT_os_arch_compiler_ver), so 
    # that we can set the path to the Python version that visit_build creates for 
    # the mesa configure part of the build.
    cat build_visit${VISIT_VER_UNDERSCORE} | head -n -3 > build_visit${VISIT_VER_UNDERSCORE}_to_source 
    # Download the visit zip files to be copied into the containers
    ./build_visit${VISIT_VER_UNDERSCORE}  --download-only  --thirdparty-path ${INSTALL_PREFIX} --mesagl  --llvm --server-components-only  --system-cmake --parallel
  fi
  
  if [[ ! -f build_visit${VISIT_VER_UNDERSCORE} ]] ; then
    echo "The Visit build script could not be downloaded: build_visit${VISIT_VER_UNDERSCORE}"
    exit 1
  fi
  
  for DOCKERFILE in ${DOCKERFILE_ARRAY[@]};
  do
    BASE_IMAGE_SHORT=$(get_tag_name "./Dockerfile.${DOCKERFILE}.visit")
    VISITSHORT=${VISIT_VERSION//./}
    TAG=$(echo $BASE_IMAGE_SHORT-visit${VISITSHORT})
    if [[ "$BASE_IMAGE_SHORT" != "" ]] ; then
          # echo $DOCKER_IMAGE_BASENAME:${TAG}
          if [[ "$1" == "--force" ]] ; then
            BUILD_IMAGE=1
          else
            BUILD_IMAGE=$(sudo docker manifest inspect $DOCKER_IMAGE_BASENAME:${TAG} 2> /dev/null ; echo $?)
          fi
          if [[ "$BUILD_IMAGE" == "1" ]] ; then
              echo "Building: $DOCKER_IMAGE_BASENAME:${TAG}"
              ## Build stage 1
              ## Visit needs a 2 stage build as we need to provide the VISTARCHTMP string
              ## to get paths set correctly
               DOCKER_BUILDKIT=1 docker build --progress=plain\
                 --target base_builder   \
                 -t  ${DOCKER_IMAGE_BASENAME}:${TAG} \
                  --build-arg INPUT_visit_ver=${VISIT_VERSION} \
                  -f ./Dockerfile.$DOCKERFILE.visit .
                  
                  # docker cp  ${DOCKER_IMAGE_BASENAME}:${TAG}:/home/docker/vist_arch.txt ./
                  docker run --rm -it ${DOCKER_IMAGE_BASENAME}:${TAG} cat /home/docker/vist_arch.txt > ./visit_arch_$DOCKERFILE.txt
                  VISITARCH=$(cat ./visit_arch_${DOCKERFILE}.txt | tr -d '\015' | tr -d '\n')
                   echo "VISITARCH: $VISITARCH"
                  ## Build stage 2
                  DOCKER_BUILDKIT=1 docker build --no-cache --progress=plain \
                  --target visit_builder   \
                  -t  ${DOCKER_IMAGE_BASENAME}:${TAG} \
                  --build-arg INPUT_visit_ver=${VISIT_VERSION} \
                  --build-arg  INPUT_os_arch_compiler_ver=$VISITARCH \
                  -f ./Dockerfile.$DOCKERFILE.visit .
            if [[ $? -eq 0 ]] ; then
               echo "Pushing $DOCKER_IMAGE_BASENAME:${TAG}"
               docker push "$DOCKER_IMAGE_BASENAME:${TAG}"
            else
               echo "ERROR: Dockerfile.${DOCKERFILE}.visit VISIT_VERSION=$VISIT_VERSION could not be built"
            fi
         else
           echo "INFO: the image $DOCKER_IMAGE_BASENAME:${TAG} already exists"
         fi
    else
      echo "ERROR: Dockerfile.${DOCKERFILE}.visit does not exist - check the names given in DOCKERFILE_ARRAY"
    fi
  done
done






