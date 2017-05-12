#!/usr/bin/env python2.7

import matplotlib.pyplot as plt
import math
import numpy as np
import sys
import re

plt.rcParams['font.family'] = 'Times New Roman'

color  = ["navy", "orangered", "green", "blue"]
marker = ["s", "o", "v", "^"]
label  = ["Kerma Factor", "Charged Paritcle Loss"]

magnet = ["CS0", "CS1", "MS1", "MS2", "TS1a", "TS1b", "TS1c", "TS1d", "TS1e", "TS1f", \
          "TS2a", "TS2-1", "TS2-2", "TS2-3", "TS2-4", "TS2-5", "TS2-6", "TS2-7", "TS2-8", \
          "TS2-9", "TS2-10", "TS2-11", "TS2-12", "TS2-13", "TS2-14", "TS2-15", "TS2-16", "TS3", \
          "BS1", "BS2", "BS3", "BS4", "BS5", "DS1", "DS2", "DS3", "DS4", "DS5", "DS6", "DS7", "DS8", \
          "DS9", "DS10", "DS11", "DS12"]

def getvolumeinfo(dataline):
    num = np.array([])
    vol = np.array([])
    for eachline in dataline:
        eachline.strip()
        item = eachline.split()
        if len(item)==5 and item[3]=="#":
            num = np.append(num, int(item[0]))
            vol = np.append(vol, float(item[2]))
    return num, vol

def getdeposit(dataline):
    edep = np.array([])
    err  = np.array([])
    for eachline in dataline:
        eachline.strip()
        item = eachline.split()
        if len(item)==5 and re.match("\d", item[0]) and item[3]!="#":
            edep = np.append(edep, float(item[3]))
            err  = np.append( err, float(item[4]))
    return edep, err

def load(filename):
    tfile = open(filename, "r")
    dataline = tfile.readlines()
    tfile.close()

    data = {"num":np.array([]), "vol":np.array([]), "edep":np.array([]), "err":np.array([])}
    data["num"], data["vol"] = getvolumeinfo(dataline)
    data["edep"], data["err"] = getdeposit(dataline)
    return data

def SumMagnet(name0, namef, data, err):
    summag = 0.
    magerr = 0.
    for i in range(len(magnet)):
        if magnet[i]==name0:
            start = i
        if magnet[i]==namef:
            end = i
            break
    for i in range(start, end+1):
        summag += data[i]
        magerr += err[i]**2
        #print magnet[i], summag
    magerr = math.sqrt(summag)
    print "%s~%s   %.2f   %.2f"  %(name0, namef, summag, magerr)


def Print(data):
    hgen = data["edep"] * data["vol"] * 3.8 * 1e-3
    herr = data["err"] * hgen
    print "*******************************************************************"
    print " HEAT GENERATION FOR EACH COIL"
    print "*******************************************************************"
    print " %7s  volume[cm3]   dose[Gy/sec]   heat[W]   error" %("name")
    for i in range(len(data["num"])):
        print " %7s  %11.2e  %13.2e   %7.2e  %5.2e" %(magnet[i], data["vol"][i], data["edep"][i], hgen[i],\
              data["edep"][i]*data["err"][i])
    print "*******************************************************************"
    SumMagnet("CS0", "TS1f", hgen, herr)
    SumMagnet("TS2a", "TS3", hgen, herr)
    print "*******************************************************************"


def plot(num, ax, data):
    ax.errorbar(data["num"], data["edep"], xerr=0.5, yerr=data["edep"]*data["err"], linewidth=1.5, \
                marker=marker[num], markeredgewidth=0., c=color[num], markersize=7, ls="none", \
                label=label[num])
    ax.set_yscale("log")
    #ax.set_ylabel("Energy Deposit [Gy/sec]", fontsize=17)
    ax.set_ylabel("Heat Generation [W]", fontsize=17)
    ax.tick_params(axis="both", labelsize=17)
    ax.legend(loc="lower left", numpoints=1)
    ax.set_xlim([0, data["num"][-1]+1])


def ploterror(ax, data1, data2):
    error = (data1["edep"]-data2["edep"])/data1["edep"]
    error_e = np.array([])
    for i in range(len(data1["num"])):
        e = math.sqrt((data2["edep"][i]*data1["err"][i]*data1["edep"][i]/data1["edep"][i]**2)**2 \
          + (data2["err"][i]*data2["edep"][i]/data1["edep"][i])**2)
        error_e = np.append(error_e, e)
    ax.errorbar(data1["num"], error, xerr=0.5, yerr=error_e, linewidth=1.5, marker="o", c="green", \
                markeredgewidth=0, markersize=7, ls="none")
    line = np.array([i for i in range(-2, len(data1["num"])+5)])
    ax.plot(line, line*0., "--", linewidth=1.5, c="k")
    ax.set_ylim([-1.4, 1.4])
    #ax.set_xlabel("Magnets", fontsize=17)
    ax.set_ylabel("$\sigma_{E}/E_1$", fontsize=17)
    ax.tick_params(axis="y", labelsize=17)
    ax.set_xticks(data1["num"])
    ax.set_xticklabels(magnet, rotation=55)
    #ax.grid()

if __name__=="__main__":
    data1 = load(sys.argv[1])
    data2 = load(sys.argv[2])

    Print(data2)

    data1["edep"] = data1["edep"]*data1["vol"]*3.8*1e-3
    data2["edep"] = data2["edep"]*data2["vol"]*3.8*1e-3

    fig = plt.figure(figsize=(10,6.5))
    ax  = plt.subplot2grid( (3,1), (0,0), rowspan=2 )
    #ax.grid()
    ax1 = plt.subplot2grid( (3,1), (2,0), sharex=ax )
    plot(0, ax, data1)
    plot(1, ax, data2)
    ploterror(ax1, data2, data1)
    #plt.xticks(data1["num"], magnet)
    plt.tight_layout()
    fig.subplots_adjust(hspace=0.)
    plt.setp( ax.get_xticklabels(), visible=False )
    plt.savefig("totalloss.pdf")

    plt.show()
