#!/usr/bin/env python

import asyncio
import websockets

async def log_messages(websocket):
    async for message in websocket:
        print(f"New message : {message}")

async def main():
    async with websockets.serve(log_messages, port=8080):
        print("Websocket server started")
        await asyncio.Future()  # run forever

asyncio.run(main())