import os
channels=["allchannels"]
year="allyears"
signalType=["4b2t", "2b2t"]
massList=[[30, 15]]

# https://github.com/hftsoi/haabbtt_combine/blob/main/compute_impacts.sh

outputDir=f"/eos/user/p/pdas/www/Ha1a2/limits/190225/"

# Blinded impacts
for channel in channels:
    for signal in signalType:
        for masspoint in massList:
            for value in [0, 1]:
                cardname = f"higgsCombine_a1a2_{signal}_{year}_{channel}_m1_{masspoint[1]}.root"
                print(cardname)

                os.system(f'combineTool.py -M Impacts -d asymmCards/hToA1A2_allchannels_allcats_allyears_30_15.root -m {masspoint[0]} --doInitialFit --robustFit 1 -t -1 --expectSignal {value} --rMin -40 --rMax 40')

                os.system(f'combineTool.py -M Impacts -d asymmCards/hToA1A2_allchannels_allcats_allyears_30_15.root -m {masspoint[0]} --doFits --robustFit 1 --parallel 12 -t -1 --expectSignal {value} --rMin -40 --rMax 40')

                os.system(f'combineTool.py -M Impacts -d asymmCards/hToA1A2_allchannels_allcats_allyears_30_15.root -m {masspoint[0]} -o impacts.json')

                os.system(f'plotImpacts.py -i impacts.json -o impacts --per-page 20')

                os.system(f'cp impacts.pdf {outputDir}/impacts_{signal}_{channel}_{masspoint[1]}_{masspoint[0]}_expectSignal{value}.pdf')
