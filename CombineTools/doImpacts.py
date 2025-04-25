import os
channels=["allchannels"]
year="allyears"
signalType=["2b2t"]
massList=[[30, 15]]

# https://github.com/hftsoi/haabbtt_combine/blob/main/compute_impacts.sh

outputDir=f"/eos/user/p/pdas/www/Ha1a2/limits/170425/"

# Blinded impacts
for channel in channels:
    for signal in signalType:
        for masspoint in massList:
            for value in [1, 0]:
                cardname = f"hToA1A2_{signal}_{channel}_{year}_{masspoint[0]}_{masspoint[1]}.root"
                print(cardname)

                os.system(f'combineTool.py -M Impacts -d asymmCards/{cardname} -m {masspoint[1]} --doInitialFit -t -1 --expectSignal {value} --rMin -50 --rMax 50')

                os.system(f'combineTool.py -M Impacts -d asymmCards/{cardname} -m {masspoint[1]} --doFits --parallel 12 -t -1 --expectSignal {value} --rMin -50 --rMax 50')

                os.system(f'combineTool.py -M Impacts -d asymmCards/{cardname} -m {masspoint[1]} -o impacts.json')

                os.system(f'plotImpacts.py -i impacts.json -o impacts --per-page 30')

                os.system(f'cp impacts.pdf {outputDir}/impacts_{signal}_{channel}_{masspoint[1]}_{masspoint[0]}_expectSignal{value}.pdf')
