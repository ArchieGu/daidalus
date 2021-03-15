import pandas as pd
import numpy as np
import os
import multiprocessing
import glob

baseDIR = "D:\Personal\RTCA DO-365 MOPS_Test_Vectors"
destBaseDIR = "D:\Personal\TestADSB"

def getTestCaseFile(baseDIR):
    for testfile in glob.glob(baseDIR + '\**\**\**\**\*_ADSB_Tracke*'):
        TrackerFile = pd.read_csv(testfile)

        cols_to_keep = ['TOA(s)', 'ICAO', 'LAT(deg)', 'LON(deg)', 'ALT(ft)', 'VS(ft/min)', 'HDG(deg)', 'GS(kts)']
        destDIR = destBaseDIR + '\\' + "\\".join(testfile.split('\\')[4:-1])

        if not os.path.exists(destDIR):
            os.makedirs(destDIR)
        TestFileName = destDIR + '\\' + testfile.split('\\')[-1]
        TrackerFile.to_csv(TestFileName, columns = cols_to_keep, index = False)

getTestCaseFile(baseDIR)
