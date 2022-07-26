# Docker files for building Damaris

The Docker files in the directories ```bases``` and ```damaris``` are currently used by the Gitlab CI system,
so should remain in the respective directories and not be modified without careful testing.
  
The dockerfiles in the ```full_builds``` directory can be used to create stand-alone docker images containing Damaris.
There are versions that build Paraview or Visit within the container on a range of base image Linux distributions.

### To build the damaris repo (no password needed):
```bash
export TAG=v1.5.0
sudo  DOCKER_BUILDKIT=1 docker build -t \
        registry.gitlab.inria.fr/damaris/damaris:${TAG}-ubu20 \
        --build-arg INPUT_damaris_ver=${TAG} \
        --build-arg INPUT_repo=damaris \
            -f ./Dockerfile.ubuntu20 .
```


## Log in to the gitlab repository 
 N.B. You may need a ~/.docker/config.json file configured with an access token
```bash
 docker login registry.gitlab.inria.fr
 
 docker push registry.gitlab.inria.fr/damaris/damaris:${TAG}
```

### To test the created docker image:
```bash
# pull the latest version (or another tagged version)
docker pull registry.gitlab.inria.fr/damaris/damaris:latest

# Get a shell into the container
sudo docker run --rm -it registry.gitlab.inria.fr/damaris/damaris:<branch> /bin/bash
 
# Run unit tests:
<container prompt> cd /home/docker/local/examples/damaris
<container prompt> ctest
 
# run and example:
<container prompt> cd /home/docker/local/examples/damaris
<container prompt> mpirun -np 4  /home/docker/local/examples/damaris/storage/3dmesh /home/docker/local/examples/damaris/storage/3dmesh.xml [-v] [-r]
 
## After running, we should find HDF5 output
<container prompt> ls *.h5
<container prompt> h5dump *.h5
```
