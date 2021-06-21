import json
import sys
from lxml import etree

class NetworkNode:
    
    def __init__(self,name):
        self.name=name
        self.input_reference=None
        self.in_pipes=[]
        self.out_pipes=[]
        
    def addInput(self,pipe):
        self.in_pipes.append(pipe)
        
    def addOutput(self,pipe):
        self.out_pipes.append(pipe)

    def isInputNode(self):
        return (self.out_pipes and not self.in_pipes)
    
    def isOutputNode(self):
        return (self.in_pipes and not self.out_pipes)
    
    def setInputReference(self,ref):
        self.input_reference=ref
        
    def getInputReference(self):
        return self.input_reference
    
    def getInputPipes(self):
        return self.in_pipes

nodes={}

try:
    #create the basic structure of the FMI XML file
    fmi_md_el = etree.Element('fmiModelDescription', fmiVersion='3.0-beta.1', modelName='Pipeline_configurable', instantiationToken='{e1059e19-5a7b-4dd8-8ee3-6ce4fd3e0cf8}')
    cosim_el = etree.SubElement(fmi_md_el, 'CoSimulation', modelIdentifier='Pipeline_configurable', canHandleVariableCommunicationStepSize='true',
                                canReturnEarlyAfterIntermediateUpdate='true', hasEventMode='true')
    mod_vars_el = etree.SubElement(fmi_md_el, 'ModelVariables')
    mod_struc_el = etree.SubElement(fmi_md_el, 'ModelStructure')

    #start parsing the network description file
    network=json.load(open('network.json'))

    nd=network['networkDescription']
    pipes=nd['pipelines']
    pipedict={}
    
    #add pipe start+end points to corresponding nodes, create referenced nodes if they don't exist yet
    for i in pipes:
        pipedict[i['name']]=i
        start_node=i['startNode']
        if start_node not in nodes:
            nodes[start_node]=NetworkNode(start_node)
        nodes[start_node].addOutput(i['name'])
        end_node=i['endNode']
        if end_node not in nodes:
            nodes[end_node]=NetworkNode(end_node)
        nodes[end_node].addInput(i['name'])
    
    in_count=1001
    out_count=2001
    mid_nodes_present=False
    
    #generate input+input clock entries for all nodes with only outgoing pipelines.
    for n in nodes:
        if nodes[n].isInputNode():
            etree.SubElement(mod_vars_el, 'Int32', name=n, valueReference=str(in_count), causality='input', variability='discrete', clocks=str(in_count+1))
            etree.SubElement(mod_vars_el, 'Clock', name=n+'_Clock', valueReference=str(in_count+1), causality='input', variability='discrete', interval='triggered')
            nodes[n].setInputReference(in_count)
            in_count+=2
        if not nodes[n].isInputNode() and not nodes[n].isOutputNode():
            mid_nodes_present=True
    
    #generate output+output clock entries for all nodes with only incoming pipelines.
    #do this in a second round after all input nodes are processed, otherwise not all input references may be set yet and the dependency declaration won't work.
    for n in nodes:
        if nodes[n].isOutputNode():
            etree.SubElement(mod_vars_el, 'Int32', name=n, valueReference=str(out_count), causality='output', variability='discrete', clocks=str(out_count+1))
            etree.SubElement(mod_vars_el, 'Clock', name=n+'_Clock', valueReference=str(out_count+1), causality='output', variability='discrete', interval='triggered')

            #brute force depth-first search for each output node to find dependencies
            visited = set()
            dep_list=[]
            def dfs(visited, node):
                if node not in visited:
                    visited.add(node)
                    for inpipe in node.getInputPipes():
                        pipe_start_node=nodes[pipedict[inpipe]['startNode']]
                        if pipe_start_node.isInputNode():
                            dep_list.append(pipe_start_node.getInputReference())
                        else:
                            dfs(visited, pipe_start_node)
            dfs(visited, nodes[n])
            etree.SubElement(mod_struc_el, 'Output', valueReference=str(out_count), dependencies=' '.join(str(x) for x in dep_list))
            out_count+=2

    #if we have any midpoint nodes, we need to generate a dummy clock for them - otherwise the packages can't be transported between pipelines.
    if mid_nodes_present:
        etree.SubElement(mod_vars_el, 'Clock', name='__DUMMY', valueReference='999', causality='output', variability='discrete', interval='triggered')

    #write the XML file
    with open("FMI3.xml", "wb") as f:
        f.write(etree.tostring(fmi_md_el, encoding='utf-8', xml_declaration = True, pretty_print = True))
        
except :
    raise

