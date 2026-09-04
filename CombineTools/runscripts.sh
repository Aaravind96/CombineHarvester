#echo "######################### 2018 Cascade #########################"
#python3 makeAsymmCardsBDT.py --year 2018 --signal 4b2t
#python3 combineCardsForOneYearBDT.py --year 2018 --signal 4b2t
echo "######################### 2018 Non Cascade #########################"
python3 makeAsymmCardsBDT.py --year 2018 --signal 2b2t
python3 combineCardsForOneYearBDT.py --year 2018 --signal 2b2t
#echo "######################### 2017 Cascade #########################"
#python3 makeAsymmCardsBDT.py --year 2017 --signal 4b2t
#python3 combineCardsForOneYearBDT.py --year 2017 --signal 4b2t
echo "######################### 2017 Non Cascade #########################"
python3 makeAsymmCardsBDT.py --year 2017 --signal 2b2t
python3 combineCardsForOneYearBDT.py --year 2017 --signal 2b2t
#echo "######################### 2016preVFP Cascade #########################"
#python3 makeAsymmCardsBDT.py --year 2016preVFP --signal 4b2t
#python3 combineCardsForOneYearBDT.py --year 2016preVFP --signal 4b2t
echo "######################### 2016preVFP Non Cascade #########################"
python3 makeAsymmCardsBDT.py --year 2016preVFP --signal 2b2t
python3 combineCardsForOneYearBDT.py --year 2016preVFP --signal 2b2t
#echo "######################### 2016postVFP Cascade #########################"
#python3 makeAsymmCardsBDT.py --year 2016postVFP --signal 4b2t
#python3 combineCardsForOneYearBDT.py --year 2016postVFP --signal 4b2t
echo "######################### 2016postVFP Non Cascade #########################"
python3 makeAsymmCardsBDT.py --year 2016postVFP --signal 2b2t
python3 combineCardsForOneYearBDT.py --year 2016postVFP --signal 2b2t
#echo "######################### Run-2 Cascade #########################"
#python3 combineCardsForAllYearBDT.py --signal 4b2t
#echo "######################### Run-2 Non Cascade #########################"
#python3 combineCardsForAllYearBDT.py --signal 2b2t
#echo "######################### Plotting limits #########################"
#python3 runLimits.py 
#echo "######################### Plotting Impacts #########################"
#python3 doImpacts.py 
