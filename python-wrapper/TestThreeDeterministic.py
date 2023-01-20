#!/usr/bin/python3
from TestFMU3 import testFMU3

model_path =    '../fmi3/build/dist'
model_name =    'Pipeline_three_deterministic'
instance_name = 'test_sim_ict'
visible =       False
debug_logging=  True
event_mode=     True
early_return=   True

testFMU3(model_path, model_name, instance_name, visible, debug_logging, event_mode, early_return)
