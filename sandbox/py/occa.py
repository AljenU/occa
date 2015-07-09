import _C_occa
import numpy as np

#---[ Setup ]---------------------------
def sizeof(npType):
    return np.dtype(npType).itemsize

def typeof(npType):
    return np.dtype(npType).num
#=======================================

#---[ Globals & Flags ]-----------------
def setVerboseCompilation(value):
    _C_occa.setVerboseCompilation(value)
#=======================================

#----[ Background Device ]--------------
#  |---[ Device ]-----------------------
def setDevice(device):
    _C_occa.setDevice(device.handle)

def setDeviceFromInfo(infos):
    _C_occa.setDeviceFromInfo(infos)

def getCurrentDevice():
    return device(_C_occa.getCurrentDevice())

def setCompiler(compiler):
    _C_occa.setCompiler(compiler)

def setCompilerEnvScript(compilerEnvScript):
    _C_occa.setCompilerEnvScript(compilerEnvScript)

def setCompilerFlags(compilerFlags):
    _C_occa.setCompilerFlags(compilerFlags)

def getCompiler():
    return _C_occa.getCompiler()

def getCompilerEnvScript():
    return _C_occa.getCompilerEnvScript()

def getCompilerFlags():
    return _C_occa.getCompilerFlags()

def flush():
    _C_occa.flush()

def finish():
    _C_occa.finish()

def createStream():
    return stream(_C_occa.createStream())

def getStream():
    return stream(_C_occa.getStream())

def setStream(stream):
    _C_occa.setStream(stream)

def wrapStream(handle):
    return stream(_C_occa.wrapStream(handle))
#  |====================================

#  |---[ Kernel ]-----------------------
def buildKernel(str_, functionName, kInfo = 0):
    return kernel(_C_occa.buildKernel(filename, functionName, info))

def buildKernelFromSource(filename, functionName, kInfo = 0):
    return kernel(_C_occa.buildKernelFromSource(filename, functionName, info))

def buildKernelFromString(source, functionName, kInfo = 0, language = "OKL"):
    return kernel(_C_occa.buildKernelFromString(source, functionName, kInfo, language))

def buildKernelFromBinary(binary, functionName):
    return kernel(_C_occa.buildKernelFromBinary(filename, functionName))

def buildKernelFromLoopy(filename, functionName, kInfo = 0):
    return kernel(_C_occa.buildKernelFromLoopy(filename, functionName, info))

def buildKernelFromFloopy():
    return kernel(_C_occa.buildKernelFromFloopy(filename, functionName, info))
#  |====================================

#  |---[ Memory ]-----------------------
def memcpy(dest, src, bytes_, offset1 = 0, offset2 = 0):
    if type(dest) is memory:
        if type(src) is memory:
            _C_occa.copyMemToMem(dest.handle, src.handle, bytes_, offset1, offset2)
        else:
            _C_occa.copyPtrToMem(dest.handle, src, bytes_, offset1)
    else:
        if type(src) is memory:
            _C_occa.copyMemToPtr(dest, src.handle, bytes_, offset1)
        else:
            _C_occa.memcpy(dest, src, bytes_)

def wrapMemory(handle, entries, type_):
    return memory(_C_occa.wrapMemory(handle, entries, sizeof(type_)))

def wrapManagedMemory(handle, entries, type_):
    return _C_occa.wrapManagedMemory(handle, entries, sizeof(type_), typeof(type_))

def malloc(entries, type_):
    return memory(_C_occa.malloc(entries, sizeof(type_)))

def managedAlloc():
    return _C_occa.managedAlloc(entries, sizeof(type_), typeof(type_))

def malloc(entries, type_):
    return memory(_C_occa.mappedAlloc(entries, sizeof(type_)))

def managedAlloc():
    return _C_occa.managedMappedAlloc(entries, sizeof(type_), typeof(type_))
#  |====================================
#=======================================

#---[ Device ]--------------------------
def printAvailableDevices():
    _C_occa.printAvailableDevices()

