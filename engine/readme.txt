Network Protocol

A message with opcode NET_SYNC contains the positions and veloicities of every actor in the game. Because this is just one UDP packet, its size is limited to approximately 510 bytes (at present).

The data in the message is laid out:

OPCODE[1] ID[1] NUMBER OF ACTORS[1] ACTOR ID[1] ACTOR POSITION[16] ACTOR VELOCITY[16]

This system is therefore limited to 15 actors.
