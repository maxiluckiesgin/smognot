#!/usr/bin/env python

"""Echo server using the asyncio API."""

import asyncio
import json
from websockets.asyncio.server import serve


async def echo(websocket):
    async for message in websocket:
        data = json.loads(message)
        for key,value in data.items():
       	    print(f"{key}: {value} from client")
        await websocket.send("Data received by server")


async def main():
    async with serve(echo, "192.168.1.43", 9786) as server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())
