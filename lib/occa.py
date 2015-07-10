import _C_occa
import numpy as np
import sys

#---[ Setup ]---------------------------
def isA(self, class_):
    return self.__class__ is class_

def isNotA(self, class_):
    return not(self.__class__ is class_)

def isAString(self):
    return isinstance(self, basestring)

def isNotAString(self):
    return (not isinstance(self, basestring))

def sizeof(npType):
    return np.dtype(npType).itemsize

def typeof(npType):
    return np.dtype(npType).num

def nameof(npType):
    return np.dtype(npType).name
#=======================================

#---[ Globals & Flags ]-----------------
def setVerboseCompilation(value):
    _C_occa.setVerboseCompilation(value)
#=======================================

#----[ Background Device ]--------------
#  |---[ Device ]-----------------------
def setDevice(arg):
    #---[ Arg Testing ]-------
    try:
        if arg.isNotAString() and \
           arg.isNotA(device):

            raise ValueError('Argument to [occa.setDevice] must be a occa.device or string')

    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    if arg.isAString():
        _C_occa.setDeviceFromInfo(arg)
    elif arg.isA(device):
        _C_occa.setDevice(arg.handle)

def getCurrentDevice():
    return device(_C_occa.getCurrentDevice())

def setCompiler(compiler):
    #---[ Arg Testing ]-------
    try:
        if arg.isNotAString():
            raise ValueError('Argument to [occa.setCompiler] must be a string')

    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    _C_occa.setCompiler(compiler)

def setCompilerEnvScript(compilerEnvScript):
    #---[ Arg Testing ]-------
    try:
        if arg.isNotAString():
            raise ValueError('Argument to [occa.setCompilerEnvScript] must be a string')

    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    _C_occa.setCompilerEnvScript(compilerEnvScript)

def setCompilerFlags(compilerFlags):
    #---[ Arg Testing ]-------
    try:
        if arg.isNotAString():
            raise ValueError('Argument to [occa.setCompilerFlags] must be a string')

    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

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
    #---[ Arg Testing ]-------
    try:
        if stream.isNotA(stream):
            raise ValueError('Argument to [occa.setStream] must be a occa.stream')

    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    _C_occa.setStream(stream)

def wrapStream(handle):
    return stream(_C_occa.wrapStream(handle))
#  |====================================

#  |---[ Kernel ]-----------------------
def buildKernel(str_, functionName, kInfo = 0):
    #---[ Arg Testing ]-------
    try:
        pass
        # if str_.isNotAString()         or \
        #    functionName.isNotAString() or \
        #    (kInfo != 0)

    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
    return kernel(_C_occa.buildKernel(str_, functionName, kInfo_))

def buildKernelFromSource(filename, functionName, kInfo = 0):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
    return kernel(_C_occa.buildKernelFromSource(filename, functionName, kInfo_))

def buildKernelFromString(source, functionName, kInfo = 0, language = "OKL"):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
    return kernel(_C_occa.buildKernelFromString(source, functionName, kInfo, language))

def buildKernelFromBinary(binary, functionName):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    return kernel(_C_occa.buildKernelFromBinary(filename, functionName))

def buildKernelFromLoopy(filename, functionName, kInfo = 0):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
    return kernel(_C_occa.buildKernelFromLoopy(filename, functionName, kInfo_))

def buildKernelFromFloopy(filename, functionName, kInfo = 0):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
    return kernel(_C_occa.buildKernelFromFloopy(filename, functionName, kInfo_))
#  |====================================

#  |---[ Memory ]-----------------------
def memcpy(dest, src, bytes_, offset1 = 0, offset2 = 0):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    if dest.__class__ is memory:
        if src.__class__ is memory:
            _C_occa.copyMemToMem(dest.handle, src.handle, bytes_, offset1, offset2)
        else:
            _C_occa.copyPtrToMem(dest.handle, src, bytes_, offset1)
    else:
        if src.__class__ is memory:
            _C_occa.copyMemToPtr(dest, src.handle, bytes_, offset1)
        else:
            _C_occa.memcpy(dest, src, bytes_)

def asyncMemcpy(dest, src, bytes_, offset1 = 0, offset2 = 0):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    if dest.__class__ is memory:
        if src.__class__ is memory:
            _C_occa.asyncCopyMemToMem(dest.handle, src.handle, bytes_, offset1, offset2)
        else:
            _C_occa.asyncCopyPtrToMem(dest.handle, src, bytes_, offset1)
    else:
        if src.__class__ is memory:
            _C_occa.asyncCopyMemToPtr(dest, src.handle, bytes_, offset1)
        else:
            _C_occa.asyncMemcpy(dest, src, bytes_)

def wrapMemory(handle, entries, type_):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    return memory(_C_occa.wrapMemory(handle, entries, sizeof(type_)))

def wrapManagedMemory(handle, entries, type_):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    return _C_occa.wrapManagedMemory(handle, entries, sizeof(type_), typeof(type_))

def malloc(entries, type_):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    return memory(_C_occa.malloc(entries, sizeof(type_)))

def managedAlloc(entries, type_):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    return _C_occa.managedAlloc(entries, sizeof(type_), typeof(type_))

def malloc(entries, type_):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    return memory(_C_occa.mappedAlloc(entries, sizeof(type_)))

def managedMappedAlloc(entries, type_):
    #---[ Arg Testing ]-------
    try:
        pass
    except ValueError as e:
        print(e)
        sys.exit()
    #=========================

    return _C_occa.managedMappedAlloc(entries, sizeof(type_), typeof(type_))
