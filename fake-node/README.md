# `fake-node.py`

This is a small Python3 program to generate fake data and send it to the server.
It is used to demonstrate that the backend works, even if the real IoT deployment is failing due to environmental issues.

Usage:
```console
python3 fake-node.py <node_id>
```

The sent data is tagged as fake so it is simple to filter out in the backend if needed.
