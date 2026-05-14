import os

plotdir = "/eos/user/p/pdas/www/Ha1a2/dataMC/PAPER/v15/limits/"

# false for expected limits only, true for expected and observed
showObserved = "true"

channels = ["allchannels", "mutau", "etau", "emu"]
years = ["allyears"]
for year in years:
    for channel in channels:
#        os.system(f'rm median_limits_{channel}.txt')
#        os.system(f'touch median_limits_{channel}.txt')
#        ### Cascade ###
#        os.system(
#            f'root -l -b -q \'plotLimitsAsymm.cpp("higgsCombine_a1a2_4b2t_{channel}_{year}_m1_15.root", "4b2t", "{year}", "{channel}", "15", 2, {showObserved})\''
#        )
#        os.system(
#            f'root -l -b -q \'plotLimitsAsymm.cpp("higgsCombine_a1a2_4b2t_{channel}_{year}_m1_20.root", "4b2t", "{year}", "{channel}", "20", 2, {showObserved})\''
#         )
#        os.system(
#            f'root -l -b -q \'plotLimitsAsymm.cpp("higgsCombine_a1a2_4b2t_{channel}_{year}_m1_30.root", "4b2t", "{year}", "{channel}", "30", 2, {showObserved})\''
#        )
#        ### Non Cascade ###
#        os.system(
#            f'root -l -b -q \'plotLimitsAsymm.cpp("higgsCombine_a1a2_2b2t_{channel}_{year}_m1_15.root", "2b2t", "{year}", "{channel}", "15", 2, {showObserved})\''
#        )
#        os.system(
#            f'root -l -b -q \'plotLimitsAsymm.cpp("higgsCombine_a1a2_2b2t_{channel}_{year}_m1_20.root", "2b2t", "{year}", "{channel}", "20", 2, {showObserved})\''
#        )
#        os.system(
#            f'root -l -b -q \'plotLimitsAsymm.cpp("higgsCombine_a1a2_2b2t_{channel}_{year}_m1_30.root", "2b2t", "{year}", "{channel}", "30", 2, {showObserved})\''
#        )
#        os.system(
#            f'root -l -b -q \'plotLimitsAsymm.cpp("higgsCombine_a1a2_2b2t_{channel}_{year}_m1_40.root", "2b2t", "{year}", "{channel}", "40", 2, {showObserved})\''
#        )
#        os.system(
#            f'root -l -b -q \'plotLimitsAsymm.cpp("higgsCombine_a1a2_2b2t_{channel}_{year}_m1_50.root", "2b2t", "{year}", "{channel}", "50", 2, {showObserved})\''
#        )
        ### 2D limits ###
        os.system(
            f'root -l -b -q \'plot2d.C("{channel}")\''
        )

        os.system(f"cp plotLimit_*{channel}*.png {plotdir}")
        os.system(f"cp plotLimit_*{channel}*.pdf {plotdir}")
        os.system(f"rm plotLimit_*{channel}*")

