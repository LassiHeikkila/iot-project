#!/bin/bash

NAME=my-experiment
DURATION=60
COUNT=5
SITE=grenoble

experimentId="$(iotlab-experiment submit -n "${NAME}" -d "${DURATION}" -l "${COUNT},archi=m3:at86rf231+site=${SITE}" | jq -r .id )"

echo "Experiment launched with ID: ${experimentId}"
echo "Set environment variable EXPERIMENT_ID by running:"
echo "export EXPERIMENT_ID=${experimentId}"
