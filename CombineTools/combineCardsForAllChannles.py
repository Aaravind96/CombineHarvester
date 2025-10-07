# Usage:
#   python3 combineCardsForAllYear.py --signal 2b2t
#
# Prerequisites: This script depends on all the necessary input files being present in the asymmCards/ directory, 
#                i.e. you need to have run makeAsymmCards.py for each of the three tautau channels to combine.
# 

import os
import argparse

parser = argparse.ArgumentParser(
    description="Get channel"
)
parser.add_argument("--signal", help="Type of the signal: 4b2t, 2b2t", required=True)

args = parser.parse_args()
signalType = args.signal

doBlindedLimits = True

os.system("ulimit -s unlimited")

channels = ["mutau", "etau", "emu"] # or ["mutau", "etau", "emu"] for multiple

allMasses = {
    "4b2t": [  
                [[30, 15], [40, 15], [50, 15], [60, 15], [70, 15], [80, 15], [90, 15], [100, 15], [110, 15]],
                [[40, 20], [50, 20], [60, 20], [70, 20], [80, 20], [90, 20], [100, 20]], 
                [[60, 30], [70, 30], [80, 30], [90, 30]]
            ],
    "2b2t": [
                [[20, 15], [30, 15]],
                [[30, 20], [40, 20]],
                [[40, 30], [50, 30], [60, 30]],
                [[50, 40], [60, 40], [70, 40], [80, 40]],
                [[60, 50], [70, 50]],
    ]
}

for channel in channels:
   for massList in allMasses[signalType]:
       for masspoint in massList:
           print(f"Combining .txt cards for all year...")
           # Note the multi-line command, we need to cd into asymmCards/ to run combineCards.py, otherwise the .txt file will try to look for a double-nested folder asymmCards/asymmCards/ which does not exist
           os.system(f'cd asymmCards/; combineCards.py hToA1A2_{signalType}_{channel}_2018_{masspoint[0]}_{masspoint[1]}.txt hToA1A2_{signalType}_{channel}_2017_{masspoint[0]}_{masspoint[1]}.txt hToA1A2_{signalType}_{channel}_2016postVFP_{masspoint[0]}_{masspoint[1]}.txt hToA1A2_{signalType}_{channel}_2016preVFP_{masspoint[0]}_{masspoint[1]}.txt > hToA1A2_{signalType}_{channel}_allyears_{masspoint[0]}_{masspoint[1]}.txt; cd ../')
