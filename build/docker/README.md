# Docker files for building Damaris
These files will build the Damaris library, it's unit tests and examples within a docker container.
The Docker files (Dockerfile.ubuntu20 and Dockerfile.centos8) are currently used by the Gitlab CI system,
so should remain in this directory (build/docker) and not be modified without careful testing.
They currently compile the HDF5 compatible  version. The Python capable version should also be implemented sometime.

### To build the damaris repo (no password needed):
```bash
 sudo TAG=v1.5.0 DOCKER_BUILDKIT=1 docker build -t \
            registry.gitlab.inria.fr/damaris/damaris-ubu20:${TAG} \
            --build-arg INPUT_damaris_ver=${TAG} \
            --build-arg INPUT_repo=damaris \
                -f ./Dockerfile.ubuntu20 .
```


## Log in to the gitlab repository 
 N.B. You may need a ~/.docker/config.json file configured with an access token
```bash
 docker login registry.gitlab.inria.fr
 
 docker push registry.gitlab.inria.fr/damaris/damaris:${TAG}"
```

### To test the created docker image:
```bash
# pull the latest version (or another tagged version)
docker pull registry.gitlab.inria.fr/damaris/damaris:latest"

# Get a shell into the container
sudo docker run --rm -it registry.gitlab.inria.fr/damaris/damaris:<branch> /bin/bash
 
# Run unit tests:
<container prompt> cd /home/docker/local/examples
<container prompt> ctest
 
# run and example:
<container prompt> mpirun -np 4  /home/docker/local/examples/storage/3dmesh /home/docker/local/examples/storage/3dmesh.xml [-v] [-r]
 
# Should find HDF5 output
<container prompt> ls *.h5
<container prompt> h5dump *.h5
```