#!/bin/bash

DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
PROJECT_ROOT=$DIR/..

source $DIR/config.sh

VERSION=0.1.0
UBUNTU=noble

# To build different images for different distros and users:

# Development environment
# docker build --build-arg VERSION=${VERSION} --build-arg UBUNTU=${UBUNTU} --target dev -t $DEV_IMAGE_NAME:latest -t $DEV_IMAGE_NAME:${UBUNTU} -f $DIR/Dockerfile $PROJECT_ROOT

# Build
# docker build --build-arg VERSION=${VERSION} --build-arg UBUNTU=${UBUNTU} --target build -t $BUILD_IMAGE_NAME:latest -t $BUILD_IMAGE_NAME:${UBUNTU} -f $DIR/Dockerfile  $PROJECT_ROOT

# Export (builds .deb package to dist folder)
docker build --build-arg VERSION=${VERSION} --build-arg UBUNTU=${UBUNTU} --target export --output type=local,dest=dist -t $EXPORT_IMAGE_NAME:latest -t $EXPORT_IMAGE_NAME:${UBUNTU} -f $DIR/Dockerfile  $PROJECT_ROOT

# User
# docker build --build-arg VERSION=${VERSION} --build-arg UBUNTU=${UBUNTU} --target user -t $USER_IMAGE_NAME:latest -t $USER_IMAGE_NAME:${UBUNTU} -f $DIR/Dockerfile  $PROJECT_ROOT