#!/bin/env python3

import sys
import time
import socket
import math

node_id = "default_fake_node"

class EnvData:
    temperature: float = 0.0
    pressure: int = 0
    def __init__(self, temperature, pressure):
        self.temperature=temperature
        self.pressure=pressure

i = 0

# slowly oscillating between 15 and 25 degrees
def generateTemperature() -> float:
    global i
    c = 20.0
    a = 0.1
    b = 5
    return c + b*math.sin(a*i)

# fast oscillation between 98 and 102 kPa
def generatePressure() -> int:
    global i
    c = 100000
    a = 1
    b = 2000
    return int(c + b*math.sin(a*i))

def formatDataIntoLineprotocol(data: EnvData) -> str:
    return f"measurement,node={node_id},fake=yes temperature={data.temperature:.2f},pressure={data.pressure}i"

# https://strangfeld.io/posts/ipv6-udp-sockets-python/
# socket to send on
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# target_ip6 = "2a05:d016:bb1:9a01:fcaf:16c1:d73f:d5ab"
target_ip = "195.148.30.80"
target_port = 50123

def sendData():
    data = formatDataIntoLineprotocol(
        EnvData(generateTemperature(), generatePressure())
    )
    print(data)
    sock.sendto(data.encode("UTF-8"), (target_ip, target_port))

def main() -> int :
    global i
    global node_id

    if len(sys.argv) < 2:
        print("You must specify the node_id as the first argument!")
        sys.exit(1)

    node_id = sys.argv[1]

    try:
        while True:
            sendData()
            i += 1
            time.sleep(15)

    except KeyboardInterrupt:
        print("\nExiting!\n")
        return 0


if __name__ == "__main__":
    ret = main()
    sys.exit(ret)