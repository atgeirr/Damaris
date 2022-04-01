# Dockerfiles for building Damaris

The files in this directory contian the instructions for building Docker images with various dependent libaries, They all inclued the system installed OpenMPI version with Parallel HDF5. Then there is a choice between Visit or Paraview visualization engines for in-situ visualization.

Once built, a base image can be used to build software that has Damaris as a dependency, by specifying the base image name:tag in the Docker FROM statement in a new dockerfile.

## Reduce the file output clipping in Docker build

As building these images creates a lot of stdout, we need to increase the limits before clipping.


Edit the file (Ubuntu 20.04):
/etc/systemd/system/multi-user.target.wants/docker.service

Then, under the [Service] tag, add the environment variables:
```
# Added this to increase the log limit of build kit!
Environment="BUILDKIT_STEP_LOG_MAX_SIZE=20971520"
Environment="BUILDKIT_STEP_LOG_MAX_SPEED=1048576"
```

Then restart the service

```
$ sudo systemctl daemon-reload
$ sudo systemctl restart docker

```

## Building a docker image
Choose the distribution and base capability  you are interested in (Visit or Paraview) and then use the following commands to build
the docker image

### For Paraview enabled Damaris

```
# Set these variables to suit
export DOCKERFILE=Dockerfile.opensuse.paraview
export DAMARIS_VER=v1.6.0
export PV_VERSION=v5.9.1
export DAMARIS_REPO=damaris
export FULL_TAG=${DAMARIS_VER}-opensuse-visit${VISIT_VER}
export PVSHORT=${PV_VERSION//./}
sudo  DOCKER_BUILDKIT=1 docker build -t \
            registry.gitlab.inria.fr/damaris/damaris:${FULL_TAG} \
            --build-arg INPUT_damaris_ver=${DAMARIS_VER} \
            --build-arg INPUT_repo=${DAMARIS_REPO}  \
            --build-arg INPUT_pv_ver=${PV_VERSION} \
                -f ./${DOCKERFILE}.

# Now we can run the image - get a shell as the 'docker' user
sudo docker run -u "docker:staff" --rm -v /dev/shm:/dev/shm -p 22222:22222 -it registry.gitlab.inria.fr/damaris/damaris:${FULL_TAG} /bin/bash


```

### For Visit enabled Damaris

```
# Set these variables to suit
export DOCKERFILE=Dockerfile.opensuse.visit
export DAMARIS_VER=v1.6.0
export VISIT_VER=3.2.1
export DAMARIS_REPO=damaris
export FULL_TAG=${DAMARIS_VER}-opensuse-visit${VISIT_VER}
sudo  DOCKER_BUILDKIT=1 docker build --progress=plain  -t \
            registry.gitlab.inria.fr/damaris/damaris:${FULL_TAG} \
            --build-arg INPUT_damaris_ver=${DAMARIS_VER} \
            --build-arg INPUT_repo=${DAMARIS_REPO} \
            --build-arg INPUT_visit_ver=${VISIT_VER} \
                -f ./${DOCKERFILE} .

 # Now we can run the image - get a shell as the 'docker' user
sudo docker run -u "docker:staff" --rm -v /dev/shm:/dev/shm  -it registry.gitlab.inria.fr/damaris/damaris:${FULL_TAG}  /bin/bash

```


