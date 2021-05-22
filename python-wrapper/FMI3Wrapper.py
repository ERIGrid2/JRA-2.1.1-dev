from ctypes import *
import inspect
import sys
import os.path
import xml.etree.ElementTree
import urllib.parse as urlparse, urllib.request as urllib
from dataclasses import dataclass


def py_logger( c, instance_name, status, category, message ):
    print( '[{}] {}: {}'.format(
        instance_name.decode( 'utf-8' ),
        category.decode( 'utf-8' ),
        message.decode( 'utf-8' )
        ) )

@dataclass
class Step_result:
    status: int
    event_encountered: bool
    terminate_simulation: bool
    early_return: bool
    last_successful_time: float

@dataclass
class Discrete_update_result:
    status:int
    discrete_states_need_update: bool
    terminate_simulation: bool
    nominals_of_continuous_states_changed: bool
    values_of_continuous_states_changed: bool
    next_event_time_defined: bool
    next_event_time: float

class FMI3Wrapper:

    fmi3_clock_active = True
    fmi3_true = 1
    fmi3_false = 0

    fmi3_ok = 0
    fmi3_warning = 1
#    fmi_discard = 2
#    fmi_error = 3
#    fmi_fatal = 4
#    fmi_pending = 5

    fmi_component=None

    ctypes_signatures=["<class '_ctypes._SimpleCData'>","<class '_ctypes._Pointer'>"]

    fmi_fn_prototypes=dict([
        ('fmi3GetVersion',{'retval':c_char_p , 'args':[]}),
        ('fmi3InstantiateCoSimulation',{'retval':c_void_p,
                                        'args':[c_char_p,c_char_p,c_char_p,c_char,c_char,c_char,c_char,POINTER(c_int),c_size_t,c_void_p,c_void_p,c_void_p]}),
        ('fmi3FreeInstance',{'retval':c_int , 'args':[c_void_p]}),
        ('fmi3EnterInitializationMode',{'retval':c_int , 'args':[c_void_p,c_char,c_double,c_double,c_char,c_double]}),
        ('fmi3ExitInitializationMode',{'retval':c_int , 'args':[c_void_p]}),
        ('fmi3Terminate',{'retval':c_int ,'args':[c_void_p]}),
        ('fmi3SetInt32',{'retval':c_int, 'args':[c_void_p,POINTER(c_int),c_size_t,POINTER(c_int),c_size_t]}),
        ('fmi3GetInt32',{'retval':c_int, 'args':[c_void_p,POINTER(c_int),c_size_t,POINTER(c_int),c_size_t]}),
        ('fmi3GetClock',{'retval':c_int, 'args':[c_void_p,POINTER(c_int),c_size_t,POINTER(c_bool),c_size_t]}),
        ('fmi3SetClock',{'retval':c_int, 'args':[c_void_p,POINTER(c_int),c_size_t,POINTER(c_char),c_size_t]}), # CHANGE TO BOOL PTR?
        ('fmi3DoStep',{'retval':c_int ,'args':[c_void_p,c_double,c_double,c_char,POINTER(c_char),POINTER(c_char),POINTER(c_char),POINTER(c_double)]}),
        ('fmi3EnterEventMode',{'retval':c_int, 'args':[c_void_p,c_char,c_char,POINTER(c_int),c_size_t,c_char]}),
        ('fmi3EnterStepMode',{'retval':c_int, 'args':[c_void_p]}),
        ('fmi3UpdateDiscreteStates',{'retval':c_int, 'args':[c_void_p,POINTER(c_char),POINTER(c_char),POINTER(c_char),POINTER(c_char),POINTER(c_char),POINTER(c_double)]}),
        
        ])

    fmi_functions=dict()

    fmi_callback_prototypes=dict([
        ('logMessage',{'args':[c_void_p,c_char_p,c_int,c_char_p,c_char_p]}),
        ('intermediateUpdate',{'retval':c_void_p, 'args':[]}),
        
        
        ])

    fmi_callbacks=dict()
    
    def call(self,fname):
        return self.fmi_functions[fname]

    def __init__( self, fmu_model_description, fmu_path ):
        
        self.fmu_path = fmu_path

        # Parse model description and set up variables
        self.fmu_var_dict=fmu_model_description.fmu_var_dict

        # Load the FMU shared library.
        self.__load_shared_library(fmu_model_description.fmu_name)

        # Set callback functions.
