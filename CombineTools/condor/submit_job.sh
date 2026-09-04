file=${1}

[ -d output ] || mkdir output
[ -d error ] || mkdir error
[ -d log ] || mkdir log

while read -r line; do

signal=$(echo $line | awk '{print $1}')
mass1=$(echo $line | awk '{print $2}')
mass2=$(echo $line | awk '{print $3}')

cat>Job_${signal}_${mass1}_${mass2}.sh<<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/aaravind/CMSSW_14_1_0_pre4/src/CombineHarvester/CombineTools/
cmsenv
ulimit -s unlimited
cd asymmCards/signal2b2t_new/; combineCards.py hToA1A2_${signal}_allchannels_2018_${mass1}_${mass2}.txt hToA1A2_${signal}_allchannels_2017_${mass1}_${mass2}.txt hToA1A2_${signal}_allchannels_2016postVFP_${mass1}_${mass2}.txt hToA1A2_${signal}_allchannels_2016preVFP_${mass1}_${mass2}.txt > hToA1A2_${signal}_allchannels_allyears_${mass1}_${mass2}.txt; cd ../../
text2workspace.py asymmCards/signal2b2t_new/hToA1A2_${signal}_allchannels_allyears_${mass1}_${mass2}.txt -m ${mass1}
cd condor/
mkdir ${signal}_${mass1}_${mass2}_signal2b2t_new
cd ${signal}_${mass1}_${mass2}_signal2b2t_new
combine -M AsymptoticLimits /afs/cern.ch/user/a/aaravind/CMSSW_14_1_0_pre4/src/CombineHarvester/CombineTools/asymmCards/signal2b2t_new/hToA1A2_${signal}_allchannels_allyears_${mass1}_${mass2}.root --rMin=-10 -m ${mass1} | tee limits_allchannels_${signal}_${mass1}_${mass2}

EOF

chmod 755 Job_${signal}_${mass1}_${mass2}.sh

sed 's/template/'${signal}_${mass1}_${mass2}'/g' < template_condor_script > condor_${signal}_${mass1}_${mass2}

condor_submit condor_${signal}_${mass1}_${mass2}

done < $file

exit 0

#hadd -f -j -k higgsCombine_a1a2_4b2t_allchannels_allyears_m1_30.root 4b2t_*_30/higgsCombineTest.AsymptoticLimits*.root and so on after condor jobs finish running
