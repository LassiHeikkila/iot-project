#!/bin/env python3

import sys
import time
import socket
import math
import argparse
import math
import random

DEFAULT_IPV4_TARGET='195.148.30.80'
DEFAULT_IPV6_TARGET='2a05:d016:bb1:9a01:fcaf:16c1:d73f:d5ab'
DEFAULT_TARGET_PORT=50123
DEFAULT_NODE_ID = "default_fake_node"
DEFAULT_NOISE_STDDEV_TEMP = 1.0 # degrees C
DEFAULT_NOISE_STDDEV_PRES = 1000 # Pa

DEFAULT_INTERVAL=10

# global data
i = 0
node_id = DEFAULT_NODE_ID

def generate_normal_random(stddev : float) -> float :
	return random.gauss(mu=0, sigma=stddev)

def add_noise(stddev : float) -> float :
	noise_val = 0.0
	num = random.randint(0, 100)
	if num > 50:
		noise_val = generate_normal_random(stddev)
	return noise_val;

class EnvData:
    temperature: float = 0.0
    pressure: int = 0
    def __init__(self, temperature, pressure):
        self.temperature=temperature
        self.pressure=pressure

# slowly oscillating between 15 and 25 degrees
def generateTemperature(with_noise: bool) -> float:
    global i
    c = 20.0
    a = 0.1
    b = 5
    val = c + b*math.sin(a*i)
    if with_noise:
        val += add_noise(DEFAULT_NOISE_STDDEV_TEMP)
    return val

# faster oscillation between 98 and 102 kPa
def generatePressure(with_noise: bool) -> int:
    global i
    c = 100000
    a = 0.3
    b = 2000
    val = int(c + b*math.sin(a*i))
    if with_noise:
        val += int(add_noise(DEFAULT_NOISE_STDDEV_PRES))
    return val

def formatDataIntoLineprotocol(data: EnvData) -> str:
    return f"measurement,node={node_id},fake=yes temperature={data.temperature:.2f},pressure={data.pressure}i"

def sendData(
        with_noise: bool = False, 
        use_ipv6: bool = False, 
        target_ipv4 = DEFAULT_IPV4_TARGET, 
        target_ipv6: str = DEFAULT_IPV6_TARGET, 
        target_port=DEFAULT_TARGET_PORT
):
    data = formatDataIntoLineprotocol(
        EnvData(generateTemperature(with_noise), generatePressure(with_noise))
    )
    print(data)

    # https://strangfeld.io/posts/ipv6-udp-sockets-python/
    if use_ipv6:
        sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
        sock.sendto(data.encode("UTF-8"), (target_ipv6, target_port))
    else:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.sendto(data.encode("UTF-8"), (target_ipv4, target_port))

def main() -> int :
    global i
    global node_id

    parser = argparse.ArgumentParser()
    parser.add_argument('--id', default=DEFAULT_NODE_ID, help="Node ID to use")
    parser.add_argument('--noise', default=False, action='store_true', help='Add noise to data')
    parser.add_argument('--use_ipv6', default=False, action='store_true', help='Use IPv6')
    parser.add_argument('--target_ipv4', default=DEFAULT_IPV4_TARGET, help='Target IPv4 address')
    parser.add_argument('--target_ipv6', default=DEFAULT_IPV6_TARGET, help='Target IPv6 address')
    parser.add_argument('--target_port', default=DEFAULT_TARGET_PORT, type=int, help='Target port')
    parser.add_argument('--interval', default=DEFAULT_INTERVAL, type=float, help='Interval between data samples, given in seconds.')

    args = parser.parse_args()
    node_id = args.id

    try:
        while True:
            sendData(
                with_noise=args.noise, 
                use_ipv6=args.use_ipv6, 
                target_ipv4=args.target_ipv4, 
                target_ipv6=args.target_ipv6, 
                target_port=args.target_port
            )
            i += 1
            time.sleep(args.interval)

    except KeyboardInterrupt:
        print("\nExiting!\n")
        return 0


if __name__ == "__main__":
    ret = main()
    sys.exit(ret)