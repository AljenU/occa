import sys, traceback, gc
from ctypes import *

libocca = CDLL('libocca.so')

"""
---[ C types ]----------------
    c_bool
    c_char
    c_int
    c_long
    c_float
    c_double
//============================
"""

class device:
    # Ok
    def mode(self):
        cMode = self.lib.occaDeviceMode(self.cDevice)
        return c_char_p(cMode).value

    # Ok
    def __init__(self, mode, platformID, deviceID):
        self.lib = libocca

        self.isAllocated = True
        self.cDevice = self.lib.occaGetDevice(mode, platformID, deviceID)

    # Ok
    def setup(self, mode, platformID, deviceID):
        self.isAllocated = True
        self.cDevice = self.lib.occaGetDevice(mode, platformID, deviceID)

    # Ok
    def setCompiler(self, compiler):
        self.lib.occaDeviceSetCompiler(self.cDevice, compiler)

    # Ok
    def setCompilerFlags(self, compilerFlags):
        self.lib.occaDeviceSetCompilerFlags(self.cDevice, compilerFlags)

    # Ok
    def malloc(self, entryType, entries):
        if type(entries) is list:
            cByteCount = sizeof(entryType)*len(entries)
            cSource    = (entryType * len(entries))(*entries)
        elif isinstance(entries, (int,long)):
            cByteCount = sizeof(entryType)*entries
            cSource    = None
        else:
            print "Entries should be a list"
            traceback.print_exc(file=sys.stdout)
            sys.exit()

        return memory(self.lib.occaDeviceMalloc(self.cDevice,
                                               cByteCount,
                                               cSource))

    def genStream(self):
        return self.lib.occaGenStream(self.cDevice)

    def getStream(self):
        return self.lib.occaGetStream(self.cDevice)

    def setStream(self):
        return self.lib.occaSetStream(self.cDevice)

    # Ok
    def free(self):
        if self.isAllocated:
            self.lib.occaDeviceFree(self.cDevice)
            self.isAllocated = False

    def __del__(self):
        self.free()

class kernelInfo:
    def addDefine(self):
        pass

class kernel:
    def mode(self):
        cMode = self.lib.occaKernelMode(self.cKernel)
        return c_char_p(cMode).value

    def __init__(self, cKernel):
        self.lib = libocca

        self.cKernel = cKernel

    def preferredDimSize(self):
        return self.lib.occaKernelPreferredDimSize(self.cKernel)

    def setWorkingDims(self, dims, itemsPerGroup, groups):


        # occaDims = ?
        return self.lib.occaKernelSetWorkingDims(self.cKernel)

    def __call__(self, args):
        argList = self.lib.occaGenArgumentList()

        for arg in args:
            if arg.__class__ is memory:
                self.lib.occaArgumentlistAddArg(argList, arg)
            else:
                print "Not implemented yet"

        self.lib.occaKernelRun_(self.cKernel, argList)

    def timeTaken(self):
        return self.lib.occaKernelTimeTaken(self.cKernel)

    def free(self):
        self.lib.occaKernelFree(self.cKernel)

class memory:
    # Ok
    def mode(self):
        cMode = self.lib.occaMemoryMode(self.cMemory)
        return c_char_p(cMode).value

    # Ok
    def __init__(self, cMemory):
        self.lib = libocca

        self.isAllocated = True
        self.cMemory = cMemory

    # Ok
    def copyTo(self, entryType, dest, entries = 0, offset = 0):
        copyingToMem = (dest.__class__ is memory)

        if (entries == 0) and not copyingToMem:
            cEntries = len(dest)
        else:
            cEntries = entries

        cByteCount = sizeof(entryType) * cEntries

        if type(dest) is list:
            cDest = (entryType * cEntries)()

            self.lib.occaCopyMemToPtr(cDest,
                                     self.cMemory,
                                     cByteCount,
                                     offset)

            for e in xrange(cEntries):
                dest[e + offset] = cDest[e]
        elif copyingToMem:
            self.lib.occaCopyMemToMem(dest.cMemory,
                                     self.cMemory,
                                     cByteCount,
                                     offset)
        else:
            print "Wrong arguments"
            traceback.print_exc(file=sys.stdout)
            sys.exit()

    # Ok
    def copyFrom(self, entryType, src, entries = 0, offset = 0):
        copyingFromMem = (src.__class__ is memory)

        if (entries == 0) and not copyingFromMem:
            cEntries = len(src)
        else:
            cEntries = entries

        cByteCount = sizeof(entryType) * cEntries

        if type(src) is list:
            cSrc = (entryType * cEntries)(*src)

            self.lib.occaCopyPtrToMem(self.cMemory,
                                     cSrc,
                                     cByteCount,
                                     offset)
        elif copyingFromMem:
            self.lib.occaCopyMemToMem(self.cMemory,
                                     src.cMemory,
                                     cByteCount,
                                     offset)
        else:
            print "Wrong arguments"
            traceback.print_exc(file=sys.stdout)
            sys.exit()

    # [-] Add async later
    def asyncCopyTo(self, entryType, dest, byteCount = 0, offset = 0):
        self.copyTo(entryType, dest, byteCount, offset)

    # [-] Add async later
    def asyncCopyFrom(self, entryType, src, byteCount = 0, offset = 0):
        self.copyFrom(entryType, src, byteCount, offset)

    # Ok
    def swap(self, m):
        self.cMemory, m.cMemory = m.cMemory, self.cMemory

    # Ok
    def free(self):
        if self.isAllocated:
            self.lib.occaMemoryFree(self.cMemory)
            self.isAllocated = False

    def __del__(self):
        self.free()

d = device("OpenCL", 0, 0)

print d.mode()

d.setCompiler("clang++")
d.setCompilerFlags("blah blah")

m1 = d.malloc(c_float, [1,1,1])
m2 = d.malloc(c_float, [3,3,3])

m1.copyFrom(c_float, [2,5,7])
m1.swap(m2)

a = [3,2,1]

m2.copyTo(c_float, a)

print a
