#!/usr/bin/env python

"""Client using the asyncio API."""

import asyncio
import json
from websockets.asyncio.client import connect


async def hello():
    async with connect("ws://192.168.1.43:9786") as websocket:
        data = {"name": "ALice", "age":25}
        to_send=json.dumps(data)
        await websocket.send(to_send)
        message = await websocket.recv()
        print(message)


if __name__ == "__main__":
    asyncio.run(hello())
