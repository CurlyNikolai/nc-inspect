#!/bin/bash

DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
PROJECT_ROOT=$DIR/..

source $DIR/config.sh

VERSION=0.1.0

for UBUNTU in jammy noble
do
    docker build \
        --build-arg VERSION=${VERSION} \
        --build-arg UBUNTU=${UBUNTU} \
        --target export \
        --output type=local,dest=dist \
        -t $EXPORT_IMAGE_NAME:latest \
        -t $EXPORT_IMAGE_NAME:${UBUNTU} \
        -f $DIR/Dockerfile $PROJECT_ROOT
done