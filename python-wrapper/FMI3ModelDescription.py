import xml.etree.ElementTree
import os.path

class FMI3ModelDescription:

    fmu_name="unknown"
    fmi_version="unknown"
    model_name="unknown"
    instantiation_token="{0}"
    fmu_var_dict=dict()

    def parse_model_description( self, fmu_name, fmu_path ):
        self.fmu_name=fmu_name
        # Parse and store the model description.
        fmu_model_description_path = os.path.join( fmu_path, fmu_name, 'modelDescription.xml' )
        fmu_model_description = xml.etree.ElementTree.parse( fmu_model_description_path ).getroot()
        
        assert(fmu_model_description.tag == "fmiModelDescription")
        self.fmi_version=fmu_model_description.attrib["fmiVersion"]
        self.model_name=fmu_model_description.attrib["modelName"]
        self.instantiation_token=fmu_model_description.attrib["instantiationToken"]

        # Retrieve dict of variable names and value references.
        scalar_variables = fmu_model_description.find( 'ModelVariables' )
        self.fmu_var_dict = dict()
        for var in list(scalar_variables):
            self.fmu_var_dict[var.get('name')] = int(var.get('valueReference'))
