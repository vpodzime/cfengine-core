import ctypes
from ctypes import POINTER

_sliby = ctypes.CDLL("libsliby.so.1")

class PolicyError(Exception):
    pass

class _SlibyPolicyStruct(ctypes.Structure):
    pass

_sliby.SlibyLoadPolicy.restype = ctypes.POINTER(_SlibyPolicyStruct)
_sliby.SlibyLoadPolicy.argtypes = [ctypes.c_char_p, ctypes.c_char_p]

_sliby.SlibyPolicyDestroy.restype = None
_sliby.SlibyPolicyDestroy.argtypes = [ctypes.POINTER(_SlibyPolicyStruct)]

_sliby.SlibyPolicyGetVariableValue.restype = ctypes.c_char_p
_sliby.SlibyPolicyGetVariableValue.argtypes = [ctypes.POINTER(_SlibyPolicyStruct), ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]

_sliby.SlibyPolicyGetControlBodyAttribute.restype = ctypes.c_char_p
_sliby.SlibyPolicyGetControlBodyAttribute.argtypes = [ctypes.POINTER(_SlibyPolicyStruct), ctypes.c_char_p, ctypes.c_char_p]

class Policy:
    def __init__(self, entry_file, agent_type="common"):
        self._c_struct = _sliby.SlibyLoadPolicy(entry_file.encode(), agent_type.encode())
        if not self._c_struct:
            raise PolicyError("Failed to load policy from '%s' (agent type: '%s')" % (entry_file, agent_type))

    def __del__(self):
        if _sliby:
            _sliby.SlibyPolicyDestroy(self._c_struct)

    def get_variable_value(self, bundle, var_name, namespace="default"):
        value = _sliby.SlibyPolicyGetVariableValue(self._c_struct, namespace.encode(), bundle.encode(), var_name.encode())
        if value:
            return value.decode()
        else:
            return None

    def get_control_body_attribute_value(self, body_type, attribute_name):
        value = _sliby.SlibyPolicyGetControlBodyAttribute(self._c_struct, body_type.encode(), attribute_name.encode())
        if value:
            return value.decode()
        else:
            return None
