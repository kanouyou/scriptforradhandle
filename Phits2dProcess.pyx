## class to extract the information from the Phtis output file

#import matplotlib.pyplot as plt
import array
import numpy as np
import math
import re
import ROOT

## class to read output file and extract the information
class Phits2dDistribution:

    ## constructor
    def __init__(self, char* filename):
        self._file = filename
        self.Load(filename)
        self._data = {"idx":np.array([]), "idy":np.array([]), \
                      "x":np.array([]), "y":np.array([]), "z":0., "e":np.array([])}
        self._mesh = {"xmin":0., "xmax":0., "nx":0, "ymin":0., "ymax":0., "ny":0, "zmin":0., "zmax":0., "nz":0}
        self._edep = np.array([])
        self.Extract(self._dataline)

    ## load file
    def Load(self, char* filename):
        tfile = open(filename, "r")
        self._dataline = tfile.readlines()
        tfile.close()

    ## set factor on energy deposit
    def SetFactor(self, factor):
        self._data["e"] *= factor

    ## extract the information from file
    def Extract(self, dataline):
        for eachline in dataline:
            eachline.strip()
            item = eachline.split()
            if len(item)>0 and item[0]=="xmin":
                self._mesh["xmin"] = float(item[2])
            if len(item)>0 and item[0]=="xmax":
                self._mesh["xmax"] = float(item[2])
            if len(item)>0 and item[0]=="nx":
                self._mesh[  "nx"] = int(item[2])
            if len(item)>0 and item[0]=="ymin":
                self._mesh["ymin"] = float(item[2])
            if len(item)>0 and item[0]=="ymax":
                self._mesh["ymax"] = float(item[2])
            if len(item)>0 and item[0]=="ny":
                self._mesh[  "ny"] = int(item[2])
            if len(item)>0 and item[0]=="zmin":
                self._mesh["zmin"] = float(item[2])
            if len(item)>0 and item[0]=="zmax":
                self._mesh["zmax"] = float(item[2])
            if len(item)>0 and item[0]=="nz":
                self._mesh[  "nz"] = int(item[2])
                break

    ## extract data
    def ExtractData(self, dataline, znum):
        cdef int cnt = 0
        cdef int row = 0
        cdef int start = self._mesh["nx"] * self._mesh["ny"] / 10 * (znum-1)
        cdef int final = self._mesh["nx"] * self._mesh["ny"] / 10 * znum
        cdef double dz = (self._mesh["zmax"] - self._mesh["zmin"]) / self._mesh["nz"]
        self._data["z"] = self._mesh["zmin"] + dz * znum + dz/2.
        self._edep = np.array([])
        for eachline in dataline:
            eachline.strip()
            item = eachline.split()
            if len(item)==10 and re.match("\d.\d", item[0]):
                if cnt>=start and cnt<final:
                    for i in range(10):
                        self._edep = np.append(self._edep, float(item[i]))
                    row += 1
                if cnt >=final:
                    break
                cnt += 1
        #print cnt, start, final, row
        #print "read group %i, corresponding to the row from %i to %i" %(znum, start, final)

    ## align the data into 3d array
    def AlignData(self, edep):
        cdef double dx = (self._mesh["xmax"] - self._mesh["xmin"]) / self._mesh["nx"]
        cdef double dy = (self._mesh["ymax"] - self._mesh["ymin"]) / self._mesh["ny"]
        cdef int cnt = 0
        self._data = {"idx":np.array([]), "idy":np.array([]), \
                      "x":np.array([]), "y":np.array([]), "z":self._data["z"], "e":np.array([])}
        for j in range(self._mesh["ny"]):
            y = self._mesh["ymin"] + j*dy + dy/2.
            for i in range(self._mesh["nx"]):
                x = self._mesh["xmin"] + i*dx + dx/2.
                self._data["idx"] = np.append(self._data["idx"], i)
                self._data["idy"] = np.append(self._data["idy"], j)
                self._data[ "x"] = np.append(self._data[ "x"], x)
                self._data[ "y"] = np.append(self._data[ "y"], y)
                self._data[ "e"] = np.append(self._data[ "e"], edep[cnt])
                cnt += 1
        #print self._mesh["ny"], self._mesh["nx"], cnt

    ## return the 3d data
    def GetData(self, zid):
        self.ExtractData(self._dataline, zid)
        self.AlignData(self._edep)
        return self._data

    ## return the mesh
    def GetMesh(self):
        return self._mesh

    ## print information
    def Print(self):
        self.PrintMesh()
        self.PrintData()

    ## print mesh information
    def PrintMesh(self):
        print "xmin: %f[cm], xmax: %f[cm], nx: %i" %(self._mesh["xmin"], self._mesh["xmax"], self._mesh["nx"])
        print "ymin: %f[cm], ymax: %f[cm], ny: %i" %(self._mesh["ymin"], self._mesh["ymax"], self._mesh["ny"])
        print "zmin: %f[cm], zmax: %f[cm], nz: %i" %(self._mesh["zmin"], self._mesh["zmax"], self._mesh["nz"])

    ## print 3d energy deposit data
    def PrintData(self):
        for i in range(len(self._data["x"])):
            print i,self._data["x"][i], self._data["y"][i], self._data["e"][i]

    ## return the filename
    def GetFilename(self):
        return self._file[:-4]


### class to extract the both data and error
class Phits2dFiles:

    ## constructor
    def __init__(self, datfile, errfile):
        self._datfile = Phits2dDistribution(datfile)
        self._errfile = Phits2dDistribution(errfile)
        self._id = 1
        self._msh = self._datfile.GetMesh()

    ## set z id
    def SetId(self, num):
        self._id = num

    ## return the data
    def GetInfo(self, num):
        dat = self._datfile.GetData(num)
        err = self._errfile.GetData(num)
        dat["err"] = np.array([])
        for i in range(len(err["e"])):
            dat["err"] = np.append(dat["err"], err["e"][i])
        return dat

    ## realign data
    def GetData(self):
        dat = self.GetInfo(self._id)
        return dat

    ## write data to root file
    def Write(self, filename):
        tfile = ROOT.TFile(filename, "recreate")
        for i in range(self._msh["nz"]):
            data = self.GetInfo(i+1)
            self.WriteToRootfile(filename, data, i+1)
            del data
        tfile.Write()
        tfile.Close()

    ## write to root file
    def WriteToRootfile(self, filename, data, cnt):
        tree = ROOT.TTree("tree%i" %cnt, "z=%.2f" %data["z"])

        print "extracting data: z=%.2f cm, data size: %i" %(data["z"], len(data["idx"]))

        _idx = np.zeros(1, dtype=int)
        _idy = np.zeros(1, dtype=int)
        _x   = np.zeros(1, dtype=float)
        _y   = np.zeros(1, dtype=float)
        _e   = np.zeros(1, dtype=float)
        _err = np.zeros(1, dtype=float)

        tree.Branch("idx", _idx, "idx/I")
        tree.Branch("idy", _idy, "idy/I")
        tree.Branch("x", _x, "x/D")
        tree.Branch("y", _y, "y/D")
        tree.Branch("e", _e, "e/D")
        tree.Branch("err", _err, "err/D")

        for i in range(len(data["idx"])):
            _idx[0] = int(data["idx"][i])
            _idy[0] = int(data["idy"][i])
            _x  [0] = data[ "x"][i]
            _y  [0] = data[ "y"][i]
            _e  [0] = data[ "e"][i]
            _err[0] = data["err"][i]
            tree.Fill()

        tree.Write()


