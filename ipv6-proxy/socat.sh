#!/bin/bash

PORT="${PORT:-50123}"
TARGET_IP="${TARGET_IP:-195.148.30.80}"

socat \
    UDP-RECVFROM:${PORT},reuseaddr,fork \
    UDP4-SENDTO:${TARGET_IP}:${PORT}
