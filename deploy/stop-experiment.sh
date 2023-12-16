#!/bin/bash

if [ -z "${EXPERIMENT_ID}" ]; then
    echo "EXPERIMENT_ID must be set!"
    exit 1
fi

iotlab-experiment stop -i "${EXPERIMENT_ID}"
