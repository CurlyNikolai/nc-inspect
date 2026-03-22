#!/bin/bash

DIR=$(dirname $(realpath ${BASH_SOURCE[0]}))
PROJECT_ROOT=$DIR/..

source $DIR/config.sh

USAGE="Usage: $0 [-u] <data_file>...
       $0 [--dev] [command...]

Options:
  -u, --user    Run user image with data file (requires building user image first)
      --dev     Run development image (default)"

MODE="user"
DATA_FILES=()
DEV_ARGS=()

while [[ $# -gt 0 ]]; do
    case $1 in
        -u|--user)
            MODE="user"
            shift
            ;;
        --dev)
            MODE="dev"
            shift
            ;;
        -h|--help)
            echo "$USAGE"
            exit 0
            ;;
        *)
            if [[ "$MODE" == "user" && ${#DATA_FILES[@]} -eq 0 && "$1" != -* ]]; then
                DATA_FILES+=("$1")
            else
                DEV_ARGS+=("$1")
            fi
            shift
            ;;
    esac
done

if [[ "$MODE" == "user" ]]; then
    if [[ ${#DATA_FILES[@]} -eq 0 ]]; then
        echo "Error: Data file required for user mode" >&2
        echo "$USAGE" >&2
        exit 1
    fi

    if ! docker image inspect "$USER_IMAGE_NAME:latest" > /dev/null 2>&1; then
        echo "Error: User image not found. Run './docker/build.sh --user' first." >&2
        exit 1
    fi

    DOCKER_ARGS=(
        --rm
        -it
        --network=host
        -e DISPLAY=$DISPLAY
        -v /tmp/.X11-unix:/tmp/.X11-unix
        --user $(id -u):$(id -g)
    )

    CONTAINER_PATHS=()
    for file in "${DATA_FILES[@]}"; do
        abs_path=$(realpath "$file")
        if [[ ! -f "$abs_path" ]]; then
            echo "Error: File not found: $file" >&2
            exit 1
        fi
        filename=$(basename "$abs_path")
        DOCKER_ARGS+=(-v "$abs_path:/data/$filename:ro")
        CONTAINER_PATHS+=("/data/$filename")
    done

    docker run ${DOCKER_ARGS[@]} $USER_IMAGE_NAME:latest nc-inspect "${CONTAINER_PATHS[@]}"
else
    ARGS=(
        --rm
        -it
        --network=host
        -e DISPLAY=$DISPLAY
        -v /tmp/.X11-unix:/tmp/.X11-unix
        -v $PROJECT_ROOT:$PROJECT_ROOT
        -w $PROJECT_ROOT
    )

    docker run ${ARGS[@]} $DEV_IMAGE_NAME:latest /bin/bash -l "${DEV_ARGS[@]}"
fi
