#!/usr/bin/env python

import os
import sys
sys.path.append("/Users/yangye/Documents/Workspace/magrad/script")
import Phits2dProcess as p2d

if __name__=="__main__":
    filename = sys.argv[1][0:-4]
    datfile = filename + ".dat"
    errfile = filename + "_err.dat"
    print "load file:", datfile, errfile
    dist = p2d.Phits2dFiles(datfile, errfile)
    dist.SetId(2)
    rootfile = filename + ".root"
    dist.Write(rootfile)
