# IPv6 proxy
[`socat.sh`](./socat.sh) uses `socat` to forward UDP traffic received on port 50123 from an IPv6 interface towards an IPv4 address with the same port number.

The script currently sends traffic towards `195.148.30.80` which is the IP of the main application server for this project, running in [CSC](https://www.csc.fi/).

PORT and TARGET_IP are configurable via environment variables.

The server hosting this IPv6 proxying script must have the used port open and have a functioning IPv6 address. 
Instructions to set that up depend on the platform.
