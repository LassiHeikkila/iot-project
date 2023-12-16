#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: $0 <path to firmware to flash> <node id>"
    exit 1
fi

if [ -z "${EXPERIMENT_ID}" ]; then
    echo "EXPERIMENT_ID must be set!"
    exit 1
fi

FW_IMAGE=$1
NODE_ID=$2
SITE=grenoble

iotlab-node --flash "${FW_IMAGE}" -i "${EXPERIMENT_ID}" -l "${SITE},m3,${NODE_ID}"
