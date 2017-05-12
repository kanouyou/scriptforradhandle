#!/usr/bin/env python

import sys
import ROOT
import numpy as np

def load(tfile, cnt):
    #tfile = ROOT.TFile.Open( filename)
    #nkey  = tfile.GetNkeys()
    tree  = tfile.Get("tree%i" %cnt)
    data  = {"idx":np.array([]), "idy":np.array([]), "x":np.array([]), "y":np.array([]), \
            "z":0., "inf":np.array([]), "err":np.array([])}

    data["z"] = float(tree.GetTitle()[2:])
    print " -- loading file: %s , z = %.2f cm" %(tfile.GetName(), data["z"])

    for i in range(tree.GetEntries()):
        tree.GetEntry(i)
        data["idx"] = np.append( data["idx"], int(tree.idx) )
        data["idy"] = np.append( data["idy"], int(tree.idy) )
        data[  "x"] = np.append( data[  "x"], float(tree.x) )
        data[  "y"] = np.append( data[  "y"], float(tree.y) )
        data["inf"] = np.append( data["inf"], float(tree.e) )
        data["err"] = np.append( data["err"], float(tree.err) )

    return data


def writetofile(neufile, dosefile, dpafile):
    neuNkey  = neufile.GetNkeys()
    doseNkey = dosefile.GetNkeys()
    dpaNkey  = dpafile.GetNkeys()

    if neuNkey!=doseNkey or doseNkey!=dpaNkey or dpaNkey!=neuNkey:
        print "Error: file's number of key is different."

    ROOT.gROOT.ProcessLine("struct Container { \
                                Int_t     idx; \
                                Int_t     idy; \
                                Double_t    x; \
                                Double_t    y; \
                                Double_t  flx; \
                                Double_t flx_e; \
                                Double_t dose; \
                                Double_t dose_e; \
                                Double_t  dpa; \
                                Double_t dpa_e; \
                            };")

    struct = ROOT.Container()

    for i in range(neuNkey):
        neu  = load( neufile, i+1 )
        dose = load( dosefile, i+1 )
        dpa  = load( dpafile, i+1 )
        tree = ROOT.TTree( "tree%i" %(i+1), "z=%.2f" %neu["z"])
        tree.Branch("idx", ROOT.AddressOf(struct, "idx"), "idx/I")
        tree.Branch("idy", ROOT.AddressOf(struct, "idy"), "idy/I")
        tree.Branch(  "x", ROOT.AddressOf(struct,   "x"),   "x/D")
        tree.Branch(  "y", ROOT.AddressOf(struct,   "y"),   "y/D")
        tree.Branch("flx", ROOT.AddressOf(struct, "flx"), "flx/D")
        tree.Branch("flx_e", ROOT.AddressOf(struct, "flx_e"), "flx_e/D")
        tree.Branch("dose", ROOT.AddressOf(struct, "dose"), "dose/D")
        tree.Branch("dose_e", ROOT.AddressOf(struct, "dose_e"), "dose_e/D")
        tree.Branch("dpa", ROOT.AddressOf(struct, "dpa"), "dpa/D")
        tree.Branch("dpa_e", ROOT.AddressOf(struct, "dpa_e"), "dpa_e/D")
        if len(neu["idx"])!=len(dose["idx"]) or len(neu["idx"])!=len(dpa["idx"]):
            print "Error: data size is different."
            break
        for j in range(len(neu["idx"])):
            struct.idx = int(neu["idx"][j])
            struct.idy = int(neu["idy"][j])
            struct.x   = neu[  "x"][j]
            struct.y   = neu[  "y"][j]
            struct.flx = neu["inf"][j]
            struct.flx_e = neu["err"][j]
            struct.dose = dose["inf"][j]
            struct.dose_e = dose["err"][j]
            struct.dpa = dpa["inf"][j]
            struct.dpa_e = dpa["err"][j]
            tree.Fill()
        tree.Write()


def combinefiles(neufilename, dosefilename, dpafilename, output="output.root"):
    # neutron file
    neufile = ROOT.TFile( neufilename )
    #neuNkey = neufile.GetNkeys()
    # dose file
    dosefile = ROOT.TFile( dosefilename )
    #doseNkey = dosefile.GetNkeys()
    # dpa file
    dpafile  = ROOT.TFile( dpafilename )
    #dpaNkey  = dpafile.GetNkeys()
    rootfile = ROOT.TFile( output, "recreate" )
    writetofile( neufile, dosefile, dpafile )
    rootfile.Write()
    rootfile.Close()


if __name__=="__main__":
    combinefiles( sys.argv[1], sys.argv[2], sys.argv[3] )

