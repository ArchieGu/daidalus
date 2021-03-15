import pandas as pd
import numpy as np
import os
import multiprocessing
import glob

baseDIR = "D:\Personal\RTCA DO-365 MOPS_Test_Vectors"
destBaseDIR = "D:\Personal\RTCA DO-365 MOPS_Test_Cases"

scenarios = ['converging', 'designer', 'dynamic', 'headon', 'highspeed', 'maneuver', 'overtaking']

def getTestCaseFile(baseDIR):
    for testfile in glob.glob(baseDIR + '\**\**\**\*_Truth_TV*'):
        ModifyGS = pd.read_csv(testfile)
        EWV = ModifyGS['EWV(kts)']
        NSV = ModifyGS['NSV(kts)']
        ModifyGS['GS(kts)'] = pd.Series(np.sqrt(np.square(EWV) + np.square(NSV)))
        ModifyGS = ModifyGS.drop(['EWV(kts)', 'NSV(kts)'], axis=1)
        destDIR = destBaseDIR + '\\' + "\\".join(testfile.split('\\')[3:-1])

        if not os.path.exists(destDIR):
            os.makedirs(destDIR)
        TestFileName = destDIR + '\\' + testfile.split('\\')[-1]
        ModifyGS.to_csv(TestFileName, index = False)

getTestCaseFile(baseDIR)
