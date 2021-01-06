#!/bin/bash

samples=`ls output/mc16a/ | cut -d . -f 1`

for sample in ${samples}
do
  echo ${sample}
  hadd output/fullrun2/${sample}.root output/mc16*/${sample}.root
done

hadd output/fullrun2/data_run2.root output/data1*/data.root
