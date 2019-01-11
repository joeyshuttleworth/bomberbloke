Network Protocol

Messages over several datagrams:

use NET_MULTI to send the total amount of datagrams required

OPCODE[1] ID[1] NUMBER OF DATAGRAMS[1]

The ID will then be reserved. 

A message with opcode NET_SYNC contains the positions and veloicities of every actor in the game. Because this is just one UDP packet, its size is limited to approximately 510 bytes (at present).

The data in the message is laid out:    [bytes]

OPCODE[1] ID[1] ID OF NET_MULTI[1] NUMBER OF ACTORS[1] ACTOR ID[1] ACTOR POSITION[16] ACTOR VELOCITY[16]

Where the leftmost bit of PART is only set if

NET_ACTOR_LIST

OPCODE[1] ID[1] ID OF NET_MULTI[1] ACTOR TYPE[1] 
