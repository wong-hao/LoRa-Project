#https://blog.csdn.net/qq_35491275/article/details/112755292

import os
import sys

import grpc
from chirpstack_api import ns

# Configuration.

# This must point to the API interface.
server = "106.14.134.224:8000"

# The DevEUI for which you want to enqueue the downlink.
dev_eui = bytes([0x53, 0x23, 0x2c, 0x5e, 0x6c, 0x93, 0x64, 0x83])

if __name__ == "__main__":
  # Connect without using TLS.
  channel = grpc.insecure_channel(server)

  # Device-queue API client.
  client = ns.NetworkServerServiceStub(channel)


  # Construct request.
  req = ns.CreateMACCommandQueueItemRequest()
  req.dev_eui = dev_eui
  req.cid = 0x03
  req.commands = bytes([0x03, 0x34, 0x00, 0x00, 0x00])

  resp = client.CreateMACCommandQueueItem(req)

 # Print the downlink frame-counter value.
  print("The MACCommand has been enqueued")