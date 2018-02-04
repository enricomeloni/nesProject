#include "ovenRimeStack.h"
#include "commons/addresses.h"
#include "commons/constants.h"

#include "stdio.h"
#include "net/rime/rime.h"

static void recv_broadcast(struct broadcast_conn *c, const linkaddr_t *from){}
static const struct broadcast_callbacks broadcast_calls = {recv_broadcast};
static struct broadcast_conn speakerBroadcastConnection;

void sendToSpeakers(unsigned char* command, int bytes)
{
	packetbuf_copyfrom(command, bytes);
	broadcast_send(&speakerBroadcastConnection);
}

void initOvenRimeStack()
{
	setNodesAddresses();
	printf("My address is %d.%d\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	broadcast_open(&speakerBroadcastConnection, SPEAKER_CHANNEL, &broadcast_calls);
}