#  |====================================
#=======================================

#---[ Device ]--------------------------
def printAvailableDevices():
    _C_occa.printAvailableDevices()

class device:
    def __init__(self, arg = 0):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        if arg.isAString():
            self.handle = _C_occa.createDevice(arg)
        else:
            self.handle = arg

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
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        _C_occa.deviceSetCompiler(self.handle, compiler)

    def setCompilerEnvScript(self, compilerEnvScript):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        _C_occa.deviceSetCompiler(self.handle, compilerEnvScript)

    def setCompilerFlags(self, compilerFlags):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

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
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
        return kernel(_C_occa.deviceBuildKernel(self.handle, str_, functionName, kInfo_))

    def buildKernelFromSource(self, filename, functionName, kInfo = 0):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
        return kernel(_C_occa.deviceBuildKernelFromSource(self.handle, filename, functionName, kInfo_))

    def buildKernelFromString(self, source, functionName, kInfo = 0):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
        return kernel(_C_occa.deviceBuildKernelFromString(self.handle, source, functionName, kInfo_))

    def buildKernelFromBinary(self, binary, functionName):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        return kernel(_C_occa.deviceBuildKernelFromBinary(self.handle, binary, functionName))

    def buildKernelFromLoopy(self, filename, functionName, kInfo = 0):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
        return kernel(_C_occa.deviceBuildKernelFromLoopy(self.handle, filename, functionName, kInfo_))

    def buildKernelFromFloopy(self, filename, functionName, kInfo = 0):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        kInfo_ = (0 if (kInfo == 0) else kInfo.handle)
        return kernel(_C_occa.deviceBuildKernelFromFloopy(self.handle, filename, functionName, kInfo_))

    def malloc(self, entries, type_):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        return memory(_C_occa.deviceMalloc(self.handle, entries, sizeof(type_)))

    def managedAlloc(self, entries, type_):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        return _C_occa.deviceManagedAlloc(self.handle, entries, sizeof(type_), typeof(type_))

    def mappedAlloc(self, entries, type_):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        return memory(_C_occa.deviceMappedAlloc(self.handle, entries, sizeof(type_)))

    def managedMappedAlloc(self, entries, type_):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

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
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        return stream(_C_occa.deviceSetStream(self.handle, stream))

    def wrapStream(self, handle):
        return stream(_C_occa.deviceWrapStream(self.handle, handle))

class stream:
    def __init__(self, handle_ = None):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        if handle_ is not None:
            self.handle      = handle_
            self.isAllocated = True
        else:
            self.handle      = 0
            self.isAllocated = False

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
    def __init__(self, handle_ = None):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        if handle_ is not None:
            self.handle      = handle_
            self.isAllocated = True
        else:
            self.handle      = 0
            self.isAllocated = False

    def free(self):
        import _C_occa

        if self.isAllocated:
            _C_occa.kernelFree(self.handle)
            self.isAllocated = False

    def __del__(self):
        self.free()

    def __call__(self, *args):
        argList = _C_occa.createArgumentList()

        for i in xrange(len(args)):
            arg = args[i]

            if arg.__class__ is np.ndarray:
                argType = _C_occa.ptr(arg.ctypes.data)
                _C_occa.argumentListAddArg(argList, i, argType)
            elif arg.__class__ is memory:
                _C_occa.argumentListAddArg(argList, i, arg.handle)
            else:
                argType = getattr(_C_occa, nameof(arg))(arg)
                _C_occa.argumentListAddArg(argList, i, argType)

        _C_occa.kernelRun(self.handle, argList)

        _C_occa.argumentListFree(argList)

    def mode(self):
        return _C_occa.kernelMode(self.handle)

    def name(self):
        return _C_occa.kernelName(self.handle)

    def getDevice(self):
        return device(_C_occa.kernelGetDevice(self.handle))

class kernelInfo:
    def __init__(self, handle_ = None):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        if handle_ is not None:
            self.handle      = handle_
        else:
            self.handle      = _C_occa.createKernelInfo()

        self.isAllocated = True

    def free(self):
        import _C_occa

        if self.isAllocated:
            _C_occa.kernelInfoFree(self.handle)
            self.isAllocated = False

    def __del__(self):
        self.free()

    def addDefine(self, macro, value):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        _C_occa.kernelInfoAddDefine(self.handle, macro, value.__str__())

    def addInclude(self, filename):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        _C_occa.kernelInfoAddInclude(self.handle, filename)
#=======================================

#---[ Memory ]--------------------------
class memory:
    def __init__(self, handle_ = None):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        if handle_ is not None:
            self.handle      = handle_
            self.isAllocated = True
        else:
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

    def copyFrom(self, src, bytes_ = 0, offset1 = 0, offset2 = 0):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        memcpy(self, src, bytes_, offset1, offset2)

    def copyTo(self, dest, bytes_ = 0, offset1 = 0, offset2 = 0):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        memcpy(dest, self, bytes_, offset1, offset2)

    def asyncCopyFrom(self, dest, bytes_ = 0, offset1 = 0, offset2 = 0):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        asyncMemcpy(self, src, bytes_, offset1, offset2)

    def asyncCopyTo(self, dest, bytes_ = 0, offset1 = 0, offset2 = 0):
        #---[ Arg Testing ]-------
        try:
            pass
        except ValueError as e:
            print(e)
            sys.exit()
        #=========================

        asyncMemcpy(dest, self, bytes_, offset1, offset2)
#=======================================
