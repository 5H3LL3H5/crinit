#!/bin/sh -e
# SPDX-License-Identifier: MIT
#
# Create and run docker build env
#
CMD_PATH=$(cd "$(dirname "$0")" && pwd)
BASE_DIR=${CMD_PATH%/*}
PROJECT=${BASE_DIR##*/}
REPO="amd64"
UBUNTU_RELEASE="noble"

clean_tag() {
    if [ -z "${1}" ]; then
        echo "Error: Argument for clean_tag missing."
        exit 1
    fi

    echo "${1}" \
        | tr -c '[:alnum:].-' '-' \
        | tr '[:upper:]' '[:lower:]' \
        | sed 's/^[^[:alnum:]]*//;s/[^[:alnum:]]*$//' \
        | sed 's/-\{2,\}/-/g'
}

CRINIT_IMAGE_NAME="$(clean_tag "${PROJECT}-crinit")"
CRINIT_DOCKER_NAME="$(clean_tag "${PROJECT}-target")"

echo "==> create docker image"
cd "$BASE_DIR"
DOCKER_BUILDKIT=1 \
    docker build \
    --progress=plain \
    --build-arg REPO="$REPO" \
    --build-arg UBUNTU_RELEASE="$UBUNTU_RELEASE" \
    --build-arg UID="$(id -u)" --build-arg GID="$(id -g)" \
    --tag "${CRINIT_IMAGE_NAME}" -f "$BASE_DIR/ci/Dockerfile.crinit" .

cd "$BASE_DIR/ci"
echo "==> run $PROJECT container"

if ! [ -e "$BASE_DIR"/ci/sshconfig ]; then
    {
        echo "Host *"
        echo "  User $(id -u -n)"
    } >"$BASE_DIR"/ci/sshconfig
fi

if [ "$SSH_AUTH_SOCK" ]; then
    SSH_AGENT_SOCK=$(readlink -f "$SSH_AUTH_SOCK")
    SSH_AGENT_OPTS="-v $SSH_AGENT_SOCK:/run/ssh-agent -e SSH_AUTH_SOCK=/run/ssh-agent"
fi

# shellcheck disable=SC2086 # Intended splitting of SSH_AGENT_OPTS.
docker run --rm -it --cap-add=SYS_ADMIN --security-opt apparmor=unconfined $SSH_AGENT_OPTS \
    --privileged \
    --name "${CRINIT_DOCKER_NAME}" \
    -w / \
    "${CRINIT_IMAGE_NAME}" "$@"
