//***************************************************************************************
//*  Copyright: National ICT Australia,  2010						*
//*  Developed at the Networks and Pervasive Computing program				*
//*  Author(s): Yuriy Tselishchev							*
//*  This file is distributed under the terms in the attached LICENSE file.		*
//*  If you do not find this file, copies can be found by writing to:			*
//*											*
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia			*
//*      Attention:  License Inquiry.							*
//*											*
//***************************************************************************************



#include "VirtualCastaliaModule.h"

#define CASTALIA_PREFIX "Castalia|\t"

void VirtualCastaliaModule::finish() {
    finishSpecific();
    if (simpleoutputs.size() == 0 && histograms.size() == 0) return; 
    EV << CASTALIA_PREFIX << "module:" << getFullPath() << endl;
    simpleOutputMapType::iterator i;
    for (i = simpleoutputs.begin(); i != simpleoutputs.end(); i++) {
	outputKeyDef key = i->first;
	simpleOutputTypeDef out = i->second;
	if (out.data.size() != 0) {
	    EV << CASTALIA_PREFIX << "\t";
	    if (key.index >= 0) EV << " index:" << key.index << " ";
	    EV << "simple output name:" << key.descr << endl;
	    map<string, double>::iterator i2;
	    for (i2 = out.data.begin(); i2 != out.data.end(); i2++) {
	        EV << CASTALIA_PREFIX "\t\t" << i2->second << " " << i2->first << endl;
	    }
	}
    }
    simpleoutputs.clear();
    
    histogramOutputMapType::iterator i3;
    for (i3 = histograms.begin(); i3 != histograms.end(); i3++) {
	outputKeyDef key = i3->first;
	histogramOutputTypeDef hist = i3->second;
	if (!hist.active) { continue; }
	EV << CASTALIA_PREFIX << "\t";
	if (key.index >= 0) EV << " index:" << key.index << " ";
	EV << "histogram name:" << key.descr << endl;
	EV << CASTALIA_PREFIX << "\thistogram_min:" << hist.min << " histogram_max:" << hist.max << endl;
	EV << CASTALIA_PREFIX << "\thistogram_values";
	for (int i = 0; i < hist.numBuckets+2; i++) {
	    EV << " " << hist.buckets[i];
	}
	EV << endl;
    }
    histograms.clear();
}

std::ostream &VirtualCastaliaModule::trace() {
    if (hasPar("collectTraceInfo") && par("collectTraceInfo")) {
	return (ostream&)DebugInfoWriter::getStream() << "\n" << setw(16) << simTime() << setw(40) << getFullPath() << " ";
    } else {
	return empty;
    }
}

std::ostream &VirtualCastaliaModule::debug() {
    return cerr;
}

void VirtualCastaliaModule::declareOutput(const char* descr) {
    outputKeyDef key(descr,-1);
    simpleoutputs[key].data.clear();
}

void VirtualCastaliaModule::declareOutput(const char* descr, int index) {
    if (index < 0) opp_error("Negative output index not permitted");
    outputKeyDef key(descr,index);
    simpleoutputs[key].data.clear();
}

void VirtualCastaliaModule::collectOutput(const char* descr, int index, const char* label, double amt) {
    if (index < 0) opp_error("Negative output index not permitted");
    collectOutputNocheck(descr,index,label,amt);
}

void VirtualCastaliaModule::collectOutputNocheck(const char* descr, int index, const char* label, double amt) {
    outputKeyDef key(descr,index);
    simpleOutputMapType::iterator i = simpleoutputs.find(key);
    if (i != simpleoutputs.end()) {
	simpleoutputs[key].data[label] += amt;
    }
}

void VirtualCastaliaModule::collectOutput(const char *descr, int index, const char *label) {
    if (index < 0) opp_error("Negative output index not permitted");
    collectOutputNocheck(descr,index,label,1); 
}
            

void VirtualCastaliaModule::declareHistogramNocheck(const char* descr, double min, double max, int buckets, int index) {
    if (min >= max || buckets < 1)
	opp_error("ERROR: declareHistogram failed, bad parameters");

    outputKeyDef key(descr,index);
    histograms[key].buckets.clear();
    histograms[key].buckets.resize(buckets+2);
    for (int i = 0; i < buckets+2; i++) {
	histograms[key].buckets[i] = 0;
    }
    histograms[key].min = min;
    histograms[key].max = max;
    histograms[key].cell = (max - min)/buckets;
    histograms[key].numBuckets = buckets;
    histograms[key].active = false;
}

void VirtualCastaliaModule::declareHistogram(const char* descr, double min, double max, int buckets, int index) {
    if (index < 0) opp_error("Negative output index not permitted");
    declareHistogramNocheck(descr,min,max,buckets,index);
}
                                                            
void VirtualCastaliaModule::collectHistogram(const char* descr, int index, double value) {
    if (index < 0) opp_error("Negative output index not permitted");
    collectHistogramNocheck(descr,index,value);
}

void VirtualCastaliaModule::collectHistogramNocheck(const char* descr, int index, double value) {
    outputKeyDef key(descr,index);
    histogramOutputMapType::iterator i = histograms.find(key);
    if(i != histograms.end()) {
	int num;
	if (value < histograms[key].min) {
	    num = 0;
	} else if (value >= histograms[key].max) {
	    num = histograms[key].numBuckets + 1;
	} else {
	    num = (int)ceil((value - histograms[key].min)/histograms[key].cell);
	}
	histograms[key].buckets[num]++;
	histograms[key].active = true;
    }
}

void VirtualCastaliaModule::powerDrawn(double power) {
/*
    if (!resourceManager) {
	string name(getName());
	if (name.compare("Radio") && name.compare("nodeSensorDevMgr")) 
	    opp_error("%s module has no rights to call drawPower() function",getFullPath().c_str());
	
	cModule *tmp = getParentModule();
	while (tmp) {
	    string name(tmp->getName());
	    if (name.compare("node") == 0) {
		resourceManager = tmp->getSubmodule("nodeResourceMgr");
		break;
	    }
	    tmp = tmp->getParentModule();
	}
        if (!resourceManager) opp_error("Unable to find pointer to resource manager module");
    }
*/
    ResourceManagerMessage *drawPowerMsg = new ResourceManagerMessage("Power consumption message",RESOURCE_MANAGER_DRAW_POWER);
    drawPowerMsg->setPowerConsumed(power);
    
    string name(getName());
    if (name.compare("Radio") == 0) {    
	sendDirect(drawPowerMsg,getParentModule()->getParentModule()->getSubmodule("nodeResourceMgr"),"powerConsumption");
    } else if (name.compare("nodeSensorDevMgr") == 0) {
	sendDirect(drawPowerMsg,getParentModule()->getSubmodule("nodeResourceMgr"),"powerConsumption");
    } else {
        opp_error("%s module has no rights to call drawPower() function",getFullPath().c_str());
    } 
//    sendDirect(drawPowerMsg,resourceManager,"powerConsumption");
}
