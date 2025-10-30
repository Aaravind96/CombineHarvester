#echo "######################### 2018 Non Cascade #########################"
#python3 makeAsymmCards-BDT-new.py --year 2018 --signal 2b2t
#python3 combineCardsForOneYear-BDT.py --year 2018 --signal 2b2t
echo "######################### 2018 Cascade #########################"
python3 makeAsymmCards-BDT-new.py --year 2018 --signal 4b2t
#python3 combineCardsForOneYear-BDT.py --year 2018 --signal 4b2t
#echo "######################### 2017 Non Cascade #########################"
#python3 makeAsymmCards-BDT-new.py --year 2017 --signal 2b2t
#python3 combineCardsForOneYear-BDT.py --year 2017 --signal 2b2t
echo "######################### 2017 Cascade #########################"
python3 makeAsymmCards-BDT-new.py --year 2017 --signal 4b2t
#python3 combineCardsForOneYear-BDT.py --year 2017 --signal 4b2t
#echo "######################### 2016preVFP Non Cascade #########################"
#python3 makeAsymmCards-BDT-new.py --year 2016preVFP --signal 2b2t
#python3 combineCardsForOneYear-BDT.py --year 2016preVFP --signal 2b2t
echo "######################### 2016preVFP Cascade #########################"
python3 makeAsymmCards-BDT-new.py --year 2016preVFP --signal 4b2t
#python3 combineCardsForOneYear-BDT.py --year 2016preVFP --signal 4b2t
#echo "######################### 2016postVFP Non Cascade #########################"
#python3 makeAsymmCards-BDT-new.py --year 2016postVFP --signal 2b2t
#python3 combineCardsForOneYear-BDT.py --year 2016postVFP --signal 2b2t
echo "######################### 2016postVFP Cascade #########################"
python3 makeAsymmCards-BDT-new.py --year 2016postVFP --signal 4b2t
#python3 combineCardsForOneYear-BDT.py --year 2016postVFP --signal 4b2t
cd /afs/cern.ch/user/a/aaravind/CMSSW_14_1_0_pre4/src/CombineHarvester/CombineTools/condor
cmsenv
bash submit_job_oneYear.sh mass_4b2t.list 2018
bash submit_job_oneYear.sh mass_4b2t.list 2017
bash submit_job_oneYear.sh mass_4b2t.list 2016postVFP
bash submit_job_oneYear.sh mass_4b2t.list 2016preVFP
#echo "######################### Run-2 Cascade #########################"
#python3 combineCardsForAllYear.py --signal 4b2t
#echo "######################### Run-2 Non Cascade #########################"
#python3 combineCardsForAllYear.py --signal 2b2t
#echo "######################### Plotting limits #########################"
#python3 runLimits.py 
#echo "######################### Plotting Impacts #########################"
#python3 doImpacts.py 
