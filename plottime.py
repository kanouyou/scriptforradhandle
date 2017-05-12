#!/usr/bin/env python2.7

import matplotlib.pyplot as plt
import sys
import math
import re
import numpy as np

nt = 0
it = 0
marker = ["o", "s", "v", "^", "<", ">"]

beam = 4.4e+13
rho  = 3.8

def Filter(num, dataline):
    data = {"V":np.array([]), "t":np.array([]), "dt":np.array([]), "e":np.array([]), \
            "err":np.array([])}
    cnt = 0
    for eachline in dataline:
        eachline.strip()
        item = eachline.split()
        if len(item)==5 and item[-2]=="#":
            data["V"] = np.append( data["V"], float(item[2]) )
        if len(item)==4 and re.match("^\d.", item[0]):
            if cnt/nt==num:
                data[ "t"] = np.append( data[ "t"], (float(item[0])+float(item[1]))/2. )
                data["dt"] = np.append( data["dt"], float(item[1])-float(item[0]) )
                data[ "e"] = np.append( data[ "e"], float(item[2])/beam*rho*6.242e+12*1e-3 )
                data["err"] = np.append( data["err"], float(item[2])*float(item[3])/beam*rho*6.242e+12*1e-3 )
                #print item
            cnt+=1
    return data


def load(num, filename):
    tfile = open(filename, "r")
    dataline = tfile.readlines()
    tfile.close()

    global nt

    for eachline in dataline:
        eachline.strip()
        item = eachline.split()
        if len(item)==8 and item[0]=="nt":
            nt = int(item[2])
    print "total time step: %i" %nt

    data = Filter(num, dataline)
    return data

def drawbeam(ax):
    time = np.array([])

def draw(ax, data):
    global it
    ax.errorbar(data["t"], data["e"], xerr=data["dt"]/2., yerr=data["err"], markeredgewidth=0, \
                markersize=7, ls="none", marker=marker[it], linewidth=1.5)
    it+=1

def plot(singlefile):
    fig, ax = plt.subplots(1,1,figsize=(10,5.5))
    data = load(0, singlefile)
    data1 = load(1, singlefile)
    data2 = load(2, singlefile)
    data3 = load(3, singlefile)
    draw(ax, data)
    draw(ax, data1)
    draw(ax, data2)
    draw(ax, data3)
    ax.set_yscale("log")
    #ax.set_ylim([1e-7, 5e-2])
    #ax.set_xlim([0, 30000])
    #ax.set_xscale("log")
    ax.set_xlabel("Time [nsec]", fontsize=17)
    ax.set_ylabel("Energy Deposit [MeV/cm$^3$/proton]", fontsize=17)
    ax.tick_params(axis="both", labelsize=17)

def run(files):
    for eachfile in files:
        data = load(1, eachfile)


if __name__=="__main__":
    #run(sys.argv[1:])
    plot(sys.argv[1])
    plt.tight_layout()
    plt.savefig("timeE.pdf")
    plt.show()
