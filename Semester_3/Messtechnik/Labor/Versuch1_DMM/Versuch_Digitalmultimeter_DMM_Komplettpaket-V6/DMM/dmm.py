# -*- coding: utf-8 -*-
"""
Created on Tue Oct 22 19:36:45 2019

@author: thch1015
"""

import os
import time
import datetime
import pandas as pd
import serial
from IPython.display import clear_output
from termcolor import colored
import matplotlib.pyplot as plt

vPort = ''
cPort = ''
name = 'VersuchDMM.jpg'

def connectDMM(port):
    connection = serial.Serial(port, timeout = 1)
    print('Multimeter connected to {}'.format(port))
    return connection

def readPrimary(connection):
    # noch checken, ob Verbindung offen
    connection.write(b'R1\r')
    # primary display value is transmitted first
    val = connection.readline().decode()
    # acknowlodge is transmitted second
    ack = connection.readline().decode()
    if ack.find("=>") < 0 : 
        val = 9.99E9
    return val

def measureRemote(nSwitch, nMeasure):
    vConn = connectDMM(vPort)
    cConn = connectDMM(cPort)
    result = pd.DataFrame()
    resistance = pd.DataFrame()
    for switch in range(nSwitch):
        counter = switch + 1
        vMeasureList = []
        cMeasureList = []
        clear_output()
        input('Bitte schalten Sie das Relais und bestätigen Sie Schaltvorgang {} mit ENTER'.format(counter))
        print(colored('Messung läuft! Bitte warten!', 'red'))
        for measure in range(nMeasure):
            vMeasureList.append(float(readPrimary(vConn)))
            cMeasureList.append(float(readPrimary(cConn)))
            time.sleep(0.1)
        vColName = 'voltage_' + str(counter)
        cColName = 'current_' + str(counter)
        result[vColName] = vMeasureList
        result[cColName] = cMeasureList
        #try : 
        #    resistance = result[vColName]/result[cColName]
        #except: 
        #    vConn.close()
        #    cConn.close()
    clear_output()
    vConn.close()
    cConn.close()
    print('Messreihenaufnahme beendet...')
    #return result, resistance
    return result

def measureRemote_Thiele(nSwitch, nMeasure):
    vConn = connectDMM(vPort)
    cConn = connectDMM(cPort)
    result = pd.DataFrame()
    resistance = pd.DataFrame()
    for switch in range(nSwitch):
        counter = switch + 1
        vMeasureList = []
        cMeasureList = []
        clear_output()
        input('Bitte schalten Sie das Relais und bestätigen Sie Schaltvorgang {} mit ENTER'.format(counter))
        print(colored('Messung läuft! Bitte warten!', 'red'))
        for measure in range(nMeasure):
             vMeasureList.append(str(readPrimary(vConn)))
             cMeasureList.append(str(readPrimary(cConn)))
             time.sleep(0.1)
        vColName = 'voltage_' + str(counter)
        cColName = 'current_' + str(counter)
        result[vColName] = vMeasureList
        result[cColName] = cMeasureList
        resistance = result[vColName]/result[cColName]
    clear_output()
    print('Messreihenaufnahme beendet...')
    return result, resistance

def meanErrorPlot(frame):
    means = frame.mean(axis=0)
    # print(means)
    std_devs = frame.std(axis=0)
    # print(std_devs)
    mean_mean = means.mean()
    # print(mean_mean)
    mean_median = means.median()
    # print(mean_median)

    plt.figure(figsize=(16, 9))
    plt.errorbar(x=frame.columns,
                 y=means,
                 yerr=std_devs,
                 linestyle='None',
                 marker='o',
                 ecolor='y',
                 capsize=10)
    plt.hlines(mean_mean,
               frame.columns[0],
               frame.columns[-1],
               colors='g',
               label='Mittelwert')
    plt.hlines(mean_median,
               frame.columns[0],
               frame.columns[-1],
               colors='m',
               label='Median')
    plt.legend(labels=['Mittelwert', 'Median', 'Messwerte(Mittelwert)'])
    # plt.savefig(r'C:\Users\chris\Nextcloud\HSKA\WS1920\HiWi\Projekte\DMM_jupyter\plots\test.png')
    plt.show()
    return means, std_devs, mean_mean, mean_median
    
    
def flushSerialInput(connection) :
    ackString = "default"
        
    while (len(ackString) > 0) : 
        ackString = connection.readline()
        # print(ackString)
        print('{} \t {}'.format(connection.port, ackString))
    return ackString