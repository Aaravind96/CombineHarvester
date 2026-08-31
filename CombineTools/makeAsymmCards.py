# Usage:
#   First make sure the right channels, year, and signalType are defined at the top of the file
#   Also note the doBlindedLimits flag
#   python3 makeAsymmCards.py --year 2018 --signal 2b2t
#
#

import os
import re 
import argparse

parser = argparse.ArgumentParser(
    description="Get channel"
)
parser.add_argument("--year", help="Era information: 2016preVFP, 2016postVFP, 2017, 2018", required=True)
parser.add_argument("--signal", help="Type of the signal: 4b2t, 2b2t", required=True)

args = parser.parse_args()
year = args.year
signalType = args.signal

doBlindedLimits = True
channels = ["mutau", "etau", "emu"] # or ["mutau", "etau", "emu"] for multiple

allMasses = {
    "4b2t": [   
                [[30, 15] , [40, 15], [50, 15], [60, 15], [70, 15], [80, 15], [90, 15], [100, 15], [110, 15]],
                [[40, 20], [50, 20], [60, 20], [70, 20], [80, 20], [90, 20], [100, 20]], 
                [[60, 30], [70, 30], [80, 30], [90, 30]],
            ],
    "2b2t": [
                [[20, 15, 0.52259], [30, 15, 0.51097]],
                [[30, 20, 0.48836], [40, 20, 0.47267]],
                [[40, 30, 0.48428], [50, 30, 0.47153], [60, 30, 0.46146]],
                [[50, 40, 0.48722], [60, 40, 0.47712], [70, 40, 0.46881], [80, 40, 0.46154]],
                [[60, 50, 0.48989], [70, 50, 0.48156]],
    ]
}

# Need this to avoid running out of system resources
os.system("ulimit -s unlimited")

for channel in channels:
    for massList in allMasses[signalType]:
        for masspoint in massList:
            command=f"hToA1A2_modified {channel} {signalType} {year} {masspoint[0]} {masspoint[1]} 3 lowMassSR mediumMassSR highMassSR"
            os.system(command)

        os.system("mv hToA1A2*.txt asymmCards/signal2b2t_new/")
        os.system("mv hAsymm*.root asymmCards/signal2b2t_new/")

        for masspoint in massList:
            ch1_filename   = f"asymmCards/signal2b2t_new/hToA1A2_{channel}_1_{year}_{signalType}_{masspoint[0]}_{masspoint[1]}.txt"
            ch2_filename   = f"asymmCards/signal2b2t_new/hToA1A2_{channel}_2_{year}_{signalType}_{masspoint[0]}_{masspoint[1]}.txt"
            ch3_filename   = f"asymmCards/signal2b2t_new/hToA1A2_{channel}_3_{year}_{signalType}_{masspoint[0]}_{masspoint[1]}.txt"
            allch_filename = f"asymmCards/signal2b2t_new/hToA1A2_{signalType}_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.txt"
            os.system(f'echo "* autoMCStats 10 0" >> {ch1_filename}')
            os.system(f'echo "* autoMCStats 10 0" >> {ch2_filename}')
            os.system(f'echo "* autoMCStats 10 0" >> {ch3_filename}')
          #  os.system(f'echo "BRnorm rateParam * ggh2b2t-{masspoint[0]}-{masspoint[1]} {masspoint[2]}" >> {ch1_filename}')
          #  os.system(f'echo "BRnorm rateParam * vbf2b2t-{masspoint[0]}-{masspoint[1]} {masspoint[2]}" >> {ch1_filename}')
          #  os.system(f'echo "BRnorm rateParam * ggh2b2t-{masspoint[0]}-{masspoint[1]} {masspoint[2]}" >> {ch2_filename}')
          #  os.system(f'echo "BRnorm rateParam * vbf2b2t-{masspoint[0]}-{masspoint[1]} {masspoint[2]}" >> {ch2_filename}')
          #  os.system(f'echo "BRnorm rateParam * ggh2b2t-{masspoint[0]}-{masspoint[1]} {masspoint[2]}" >> {ch3_filename}')
          #  os.system(f'echo "BRnorm rateParam * vbf2b2t-{masspoint[0]}-{masspoint[1]} {masspoint[2]}" >> {ch3_filename}')


            os.system(f'combineCards.py {ch1_filename} {ch2_filename} {ch3_filename} > {allch_filename}')

#            # Convert text file to workspace
#            os.system(f'text2workspace.py "{allch_filename}" -m {masspoint[0]}')
#
#
#            allch_workspace_rootfile = f"asymmCards/hToA1A2_{signalType}_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.root"
#            logfile = f"asymmCards/limits_{signalType}_{channel}_{year}_{masspoint[0]}_{masspoint[1]}"
#            if (doBlindedLimits):  # -t -1 option
#                combineCommand = f'combine -M AsymptoticLimits {allch_workspace_rootfile} --rMin=-10 -t -1 -m {masspoint[0]} | tee {logfile}'
#            else: 
#                combineCommand = f'combine -M AsymptoticLimits {allch_workspace_rootfile} -m {masspoint[0]} | tee {logfile}'
#            os.system(combineCommand)
#            
#            # this produces a file called higgsCombineTest.AsymptoticLimits.mH30.root, higgsCombineTest.AsymptoticLimits.mH40.root in the top-level directory for (30, 15)
#            oldName = f'higgsCombineTest.AsymptoticLimits.mH{masspoint[0]}.root'
#            # rename this file to something more descriptive that will not get overwritten, and move it to asymmCards/
#            newName = f"higgsCombineTest.AsymptoticLimits.{signalType}_{channel}_{year}.m2_{masspoint[0]}.m1_{masspoint[1]}.root"
#            os.system(f"mv {oldName} asymmCards/{newName}")

#        # Remember we are itearting over massList, which is e.g. (40, 15), (50, 15), etc. all with the same m_1 = 15 GeV. Get this m_1, can just get the 0th entry, 1st element
#        thisM1Point = massList[0][1]
#        # Next we need to hadd the corresponding mass points but in ascending order. 
#        filesToHadd = ""
#        for masspoint in massList:
#            filesToHadd += f"asymmCards/higgsCombineTest.AsymptoticLimits.{signalType}_{channel}_{year}.m2_{masspoint[0]}.m1_{masspoint[1]}.root"
#            filesToHadd += " "
#        print(filesToHadd)
#        # add the signalType to the file name, to distinguish the cascade and non-cascade ones
#        os.system(f'hadd -f -j -k asymmCards/higgsCombine_a1a2_{signalType}_{channel}_{year}_m1_{thisM1Point}.root {filesToHadd}')