#        self.__set_callbacks()

        # Initialize the FMU functions.
        self.__init_functions()


    def __del__( self ):
        self.fmi3Terminate()
        self.fmi3FreeInstance()


    def __load_shared_library( self, fmu_name ):
        if not os.path.isdir( self.fmu_path ):
            raise IOError( 'directory not found: {}'.format( self.fmu_path ) )

        # Construct path to shared library from FMU name and path.
        if sys.platform == 'cygwin':
            fmu_shared_library_path = \
                os.path.join( self.fmu_path, fmu_name, 'binaries', 'cygwin32', fmu_name + '.dll' )
        else:
            fmu_shared_library_path = \
                os.path.join( self.fmu_path, fmu_name, 'binaries', 'x86_64-linux', fmu_name + '.so' )

        # Load shared library.
        self.fmu_shared_library = cdll.LoadLibrary( fmu_shared_library_path )

    def __init_functions( self ):
        # set up callbacks first because some of the regular functions depend on them
        #for cname in self.fmi_callback_prototypes:
        #    cobject=CALLBACK(self.fmu_shared_library, cname)
        #    retval=self.fmi_callback_prototypes[cname].get('retval')
        #    if retval:
        #        if not inspect.isclass(retval) or not str(inspect.getmro(retval)[1])=="<class '_ctypes._SimpleCData'>":
        #            sys.exit('Return value for callback ',cname,' is not a ctypes datatype.')
        #        cobject.restype=retval
        #    args=self.fmi_callback_prototypes[cname].get('args')
        #    if args:
        #        argtypes=()
        #        for arg in args:
        #            if not inspect.isclass(arg) or not str(inspect.getmro(arg)[1])=="<class '_ctypes._SimpleCData'>":
        #                sys.exit('Argument for callback ',cname,' is not a ctypes datatype.')
        #            argtypes=argtypes+(arg,)
        #        cobject.argtypes=argtypes
        #    self.fmi_callbacks[cname]=cobject
        
        # now set up the regular functions
        for fname in self.fmi_fn_prototypes:
            fobject=getattr(self.fmu_shared_library, fname)
            retval=self.fmi_fn_prototypes[fname].get('retval')
            if retval:
                if not inspect.isclass(retval) or not str(inspect.getmro(retval)[1]) in self.ctypes_signatures:
                    sys.exit('Return value for function ',fname,' is not a ctypes datatype.')
                fobject.restype=retval
            args=self.fmi_fn_prototypes[fname].get('args')
            if args:
                argtypes=()
                for arg in args:
                    if not inspect.isclass(arg) or not str(inspect.getmro(arg)[1]) in self.ctypes_signatures:
                        sys.exit('Argument for function '+fname+' is not a ctypes datatype.')
                    argtypes=argtypes+(arg,)
                fobject.argtypes=argtypes
            self.fmi_functions[fname]=fobject
            
    def bool_to_c_char(self,b):
        return c_char(self.fmi3_true if b is True else self.fmi3_false)

    def list_to_cint_array(self,l):
        nl = len(l)
        return (( c_int * nl )( *l ),
            c_size_t( nl ))

    def bool_list_to_cchar_array(self,l):
        nl = len(l)
        return (( c_char * nl )( *[self.fmi3_true if x is True else self.fmi3_false for x in l] ),
            c_size_t( nl ))
        

