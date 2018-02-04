//
// Created by Raff on 31/10/2017.
//

#include "speakerRimeStack.h"
#include "commons/addresses.h"
#include "commons/constants.h"

#include "stdio.h"
#include "net/rime/rime.h"

extern void processOvenCommand(unsigned char command);
extern void processCUCommand(unsigned char command);

static void recv_broadcast(struct broadcast_conn *c, const linkaddr_t *from)
{
	unsigned char receivedCommand = *( (unsigned char*)packetbuf_dataptr() );
	
	#if DEBUG
	printf("broadcast message received from %d.%d, message: %d\n",
		   from->u8[0],
		   from->u8[1],
		   receivedCommand);
	#endif
	
	if(linkaddr_cmp(from, &centralNodeAddress))
	{
		processCUCommand(receivedCommand);
	}
	else if(linkaddr_cmp(from, &ovenNodeAddress))
	{
		processOvenCommand(receivedCommand);
	}
	else
	{
		printf("Message from unexpected node: refused\n");
	}
}
static const struct broadcast_callbacks broadcast_calls = {recv_broadcast};
static struct broadcast_conn cuBroadcastConnection;

void initSpeakerRimeStack()
{
	setNodesAddresses();
	printf("My address is %d.%d\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	broadcast_open(&cuBroadcastConnection, SPEAKER_CHANNEL, &broadcast_calls);
}