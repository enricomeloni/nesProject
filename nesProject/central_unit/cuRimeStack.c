#include <stdlib.h>
#include "cuRimeStack.h"
#include "commons/addresses.h"
#include "commons/constants.h"

#include "stdio.h"
#include "net/rime/rime.h"

extern void processDoorMessage(unsigned char* message, int payloadSize);
extern void processGateMessage(unsigned char* message, int payloadSize);

extern void setNodesAddresses();

static struct runicast_conn doorRunicastConnection;
static struct runicast_conn gateRunicastConnection;
static struct runicast_conn htRunicastConnection;
static struct runicast_conn rlRunicastConnection;
void forward(unsigned char* buffer, int bytes, char nextHop);

static void recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno)
{
    unsigned char* buffer = packetbuf_dataptr();
	unsigned int bufferLength = packetbuf_datalen();
	char srcNode = *buffer;
	char dstNode = *(buffer+1);
	char payloadSize = *(buffer+2);
	unsigned char* payload = buffer+3;

    printf("runicast message received from %d.%d, seqno %d\n",
           from->u8[0],
           from->u8[1],
           seqno);

	if(dstNode == CENTRAL_UNIT_HIGH) //process message
	{
		if(srcNode == DOOR_NODE_HIGH)
		{
			processDoorMessage(payload, payloadSize);
		}
		else if(srcNode == GATE_NODE_HIGH)
		{
			processGateMessage(payload, payloadSize);
		}
		else
		{
			printf("Message from unexpected node: refused\n");
		}
	}
	else
	{
		forward(buffer, bufferLength, dstNode);
	}
	
}

static void sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
	printf("runicast message sent to %d.%d, retransmissions %d\n", to->u8[0], to->u8[1], retransmissions);
}

static void timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions)
{
	printf("runicast message timed out when sending to %d.%d, retransmissions %d\n", to->u8[0], to->u8[1], retransmissions);
}

static const struct runicast_callbacks runicast_calls = {recv_runicast, sent_runicast, timedout_runicast};

void sendDoorNode(unsigned char* c, int bytes)
{
	unsigned char* buffer;
	char bufferLength = setBuffer(&buffer, c, bytes, CENTRAL_UNIT_HIGH, DOOR_NODE_HIGH);
	packetbuf_copyfrom(buffer, bufferLength);
	runicast_send(&doorRunicastConnection, &doorNodeAddress, MAX_RETRANSMISSIONS);
	free(buffer);
}

void sendGateNode(unsigned char* c, int bytes)
{
	unsigned char* buffer;
	char bufferLength = setBuffer(&buffer, c, bytes, CENTRAL_UNIT_HIGH, GATE_NODE_HIGH);
	packetbuf_copyfrom(buffer, bufferLength);
	runicast_send(&gateRunicastConnection, &gateNodeAddress, MAX_RETRANSMISSIONS);
	free(buffer);
}

void forward(unsigned char* buffer, int bytes, char nextHop)
{
	packetbuf_copyfrom(buffer, bytes);
	switch(nextHop)
	{
		case RL_NODE_HIGH:
			runicast_send(&rlRunicastConnection, &rlNodeAddress, MAX_RETRANSMISSIONS);
			break;
		case GATE_NODE_HIGH:
			runicast_send(&gateRunicastConnection, &gateNodeAddress, MAX_RETRANSMISSIONS);
			break;
		case DOOR_NODE_HIGH:
			runicast_send(&doorRunicastConnection, &doorNodeAddress, MAX_RETRANSMISSIONS);
			break;
		case HT_NODE_HIGH:
			runicast_send(&htRunicastConnection, &htNodeAddress, MAX_RETRANSMISSIONS);
			break;
		default:
			printf("Invalid next hop\n");
	}
}

void initCURimeStack()
{
	setNodesAddresses();
	
	printf("My address is %d.%d\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	
	runicast_open(&doorRunicastConnection, CU_DOOR_CHANNEL, &runicast_calls);
	runicast_open(&gateRunicastConnection, CU_GATE_CHANNEL, &runicast_calls);
	runicast_open(&htRunicastConnection, HT_CU_CHANNEL, &runicast_calls);
	runicast_open(&rlRunicastConnection, CU_RL_CHANNEL, &runicast_calls);
}