#####################################################
### Creation and destruction of FMU instances
#####################################################

    def fmi3InstantiateCoSimulation( self, instanceName, instantiationToken, resourceLocation, visible, loggingOn, eventModeUsed, earlyReturnAllowed,
                                     requiredIntermediateVariables, instanceEnvironment, logcallb, intupd_callback):
        var_ref_ids = []
        if requiredIntermediateVariables:
            n_vars = len(requiredIntermediateVariables)
            for name in requiredIntermediateVariables:
                var_ref_ids.append( self.fmu_var_dict[ name ] )
        else:
            n_vars=0            
        var_values = ( c_int * n_vars )()
        self.fmi_component=self.call('fmi3InstantiateCoSimulation')(
            bytes(instanceName, encoding='utf-8'),bytes(instantiationToken, encoding='utf-8'),bytes(resourceLocation, encoding='utf-8'),
            c_char(self.fmi3_true if visible else self.fmi3_false),
            c_char(self.fmi3_true if loggingOn else self.fmi3_false),
            c_char(self.fmi3_true if eventModeUsed else self.fmi3_false),
            c_char(self.fmi3_true if earlyReturnAllowed else self.fmi3_false),
            ( c_int * n_vars )( *var_ref_ids ),c_size_t(n_vars),
            instanceEnvironment,
            logcallb,
            intupd_callback)
        assert(self.fmi_component is not None)
        return self.fmi_component                    

    def fmi3FreeInstance( self ):
        if self.fmi_component:
            self.call('fmi3FreeInstance')(
                self.fmi_component
            )
        self.fmi_component = None
    
###########################################################
### Enter and exit initialization mode, terminate and reset
###########################################################

    def fmi3EnterInitializationMode( self, tolerance, start_time, stop_time = 0, tolerance_defined=False, stop_time_defined = False ):
        if not self.fmi_component:
            raise RuntimeError('Trying to initialize an uninstantiated FMU')
        status = self.call('fmi3EnterInitializationMode')(
            self.fmi_component,
            self.bool_to_c_char(tolerance_defined),
            c_double(tolerance),
            c_double(start_time),
            self.bool_to_c_char(stop_time_defined),
            c_double(stop_time),
            )
        assert( status == self.fmi3_ok )
        return status
    
    def fmi3ExitInitializationMode( self ):
        if not self.fmi_component:
            raise RuntimeError('Trying to initialize an uninstantiated FMU')
        status = self.call('fmi3ExitInitializationMode')(
            self.fmi_component
            )
        assert( status == self.fmi3_ok )
        return status

    def fmi3EnterEventMode( self, step_event, state_event, roots_found, time_event ):
        if not self.fmi_component:
            raise RuntimeError('Trying to initialize an uninstantiated FMU')
        n_roots = len(roots_found)
        status = self.call('fmi3EnterEventMode')(
            self.fmi_component,
            self.bool_to_c_char(step_event),
            self.bool_to_c_char(state_event),
            ( c_int * n_roots )( *roots_found ),
            c_size_t(n_roots),
            self.bool_to_c_char(time_event)
            )
        assert( status == self.fmi3_ok )
        return status

    def fmi3Terminate( self ):
        if self.fmi_component:
            status=self.call('fmi3Terminate')(
            self.fmi_component
            )
            # Check the FMU status.
            assert( status == self.fmi3_ok  )
            return status
        return self.fmi3_warning


###########################################################
### Getting and setting variables values
###########################################################

    def fmi3SetInt32( self, value_references, values):
        if not self.fmi_component:
            raise RuntimeError('Trying to set a variable in an uninstantiated FMU')
        var_ref_ids = []
        for name in value_references:
            var_ref_ids.append( self.fmu_var_dict[name] )
        status = self.call('fmi3SetInt32')(
            self.fmi_component,
            *self.list_to_cint_array(var_ref_ids),
            *self.list_to_cint_array(values)
            )
        assert( status == self.fmi3_ok )
        return status

    def fmi3GetInt32( self, value_references):
        if not self.fmi_component:
            raise RuntimeError('Trying to get a variable in an uninstantiated FMU')
        var_ref_ids = []
        for name in value_references:
            var_ref_ids.append( self.fmu_var_dict[name] )
        nvars=len(var_ref_ids)
        var_values = (c_int*nvars)()
        status = self.call('fmi3GetInt32')(
            self.fmi_component,
            *self.list_to_cint_array(var_ref_ids),
            var_values,
            nvars
            )
        assert( status == self.fmi3_ok )
        return var_values

