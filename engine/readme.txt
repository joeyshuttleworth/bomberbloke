\Network Protocol

Messages over several datagrams:

use the first message e.g NET_SYNC to send the total amount of datagrams required

OPCODE[1] ID[2] NUMBER OF DATAGRAMS[1]

The ID will then be reserved. 

The data in the following NET_MUTLI messages is laid out:    [bytes]

OPCODE[1] ID[2] ID OF NET_MULTI(part number)[1] NUMBER OF ACTORS[1] ACTOR ID[1] ACTOR POSITION[16] ACTOR VELOCITY[16]

or in the case of a NET_ACTOR_LIST message: 
OPCODE[1] ID[2] ID OF NET_MULTI(part number)[1] ACTOR TYPE[1] 
