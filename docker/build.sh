#!/bin/bash

DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
PROJECT_ROOT=$DIR/..

source $DIR/config.sh

VERSION=0.1.0
UBUNTU=noble

USAGE="Usage: $0 [option]
Options:
  --dev     Build development environment image
  --build   Build release binary (target: build)
  --export  Build and export .deb package (target: export)
  --user    Build user runtime image (target: user)
  all       Build all images (default)"

TARGETS=()

while [[ $# -gt 0 ]]; do
    case $1 in
        --dev)
            TARGETS+=(dev)
            shift
            ;;
        --build)
            TARGETS+=(build)
            shift
            ;;
        --export)
            TARGETS+=(export)
            shift
            ;;
        --user)
            TARGETS+=(user)
            shift
            ;;
        all)
            TARGETS=(dev build export user)
            shift
            ;;
        -h|--help)
            echo "$USAGE"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "$USAGE"
            exit 1
            ;;
    esac
done

if [[ ${#TARGETS[@]} -eq 0 ]]; then
    TARGETS=(export)
fi

for target in "${TARGETS[@]}"; do
    case $target in
        dev)
            echo "Building dev image..."
            docker build --build-arg VERSION=${VERSION} --build-arg UBUNTU=${UBUNTU} --target dev -t $DEV_IMAGE_NAME:latest -t $DEV_IMAGE_NAME:${UBUNTU} -f $DIR/Dockerfile $PROJECT_ROOT
            ;;
        build)
            echo "Building build image..."
            docker build --build-arg VERSION=${VERSION} --build-arg UBUNTU=${UBUNTU} --target build -t $BUILD_IMAGE_NAME:latest -t $BUILD_IMAGE_NAME:${UBUNTU} -f $DIR/Dockerfile  $PROJECT_ROOT
            ;;
        export)
            echo "Building and exporting .deb package..."
            docker build --build-arg VERSION=${VERSION} --build-arg UBUNTU=${UBUNTU} --target export --output type=local,dest=dist -t $EXPORT_IMAGE_NAME:latest -t $EXPORT_IMAGE_NAME:${UBUNTU} -f $DIR/Dockerfile  $PROJECT_ROOT
            ;;
        user)
            echo "Building user image..."
            docker build --build-arg VERSION=${VERSION} --build-arg UBUNTU=${UBUNTU} --target user -t $USER_IMAGE_NAME:latest -t $USER_IMAGE_NAME:${UBUNTU} -f $DIR/Dockerfile  $PROJECT_ROOT
            ;;
    esac
done