#####################################################
### Clock related Functions
#####################################################

    def fmi3GetClock( self, value_references):
        if not self.fmi_component:
            raise RuntimeError('Trying to get a variable in an uninstantiated FMU')
        var_ref_ids = []
        for name in value_references:
            var_ref_ids.append( self.fmu_var_dict[name] )
        nvars=len(var_ref_ids)
        var_values = (c_bool*nvars)()
        status = self.call('fmi3GetClock')(
            self.fmi_component,
            *self.list_to_cint_array(var_ref_ids),
            var_values,
            nvars
            )
        assert( status == self.fmi3_ok )
        return var_values

    def fmi3SetClock( self, value_references, values):
        if not self.fmi_component:
            raise RuntimeError('Trying to set a variable in an uninstantiated FMU')
        var_ref_ids = []
        for name in value_references:
            var_ref_ids.append( self.fmu_var_dict[name] )
        status = self.call('fmi3SetClock')(
            self.fmi_component,
            *self.list_to_cint_array(var_ref_ids),
            *self.bool_list_to_cchar_array(values)
            )
        assert( status == self.fmi3_ok )
        return status

    def fmi3UpdateDiscreteStates( self, next_event_time ):
        if not self.fmi_component:
            raise RuntimeError('Trying to update an uninstantiated FMU')
        result=Discrete_update_result(0,False,False,False,False,False,next_event_time)
        xds=self.bool_to_c_char(result.discrete_states_need_update)
        xts=self.bool_to_c_char(result.terminate_simulation)
        xnc=self.bool_to_c_char(result.nominals_of_continuous_states_changed)
        xvc=self.bool_to_c_char(result.values_of_continuous_states_changed)
        xnd=self.bool_to_c_char(result.next_event_time_defined)
        xnt=c_double(result.next_event_time)
        status = self.call('fmi3UpdateDiscreteStates')(
            self.fmi_component,
            byref(xds),
            byref(xts),
            byref(xnc),
            byref(xvc),
            byref(xnd),
            byref(xnt)
            )
        assert( status == self.fmi3_ok )
        result.status=status
        result.discrete_states_need_update=bool(xds)
        result.terminate_simulation=bool(xts)
        result.nominals_of_continuous_states_changed=bool(xnc)
        result.values_of_continuous_states_changed=bool(xvc)
        result.next_event_time_defined=bool(xnd)
        result.next_event_time=xnt.value
        return result

#####################################################
### Functions for Co-Simulation
#####################################################

    def fmi3DoStep( self, current_communication_point, communication_step_size, no_set_FMU_state_prior_to_current_point, last_successful_time):
        if not self.fmi_component:
            raise RuntimeError('Trying to step an uninstantiated FMU')
        result=Step_result(0,False,False,False,last_successful_time)
        xee=self.bool_to_c_char(result.event_encountered)
        xts=self.bool_to_c_char(result.terminate_simulation)
        xer=self.bool_to_c_char(result.early_return)
        xls=c_double(result.last_successful_time)
        status = self.call('fmi3DoStep')(
            self.fmi_component,
            c_double(current_communication_point),
            c_double(communication_step_size),
            self.bool_to_c_char(no_set_FMU_state_prior_to_current_point),
            byref(xee),
            byref(xts),
            byref(xer),
            byref(xls)
            )
        assert( status == self.fmi3_ok )
        result.status=status
        result.event_encountered=bool(xee)
        result.terminate_simulation=bool(xts)
        result.early_return=bool(xer)
        result.last_successful_time=xls.value
        return result

    def fmi3EnterStepMode ( self ):
        if not self.fmi_component:
            raise RuntimeError('Trying to change mode on an uninstantiated FMU')
        status = self.call('fmi3EnterStepMode')(
            self.fmi_component
            )
        assert( status == self.fmi3_ok )
        return status
