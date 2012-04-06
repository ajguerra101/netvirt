/*
 * Dynamic Network Directory Service
 * Copyright (C) 2010-2012 Nicolas Bouliane
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 *
 */

#include <dnds/dnds.h>
#include <dnds/journal.h>
#include <dnds/net.h>
#include <dnds/pki.h>

#include "context.h"
#include "dsc.h"

netc_t *netc;

int transmit_peerconnectinfo(e_ConnectState state, char *ipAddress, char *certName)
{
	DNDSMessage_t *msg;

	DNDSMessage_new(&msg);
	DNDSMessage_set_channel(msg, 0);
	DNDSMessage_set_pdu(msg, pdu_PR_dsm);

        DSMessage_set_seqNumber(msg, 0);
        DSMessage_set_ackNumber(msg, 0);
        DSMessage_set_operation(msg, dsop_PR_peerConnectInfo);

        PeerConnectInfo_set_certName(msg, certName, strlen(certName));
        PeerConnectInfo_set_ipAddr(msg, ipAddress);
        PeerConnectInfo_set_state(msg, state);  

	net_send_msg(netc, msg);

	return 0;
}

static void on_secure(netc_t *netc)
{
	JOURNAL_DEBUG("dsc on secure");
}

static void handle_contextinfo(netc_t *netc, DNDSMessage_t *msg)
{
	size_t length;
	uint32_t id;
	char network[INET_ADDRSTRLEN];
	char netmask[INET_ADDRSTRLEN];

//	ContextInfo_printf(msg);
	ContextInfo_get_id(msg, &id);
	ContextInfo_get_network(msg, network);
	ContextInfo_get_netmask(msg, netmask);

	char *serverCert;
	char *serverPrivkey;
	char *trustedCert;

	ContextInfo_get_serverCert(msg, &serverCert, &length);
	ContextInfo_get_serverPrivkey(msg, &serverPrivkey, &length);
	ContextInfo_get_trustedCert(msg, &trustedCert, &length);

	context_create(id, network, netmask, serverCert, serverPrivkey, trustedCert);
}

static void dispatch_operation(netc_t *netc, DNDSMessage_t *msg)
{
	dsop_PR operation;

	DSMessage_get_operation(msg, &operation);

	printf("dispatch operation\n");
//	switch (operation) {
//	case dsop_PR_contextInfo:
		handle_contextinfo(netc, msg);
//		break;
//	}
}

static void on_input(netc_t *netc)
{
	DNDSMessage_t *msg;
	mbuf_t **mbuf_itr;
	pdu_PR pdu;

	mbuf_itr = &netc->queue_msg;

	while (*mbuf_itr != NULL) {

		msg = (DNDSMessage_t *)(*mbuf_itr)->ext_buf;
		DNDSMessage_get_pdu(msg, &pdu);

		switch (pdu) {
			case pdu_PR_dsm:	/* DNDS protocol */
				dispatch_operation(netc, msg);
				break;
			default:
				/* TODO disconnect session */
				JOURNAL_ERR("dnd]> invalid PDU");
				break;
		}

		mbuf_del(mbuf_itr, *mbuf_itr);
	}
}

static void on_connect(netc_t *netc)
{
	JOURNAL_DEBUG("dsc on connect");
}


static void on_disconnect(netc_t *netc)
{
	JOURNAL_DEBUG("dsc on disconnect");
}

void dsc_fini(void *ext_ptr)
{

}

int dsc_init(char *ip_address, char *port, char *certificate, char *privatekey, char *trusted_authority)
{
	passport_t *dnd_passport;
	dnd_passport = pki_passport_load_from_file(certificate, privatekey, trusted_authority);

	netc = net_client(ip_address, port, NET_PROTO_UDT, NET_SECURE_RSA, dnd_passport,
				on_disconnect, on_input, on_secure);

	if (netc == NULL) {
		JOURNAL_NOTICE("dnd]> failed to connect to the Directory Service :: %s:%i\n", __FILE__, __LINE__);
		return -1;
	}

	return 0;
}
