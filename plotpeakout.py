#!/usr/bin/env python2.7

import numpy as np
import matplotlib.pyplot as plt
import sys

color = ["red", "green"]
marker = ["o", "s"]
factor = 4.4e+13

def load(num, filename, ax):
    tfile = open(filename, "r")
    dataline = tfile.readlines()
    tfile.close()

    data = {"x":np.array([]), "y":np.array([]), "z":np.array([]), \
            "r":np.array([]), "theta":np.array([]), \
            "peak":np.array([]), "perr":np.array([]), \
            "aver":np.array([]), "aerr":np.array([])}

    for eachline in dataline:
        eachline.strip()
        item = eachline.split()
        data["x"] = np.append(data["x"], float(item[0]))
        data["y"] = np.append(data["y"], float(item[1]))
        data["z"] = np.append(data["z"], float(item[2]))
        data["r"] = np.append(data["r"], float(item[3]))
        data["theta"] = np.append(data["theta"], float(item[4]))
        data["peak"] = np.append(data["peak"], float(item[5])*factor)
        data["perr"] = np.append(data["perr"], float(item[6])*factor)
        data["aver"] = np.append(data["aver"], float(item[7])*factor)
        data["aerr"] = np.append(data["aerr"], float(item[8])*factor)

    dz = (data["z"][1] - data["z"][0]) / 2.
    ax.errorbar(data["z"], data["peak"], xerr=dz, yerr=data["perr"], marker=marker[num], color=color[num],\
                linewidth=1.5, markeredgewidth=0, markersize=7, ls="none")

    return ax

def draw(files):
    fig, ax = plt.subplots(1, 1, figsize=(10,6))
    cnt = 0
    for eachfile in files:
        load(cnt, eachfile, ax)
        cnt += 1
    ax.set_yscale("log")
    ax.tick_params(axis="both", labelsize=18)
    ax.set_xlabel("Z [cm]", fontsize=18)
    ax.set_ylabel("Energy Deposition [Gy/sec]", fontsize=18)

if __name__=="__main__":
    files = sys.argv[1:]
    draw(files)
    plt.tight_layout()
    plt.show()
