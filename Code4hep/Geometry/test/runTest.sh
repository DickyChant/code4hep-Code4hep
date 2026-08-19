#!/bin/bash

# put included files in test directory
echo $PWD
export DD4HEP_XML_DIR=$(scram_tag dd4hep-core DD4HEP_CORE_BASE)/DDDetectors/compact
ln -sf ${DD4HEP_XML_DIR}/elements.xml .
ln -sf ${DD4HEP_XML_DIR}/materials.xml .
C4H_DIR=$(readlink -f ${PWD}/../../../)
export CMSSW_SEARCH_PATH=$CMSSW_SEARCH_PATH:${C4H_DIR}
cmsRun testGeo.py
