#include "htRimeStack.h"
#include "commons/addresses.h"
#include "commons/constants.h"

#include <stdio.h>
#include <stdlib.h>
#include "net/rime/rime.h"

extern void setNodesAddresses();

static void recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno){}

static void sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
	printf("runicast message sent to %d.%d, retransmissions %d\n", to->u8[0], to->u8[1], retransmissions);
}

static void timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
	printf("runicast message timed out when sending to %d.%d, retransmissions %d\n", to->u8[0], to->u8[1], retransmissions);
}

static const struct runicast_callbacks runicast_calls = {recv_runicast, sent_runicast, timedout_runicast};

static struct runicast_conn cuRunicastConnection;

void sendToRoomLightNode(unsigned char* c, int bytes)
{
	unsigned char* buffer;
	char bufferLength = setBuffer(&buffer, c, bytes, HT_NODE_HIGH, RL_NODE_HIGH);
	packetbuf_copyfrom(buffer, bufferLength);
	runicast_send(&cuRunicastConnection, &centralNodeAddress, MAX_RETRANSMISSIONS);
	free(buffer);
}

void initHTRimeStack()
{
	setNodesAddresses();
	printf("My address is %d.%d\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	runicast_open(&cuRunicastConnection, RIME_CHANNEL, &runicast_calls);
}
