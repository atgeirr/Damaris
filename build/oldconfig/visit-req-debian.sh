#!/bin/bash

apt-get update
yes Y | apt-get install xutils-dev
yes Y | apt-get install libxt-dev
yes Y | apt-get install python-libxml2
yes Y | apt-get install freeglut3-dev

