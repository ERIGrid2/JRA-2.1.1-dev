/**************************************************************************
 * Copyright (c) ERIGrid 2.0 (H2020 Programme Grant Agreement No. 870620) *
 * All rights reserved.                                                   *
 * See file LICENSE in the project root for license information.          *
 **************************************************************************/

#ifndef NetworkConfiguration_h
#define NetworkConfiguration_h
#include "rapidjson/document.h"
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

#define ACCEPTS_FORMAT "1.0"
#define ERROR_PREFIX "Network configuration file: "
#define TAG_NDESC "networkDescription"
#define TAG_PIPES "pipelines"
#define ATTR_FILEFMT "format"
#define ATTR_NWKNAME "name"
#define ATTR_PIPENAME "name"
#define ATTR_PIPEDLY "delay"
#define ATTR_PIPEJTR "jitter"
#define ATTR_PIPELSS "loss"
#define ATTR_STARTNODE "startNode"
#define ATTR_ENDNODE "endNode"

class NetworkNode {
    
    private:
        
        std::string nodeName;
        std::vector<std::string> inPipes;
        std::vector<std::string> outPipes;
        
    public:
        
        NetworkNode(std::string n) {
            nodeName=n;
        }
        
        std::string getNodeName() {return nodeName;}
        
        void addInput(std::string pipe) {inPipes.push_back(pipe);}
        
        void addOutput(std::string pipe) {outPipes.push_back(pipe);}
        
        bool isInputNode() {return (!outPipes.empty() && inPipes.empty());}
        
        bool isOutputNode() {return (!inPipes.empty() && outPipes.empty());}

};

class NetworkPipeline {
    
    private:
    
        std::string pipeName;
        std::string startNodeName;
        NetworkNode* startNode;
        std::string endNodeName;
        NetworkNode* endNode;
        double delay;
        double jitter;
        double loss;
        fmi3Int32* receiver;
        fmi3Clock* receiverClock;
        
    public:
        
        NetworkPipeline(std::string n, std::string s, std::string e, std::string d, std::string j, std::string l) {
            pipeName=n;
            startNodeName=s;
            endNodeName=e;
            delay=stod(d);
            jitter=stod(j);
            loss=stod(l);
            receiver=NULL;
            receiverClock=NULL;
        }
        
        bool connectNodes(std::map<std::string,NetworkNode>* nodes) {
            startNode=NULL;
            endNode=NULL;
            std::map<std::string,NetworkNode>::iterator sit=nodes->find(startNodeName);
            std::map<std::string,NetworkNode>::iterator eit=nodes->find(endNodeName);
            if (sit != nodes->end()) {
                startNode=&(sit->second);
            }
            else throw std::runtime_error(ERROR_PREFIX "Pipeline: Unknown start node");
            if (eit != nodes->end()) {
                endNode=&(eit->second);
            }
            else throw std::runtime_error(ERROR_PREFIX "Pipeline: Unknown end node");
            
            return (startNode!=NULL && endNode!=NULL);
        }
    
};

class NetworkConfiguration {

  private:
      
    std::string networkName;
    std::string formatVersion;
    std::map<std::string,NetworkNode> nodes;
    std::map<std::string,NetworkPipeline> pipelines;
    
  public:

    std::string getNetworkName() {return networkName;}
    std::string getFormatVersion() {return formatVersion;}
    
