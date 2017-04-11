#!/bin/bash


zypper update
zypper --non-interactive install -t pattern devel_basis
zypper --non-interactive install gcc-c++
zypper --non-interactive install gcc-fortran
zypper --non-interactive install git-core
zypper --non-interactive install hostname
zypper --non-interactive install wget
zypper --non-interactive install tar
zypper --non-interactive install makedepend
zypper --non-interactive install libXt-devel
zypper --non-interactive install python-libxml2
zypper --non-interactive install python-libxml2
zypper --non-interactive install glu-devel

