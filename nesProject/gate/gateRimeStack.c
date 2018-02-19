//
// Created by Raff on 31/10/2017.
//

#include "gateRimeStack.h"
#include "commons/addresses.h"
#include "commons/constants.h"

#include <stdio.h>
#include <stdlib.h>
#include "net/rime/rime.h"

extern void processCUCommand(unsigned char command);
extern void setNodesAddresses();

static void recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno)
{
	unsigned char* buffer = packetbuf_dataptr();
	char srcNode = *buffer;
	char dstNode = *(buffer+1);
	char payloadSize = *(buffer+2);
	unsigned char* payload = buffer+3;
	
	printf("runicast message received from %d.%d, seqno: %d, cmd: %d\n",
		   from->u8[0],
		   from->u8[1],
		   seqno,
		   *payload);
	
	if(dstNode != GATE_NODE_HIGH)
		return; //this node doesn't forward
	if(srcNode == CENTRAL_UNIT_HIGH)
	{
		//retrieve command
		if(payloadSize == 1)
		{
			char receivedCommand = *payload;
			processCUCommand(receivedCommand);
		}
		else
			printf("Invalid payload size\n");
	}
	else
	{
		printf("Message from unexpected node: refused\n");
	}
}

static void sent_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions){}

static void timedout_runicast(struct runicast_conn *c, const linkaddr_t *to, uint8_t retransmissions){}

static const struct runicast_callbacks runicast_calls = {recv_runicast, sent_runicast, timedout_runicast};
static struct runicast_conn cuRunicastConnection;

void initGateRimeStack()
{
	setNodesAddresses();
	
	printf("My address is %d.%d\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	
	runicast_open(&cuRunicastConnection, CU_GATE_CHANNEL, &runicast_calls);
}

void sendFromGateToCentralUnit(unsigned char *cmd, int bytes)
{
	unsigned char* buffer;
	char bufferLength = setBuffer(&buffer, cmd, bytes, GATE_NODE_HIGH, CENTRAL_UNIT_HIGH);
    packetbuf_copyfrom(buffer, bufferLength);
    runicast_send(&cuRunicastConnection, &centralNodeAddress, MAX_RETRANSMISSIONS);
	free(buffer);
}