//***************************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2010				*
//*  Developed at the Networks and Pervasive Computing program				*
//*  Author(s): Athanassios Boulis, Yuriy Tselishchev						*
//*  This file is distributed under the terms in the attached LICENSE file.		*
//*  If you do not find this file, copies can be found by writing to:			*
//*											*
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia			*
//*      Attention:  License Inquiry.							*
//*											*
//***************************************************************************************



#ifndef TUNABLEMACMODULE
#define TUNABLEMACMODULE

#include "VirtualMacModule.h"
#include "TunableMacControl_m.h"
#include "TunableMacSimpleFrame_m.h"

using namespace std;

enum backoffTypes
{
    BACKOFF_SLEEP_INT = 2200,
    BACKOFF_CONSTANT = 2201,
    BACKOFF_MULTIPLYING = 2202,
    BACKOFF_EXPONENTIAL = 2203
};

enum MacStates
{
    MAC_STATE_DEFAULT = 1,
    MAC_STATE_TX = 2,
    MAC_STATE_CONTENDING = 3,
    MAC_STATE_RX = 4
};

enum Timers
{
    START_SLEEPING = 1,
    START_LISTENING = 2,
    START_CARRIER_SENSING = 3,
    ATTEMPT_TX = 4,
    SEND_BEACONS_OR_DATA = 5,
};

class TunableMacModule : public VirtualMacModule
{
    private:
	/*--- The .ned file's parameters ---*/
	double dutyCycle;		// sleeping interval / sleeping + listening intervals
	double listenInterval;		// in secs, note: parammeter in omnetpp.ini in msecs
	double beaconIntervalFraction;
	double probTx;			// probability of a single transmission to happen
	int numTx;			// when we have something to send, how many times do we try to try to transmit it. We say "try" because probTx might be < 1
	double randomTxOffset;		// when have somethingnto transmit, don't do it immediatelly
	double reTxInterval;		// the interval between retransmissions, in msec but after a time [0..randomTxOffset] chosen randomly (uniform)
	int beaconFrameSize;		//in bytes
	int backoffType;		//can be 0 or 1 or 2 or 3
	double backoffBaseValue;	//the backoff value

	int phyLayerOverhead;
	simtime_t phyDelayForValidCS;		// delay for valid CS
	double phyDataRate;

	int macState;
	int numTxTries;
	int backoffTimes;		// number of consequtive backoff times
	int remainingBeaconsToTx;

	double sleepInterval;		// in secs

    protected:
	void startup();
	void fromNetworkLayer(cPacket *, int);
	void fromRadioLayer(cPacket *, double, double);
	int handleControlCommand(cMessage *msg);
	void timerFiredCallback(int);
	void carrierSenseCallback(int);
	void attemptTx();
	void sendBeaconsOrData();
};

#endif //TUNABLEMACMODULE
