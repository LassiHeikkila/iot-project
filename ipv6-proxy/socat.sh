#!/bin/bash

PORT="${PORT:-50123}"
TARGET_IP="${TARGET_IP:-195.148.30.80}"

# tip: add -v flag to enable logging of the traffic
socat \
    UDP6-RECVFROM:${PORT},reuseaddr,fork \
    UDP4-SENDTO:${TARGET_IP}:${PORT}
