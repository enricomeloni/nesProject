#include "roomLightRimeStack.h"
#include "commons/addresses.h"
#include "commons/constants.h"

#include "stdio.h"
#include "net/rime/rime.h"

extern void processHTCommand(unsigned char command);

static void recv_runicast(struct runicast_conn *c, const linkaddr_t *from, uint8_t seqno)
{
	unsigned char* buffer = packetbuf_dataptr();
	char srcNode = *buffer;
	char dstNode = *(buffer+1);
	char payloadSize = *(buffer+2);
	unsigned char* payload = buffer+3;
	
	printf("runicast message received from %d.%d, seqno: %d\n",
		   from->u8[0],
		   from->u8[1],
		   seqno);

	if(dstNode != RL_NODE_HIGH)
		return; //this node doesn't forward
	if(srcNode == HT_NODE_HIGH)
	{
		if(payloadSize == 1)
		{
			char receivedCommand = *payload;
			processHTCommand(receivedCommand);
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

void initLightRimeStack()
{
	setNodesAddresses();
	printf("My address is %d.%d\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	runicast_open(&cuRunicastConnection, CU_RL_CHANNEL, &runicast_calls);
}