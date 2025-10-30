file=${1}

[ -d output ] || mkdir output
[ -d error ] || mkdir error
[ -d log ] || mkdir log

while read -r line; do

signal=$(echo $line | awk '{print $1}')
mass1=$(echo $line | awk '{print $2}')
mass2=$(echo $line | awk '{print $3}')

#--------------------------------------------------------
# Get the voms-proxy-info certificate
#--------------------------------------------------------
#export MYPROXYPATH="$(voms-proxy-info -path)"
#
## Use regex to extract the first letter of the username. ^ is the beginning of the string. \w means any alphanumeric character from the Latin alphabet. ( ) captures the variable
#echo ${USER}
#re="^(\w)"
## Bash's =~ operator matches the left hand side to the regex on the right hand side
#[[ ${USER} =~ ${re} ]]
## The results of the regex can be extracted from the bash variable BASH_REMATCH. Index [0] returns all of the matches.
#USER_FIRST_LETTER=${BASH_REMATCH[0]}
#
#
#if [[ -f ${MYPROXYPATH} ]]; then
#    echo ">>> submit_job.sh: Copying proxy from ${MYPROXYPATH} to /afs/cern.ch/user/${USER_FIRST_LETTER}/${USER}/private/x509up_file"
#    cp ${MYPROXYPATH} /afs/cern.ch/user/${USER_FIRST_LETTER}/${USER}/private/x509up_file
#else
#    echo ">>> ${BASH_SOURCE[0]}: [ERROR]: x509 proxy not found on this machine, make sure voms-proxy-init was run, exiting"
#    exit 1
#fi
#
cat>Job_${signal}_${mass1}_${mass2}.sh<<EOF
#!/bin/bash
cd /afs/cern.ch/user/a/aaravind/CMSSW_14_1_0_pre4/src/CombineHarvester/CombineTools/
cmsenv
ulimit -s unlimited
export X509_USER_PROXY=\$1
cd asymmCards/BDT; combineCards.py hToA1A2_${signal}_allchannels_2018_${mass1}_${mass2}.txt hToA1A2_${signal}_allchannels_2017_${mass1}_${mass2}.txt hToA1A2_${signal}_allchannels_2016postVFP_${mass1}_${mass2}.txt hToA1A2_${signal}_allchannels_2016preVFP_${mass1}_${mass2}.txt > hToA1A2_${signal}_allchannels_allyears_${mass1}_${mass2}.txt; cd ../../
text2workspace.py asymmCards/BDT/hToA1A2_${signal}_allchannels_allyears_${mass1}_${mass2}.txt -m ${mass1}
cd condor/
mkdir ${signal}_${mass1}_${mass2}
cd ${signal}_${mass1}_${mass2}
combine -M AsymptoticLimits /afs/cern.ch/user/a/aaravind/CMSSW_14_1_0_pre4/src/CombineHarvester/CombineTools/asymmCards/BDT/hToA1A2_${signal}_allchannels_allyears_${mass1}_${mass2}.root --rMin=-10 -t -1 -m ${mass1} | tee limits_allchannels_${signal}_${mass1}_${mass2}

EOF

chmod 755 Job_${signal}_${mass1}_${mass2}.sh

sed 's/template/'${signal}_${mass1}_${mass2}'/g' < template_condor_script > condor_${signal}_${mass1}_${mass2}

condor_submit condor_${signal}_${mass1}_${mass2}

done < $file

exit 0

#hadd -f -j -k higgsCombine_a1a2_4b2t_allchannels_allyears_m1_30.root 4b2t_*_30/higgsCombineTest.AsymptoticLimits*.root and so on after condor jobs finish running
