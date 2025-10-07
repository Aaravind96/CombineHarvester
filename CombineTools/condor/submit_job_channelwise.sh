file=${1}

[ -d output ] || mkdir output
[ -d error ] || mkdir error
[ -d log ] || mkdir log

while read -r line; do

signal=$(echo $line | awk '{print $1}')
mass1=$(echo $line | awk '{print $2}')
mass2=$(echo $line | awk '{print $3}')

for channel in mutau etau emu; do

cat>Job_${signal}_${channel}_${mass1}_${mass2}.sh<<EOF
#!/bin/bash
cd /afs/cern.ch/work/p/pdas/haa/luna/CMSSW_14_1_0_pre4/src/CombineHarvester/CombineTools/
cmsenv
ulimit -s unlimited

cd asymmCards/

combineCards.py hToA1A2_${signal}_${channel}_2018_${mass1}_${mass2}.txt hToA1A2_${signal}_${channel}_2017_${mass1}_${mass2}.txt hToA1A2_${signal}_${channel}_2016postVFP_${mass1}_${mass2}.txt hToA1A2_${signal}_${channel}_2016preVFP_${mass1}_${mass2}.txt > hToA1A2_${signal}_${channel}_allyears_${mass1}_${mass2}.txt

cd ../

text2workspace.py asymmCards/hToA1A2_${signal}_${channel}_allyears_${mass1}_${mass2}.txt -m ${mass1}
cd condor/
mkdir ${signal}_${channel}_${mass1}_${mass2}
cd ${signal}_${channel}_${mass1}_${mass2}
combine -M AsymptoticLimits /afs/cern.ch/work/p/pdas/haa/luna/CMSSW_14_1_0_pre4/src/CombineHarvester/CombineTools/asymmCards/hToA1A2_${signal}_${channel}_allyears_${mass1}_${mass2}.root --rMin=-10 -m ${mass1} | tee limits_${channel}_${signal}_${mass1}_${mass2}

EOF

chmod 755 Job_${signal}_${channel}_${mass1}_${mass2}.sh

sed 's/template/'${signal}_${channel}_${mass1}_${mass2}'/g' < template_condor_script > condor_${signal}_${channel}_${mass1}_${mass2}

condor_submit condor_${signal}_${channel}_${mass1}_${mass2}

done

done < $file

exit 0
