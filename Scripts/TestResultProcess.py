import numpy as np
import pandas as pd
import os
import glob
import json

baseDIR = "TestResults"
destBaseDIR = "TestReports"

class Result:
    def __init__(self):
        self.none_alert = []
        self.preventive_alert = []
        self.corrective_alert = []
        self.warning_alert = []

def getTestResult(filename):

    ResultData = pd.read_csv(filename, sep=r'\s*,\s*', engine='python') # use sep='\s*,\s*' so that you will take care of spaces in column-names
    ResultData = ResultData.drop([0]) # delete unit row

    result = Result()

    none_alert = ResultData.loc[ResultData['Alert Level'] == 0, 'Time'].values.tolist()
    result.none_alert = list(map(float, none_alert))
    preventive_alert = ResultData.loc[ResultData['Alert Level'] == 1, 'Time'].values.tolist()
    result.preventive_alert = list(map(float, preventive_alert))
    corrective_alert = ResultData.loc[ResultData['Alert Level'] == 2, 'Time'].values.tolist()
    result.corrective_alert = list(map(float, corrective_alert))
    warning_alert = ResultData.loc[ResultData['Alert Level'] == 3, 'Time'].values.tolist()
    result.warning_alert = list(map(float, warning_alert))
    return result

def ranges(nums):
    gaps = [[s, e] for s, e in zip(nums, nums[1:]) if s + 1 < e]
    edges = iter(nums[:1] + sum(gaps, []) + nums[-1:])
    return list(zip(edges, edges))

def storeResult(result, filename, TestResult):
    #4 alert levels, stored by 2d-list [[start, end], [start, end]]
    None_Alert_SUM = ranges(result.none_alert)
    Preventive_Alert_SUM = ranges(result.preventive_alert)
    Corrective_Alert_SUM = ranges(result.corrective_alert)
    Warning_Alert_SUM = ranges(result.warning_alert)
    casename = filename.split('\\')[1]
    TestResult[casename] = []
    TestResult[casename].append({
        'None Alert': None_Alert_SUM,
        'Preventive Alert': Preventive_Alert_SUM,
        'Corrective Alert': Corrective_Alert_SUM,
        'Warning Alert': Warning_Alert_SUM
    })
    return TestResult

if __name__ == "__main__":
    TestResult = {}
    for file in glob.glob(baseDIR + '/' + '*.csv'):
        result = getTestResult(file)
        TestResult = storeResult(result, file, TestResult)
    with open(destBaseDIR + '\\report.json', 'w') as outfile:
        json.dump(TestResult, outfile, indent = 2)
