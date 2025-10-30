import os
from pathlib import Path

channels=["allchannels"]# "allchannels"]
year= "2018" #"allyears"
signalType=["2b2t"]
massList=[[60, 40]]

# https://github.com/hftsoi/haabbtt_combine/blob/main/compute_impacts.sh

outputDir=f"/eos/user/a/aaravind/www/Ha1a2/limits/300725/"
Path(outputDir).mkdir(parents=True, exist_ok=True)
# Blinded impacts
for channel in channels:
    for signal in signalType:
        for masspoint in massList:
            for value in [1,0]:#, 0]:
                cardname = f"hToA1A2_{signal}_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.root"
                print(cardname)

                os.system(f'combineTool.py -M Impacts -d asymmCards/BDT/{cardname} -m {masspoint[1]} --doInitialFit --robustFit 1 -t -1 --expectSignal {value} --rMin -100 --rMax 100 --cminDefaultMinimizerStrategy 0 --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstants  --X-rtd MINIMIZER_multiMin_maskConstraints --X-rtd MINIMIZER_multiMin_maskChannels=2')
                print("Initial fit done")
                #os.system(f'combineTool.py -M Impacts -d asymmCards/BDT/{cardname} -m {masspoint[1]} --robustFit=1 --doFits --parallel 12 -t -1 --expectSignal {value} --rMin -100 --rMax 100 --job-mode condor --task-name condor-test --sub-opts='+JobFlavour="tomorrow"' --dry-run')
                os.system(
                f'combineTool.py -M Impacts -d asymmCards/BDT/{cardname} '
                f'-m {masspoint[1]} --robustFit 1 --doFits --parallel 12 '
                f'-t -1 --expectSignal {value} --rMin -100 --rMax 100 --cminDefaultMinimizerStrategy 0 --X-rtd MINIMIZER_freezeDisassociatedParams --X-rtd MINIMIZER_multiMin_hideConstants  --X-rtd MINIMIZER_multiMin_maskConstraints --X-rtd MINIMIZER_multiMin_maskChannels=2'
                f'--job-mode condor --task-name condor-allyears '
                f'--sub-opts=\'+JobFlavour="tomorrow"\'')
                #os.system(f"condor_submit  condor_condor-allyears.sub")
                #os.system(f'combineTool.py -M Impacts -d asymmCards/BDT/{cardname} -m {masspoint[1]} -o impacts_{channel}_{year}.json')

                #os.system(f'plotImpacts.py -i impacts_{channel}_{year}.json -o impacts --per-page 30')

                #os.system(f'cp impacts.pdf {outputDir}/impacts_{year}_{signal}_{channel}_{masspoint[1]}_{masspoint[0]}_expectSignal{value}.pdf')

#text2workspace.py datacards/datacard_M1800_mconly_nopdfoutput_impact_UL201678_rescale.dat -m 125 -o workspace_rescale_rmin_UL201678.root
#combineTool.py -M Impacts -d workspace_rescale_rmin_UL201678.root -m 125 --rMin -10 --doInitialFit --robustFit 1
#combineTool.py -M Impacts -d workspace_rescale_rmin_UL201678.root -m 125 --rMin -10 --job-mode condor --task-name condor-test --sub-opts='+JobFlavour="tomorrow"' --robustFit 1 --doFits --dry-run
#condor_submit [jds file]
#combineTool.py -M Impacts -d workspace_rescale_rmin_UL201678.root -m 125 -o impacts_rescale_rmin_UL201678.json
#plotImpacts.py -i impacts_rescale_rmin_UL201678.json -o plot_impacts_rescale_rmin_UL201678
