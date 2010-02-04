//***************************************************************************************
//*  Copyright: National ICT Australia,  2007, 2008, 2009, 2010				*
//*  Developed at the Networks and Pervasive Computing program				*
//*  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis, Yuriy Tselishchev		*
//*  This file is distributed under the terms in the attached LICENSE file.		*
//*  If you do not find this file, copies can be found by writing to:			*
//*											*
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia			*
//*      Attention:  License Inquiry.							*
//*											*
//***************************************************************************************

#ifndef _THROUGHPUTTEST_APPLICATIONMODULE_H_
#define _THROUGHPUTTEST_APPLICATIONMODULE_H_

#include "VirtualApplicationModule.h"

using namespace std;

struct packet_info
{
    map <int, bool> packets_received;
};

enum Timers {
    SEND_PACKET = 1
};

class throughputTest_ApplicationModule : public VirtualApplicationModule {
    private:
	int latencyHistogramMin;
	int latencyHistogramMax;
	int latencyHistogramBuckets;
	double packet_rate;
	string recipientAddress;
	
	/*--- Latency calculation parameters ---*/
	cLongHistogram latencyHistogram;
	int latencyOverflow;
		
	float packet_spacing;
	map <int, packet_info> packet_info_table; // this table records the number of packets received by node 0 from each other node
	int total_packets_received;
	int packets_lost_at_mac;
	int packets_lost_at_network;
	int dataSN;
		
    protected:
	void startup();
	virtual void finishSpecific();
	void update_packets_received(int srcID, int SN);
	void fromNetworkLayer(ApplicationGenericDataPacket*, const char*, const char*, double, double);
	void timerFiredCallback(int);
};

#endif // _THROUGHPUTTEST_APPLICATIONMODULE_H_
