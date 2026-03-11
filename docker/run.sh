#!/bin/bash

DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
PROJECT_ROOT=$DIR/..

source $DIR/config.sh

ARGS=(
    --rm
    -it

    # X11 forwarding
    --network=host
    -e DISPLAY=$DISPLAY
    -v /tmp/.X11-unix:/tmp/.X11-unix

    # Set up working environment
    -v $PROJECT_ROOT:$PROJECT_ROOT
    -w $PROJECT_ROOT
)

docker run ${ARGS[@]} $DEV_IMAGE_NAME:latest /bin/bash -l "$@"