class device:
    def __init__(self):
        self.handle      = 0
        self.isAllocated = False

    def __init__(self, handle_):
        self.handle      = handle_
        self.isAllocated = True

    def free(self):
        import _C_occa

        if self.isAllocated:
            _C_occa.deviceFree(self.handle)
            self.isAllocated = False

    def __del__(self):
        self.free()

    def mode(self):
        return _C_occa.deviceMode(self.handle)

    def setCompiler(self, compiler):
        _C_occa.deviceSetCompiler(self.handle, compiler)

    def setCompilerEnvScript(self, compilerEnvScript):
        _C_occa.deviceSetCompiler(self.handle, compilerEnvScript)

    def setCompilerFlags(self, compilerFlags):
        _C_occa.deviceSetCompilerFlags(self.handle, compilerFlags)

    def getCompiler(self):
        return _C_occa.deviceGetCompiler(self.handle)

    def getCompilerEnvScript(self):
        return _C_occa.deviceGetCompiler(self.handle)

    def getCompilerFlags(self):
        return _C_occa.deviceGetCompilerFlags(self.handle)

    def bytesAllocated(self):
        return _C_occa.bytesAllocated(self.handle)

    def buildKernel(self, str_, functionName, kInfo = 0):
        return kernel(_C_occa.deviceBuildKernel(self.handle, str_, functionName, kInfo))

    def buildKernelFromSource(self, filename, functionName, kInfo = 0):
        return kernel(_C_occa.deviceBuildKernelFromSource(self.handle, filename, functionName, kInfo))

    def buildKernelFromString(self, source, functionName, kInfo = 0):
        return kernel(_C_occa.deviceBuildKernelFromString(self.handle, source, functionName, kInfo))

    def buildKernelFromBinary(self, binary, functionName):
        return kernel(_C_occa.deviceBuildKernelFromBinary(self.handle, binary, functionName))

    def buildKernelFromLoopy(self, filename, functionName, kInfo = 0):
        return kernel(_C_occa.deviceBuildKernelFromLoopy(self.handle, filename, functionName, kInfo))

    def buildKernelFromFloopy(self, filename, functionName, kInfo = 0):
        return kernel(_C_occa.deviceBuildKernelFromFloopy(self.handle, filename, functionName, kInfo))

    def malloc(self, entries, type_):
        return memory(_C_occa.deviceMalloc(self.handle, entries, sizeof(type_)))

    def managedAlloc(self, entries, type_):
        return _C_occa.deviceManagedAlloc(self.handle, entries, sizeof(type_), typeof(type_))

    def mappedAlloc(self, entries, type_):
        return memory(_C_occa.deviceMappedAlloc(self.handle, entries, sizeof(type_)))

    def managedMappedAlloc(self, entries, type_):
        return _C_occa.deviceManagedMappedAlloc(self.handle, entries, sizeof(type_), typeof(type_))

    def flush(self):
        return _C_occa.deviceFlush(self.handle)

    def finish(self):
        return _C_occa.deviceFinish(self.handle)

    def createStream(self):
        return stream(_C_occa.deviceCreateStream(self.handle))

    def getStream(self):
        return stream(_C_occa.deviceGetStream(self.handle))

    def setStream(self, stream):
        return stream(_C_occa.deviceSetStream(self.handle, stream))

    def wrapStream(self, handle):
        return stream(_C_occa.deviceWrapStream(self.handle, handle))

class stream:
    def __init__(self):
        self.handle      = 0
        self.isAllocated = False

    def __init__(self, handle_):
        self.handle      = handle_
        self.isAllocated = True

    def free(self):
        import _C_occa

        if self.isAllocated:
            _C_occa.streamFree(self.handle)
            self.isAllocated = False

    def __del__(self):
        self.free()
#=======================================

#---[ Kernel ]--------------------------
class kernel:
    def __init__(self):
        self.handle      = 0
        self.isAllocated = False

    def __init__(self, handle_):
        self.handle      = handle_
        self.isAllocated = True

    def free(self):
        import _C_occa

        if self.isAllocated:
            _C_occa.kernelFree(self.handle)
            self.isAllocated = False

    def __del__(self):
        self.free()

    def __call__(self, args):
        argList = _C_occa.createArgumentList()

        for i in xrange(len(args)):
            _C_occa.argumentListAddArg(argList, i, 0) # <>

        _C_occa.kernelRun(self.handle, argList)

        _C_occa.argumenetListFree(argList)

    def mode(self):
        return _C_occa.kernelMode(self.handle)

    def name(self):
        return _C_occa.kernelName(self.handle)

    def getDevice(self):
        return device(_C_occa.kernelGetDevice(self.handle))

class kernelInfo:
    def __init__(self):
        self.handle = _C_occa.createKernelInfo()

    def __init__(self, handle_):
        self.handle = handle_

    def free(self):
        import _C_occa

        if self.isAllocated:
            _C_occa.kernelInfoFree(self.handle)

    def __del__(self):
        self.free()

    def addDefine(self, macro, value):
        _C_occa.kernelInfoAddDefine(self.handle, macro, value.__str__())

    def addInclude(self, filename):
        _C_occa.kernelInfoAddInclude(self.handle, filename)
#=======================================

#---[ Memory ]--------------------------
class memory:
    def __init__(self):
        self.handle      = 0
        self.isAllocated = False

    def free(self):
        import _C_occa

        if self.isAllocated:
            _C_occa.memoryFree(self.handle)
            self.isAllocated = False

    def __del__(self):
        self.free()

    def mode(self):
        return _C_occa.memoryMode(self.handle)

    def getMemoryHandle(self):
        return _C_occa.memoryGetMemoryHandle(self.handle)

    def getMappedPointer(self):
        return _C_occa.memoryGetMappedPointer(self.handle)

    def getTextureHandle(self):
        return _C_occa.memoryGetTextureHandle(self.handle)
#=======================================