    static NetworkConfiguration parse(fmi3String filename) {
        std::ifstream myFile(filename);
        if (!myFile.good()) throw std::runtime_error("Could not open network configuration file." );
        std::ostringstream tmp;
        tmp << myFile.rdbuf();
        std::string s=tmp.str();
    
        rapidjson::Document document;
        if (document.Parse(s.c_str()).HasParseError()) {
            std::stringstream ss;
            ss << "Error parsing network configuration file (error=" << document.GetParseError() << ", offset=" << document.GetErrorOffset();
            throw std::runtime_error(ss.str());
        }
    
        if (!document.IsObject()) throw std::runtime_error(ERROR_PREFIX "root element is not a JSON object.");
        rapidjson::Value ndesc=document[TAG_NDESC].GetObject();
        if (!ndesc.IsObject()) throw std::runtime_error(ERROR_PREFIX "no network description section.");
        rapidjson::Value& fmtver=ndesc[ATTR_FILEFMT];
        if (!fmtver.IsString()) throw std::runtime_error(ERROR_PREFIX "no file format version specified.");
        NetworkConfiguration retval;
        retval.formatVersion=fmtver.GetString();
        if (retval.formatVersion!=ACCEPTS_FORMAT) throw std::runtime_error(ERROR_PREFIX "must be file format version " ACCEPTS_FORMAT);
        rapidjson::Value& nname=ndesc[ATTR_NWKNAME];
        if (!fmtver.IsString()) throw std::runtime_error(ERROR_PREFIX "no network name specified.");
        retval.networkName=nname.GetString();
        
        rapidjson::Value&  pipemap = ndesc[TAG_PIPES];
        if (!pipemap.IsArray()) throw std::runtime_error(ERROR_PREFIX "no pipelines section defined.");
        for (rapidjson::Value::ValueIterator itr = pipemap.Begin(); itr != pipemap.End(); ++itr) {
            rapidjson::Value& pipe = *itr;
            if (!pipe.IsObject()) throw std::runtime_error(ERROR_PREFIX "no pipeline objects in pipelines array.");
            rapidjson::Value& v_pnm=pipe[ATTR_PIPENAME];
            if (!v_pnm.IsString()) throw std::runtime_error(ERROR_PREFIX "no pipeline name specified.");
            std::string pipename=v_pnm.GetString();
            
            //identify pipe start and end nodes, create node objects as necessary
            rapidjson::Value& v_stn=pipe[ATTR_STARTNODE];
            if (!v_stn.IsString()) throw std::runtime_error(ERROR_PREFIX "no pipeline start node specified.");
            std::string startnode=v_stn.GetString();
            std::map<std::string,NetworkNode>::iterator nn= retval.nodes.find(startnode);
            if ( nn == retval.nodes.end() ) {
                NetworkNode nx(startnode);
                retval.nodes.insert(std::pair<std::string,NetworkNode>(startnode,nx));
                nx.addOutput(pipename);
            }
            else {
                nn->second.addOutput(pipename);
            }
            rapidjson::Value& v_enn=pipe[ATTR_ENDNODE];
            if (!v_enn.IsString()) throw std::runtime_error(ERROR_PREFIX "no pipeline end node specified.");
            std::string endnode=v_enn.GetString();
            nn= retval.nodes.find(endnode);
            if ( nn == retval.nodes.end() ) {
                NetworkNode nx(endnode);
                retval.nodes.insert(std::pair<std::string,NetworkNode>(endnode,nx));
                nx.addInput(pipename);
            }
            else {
                nn->second.addInput(pipename);
            }
            
            rapidjson::Value& v_dly=pipe[ATTR_PIPEDLY];
            if (!v_dly.IsString()) throw std::runtime_error(ERROR_PREFIX "no pipeline delay specified.");
            std::string pipedelay=v_dly.GetString();
            rapidjson::Value& v_jtr=pipe[ATTR_PIPEJTR];
            if (!v_jtr.IsString()) throw std::runtime_error(ERROR_PREFIX "no pipeline jitter specified.");
            std::string pipejitter=v_jtr.GetString();
            rapidjson::Value& v_lss=pipe[ATTR_PIPELSS];
            if (!v_lss.IsString()) throw std::runtime_error(ERROR_PREFIX "no pipeline loss specified.");
            std::string pipeloss=v_lss.GetString();
            
            NetworkPipeline px(pipename,startnode,endnode,pipedelay,pipejitter,pipeloss);
            retval.pipelines.insert(std::pair<std::string,NetworkPipeline>(pipename,px));
            
            
        } //iterate over pipes

        return retval;
    }
    
};

#endif // NetworkConfiguration_h
