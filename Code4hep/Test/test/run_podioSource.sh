#!/bin/bash

# Pass in name and status
function die { echo $1: status $2 ;  exit $2; }

F1=testPodioSource_cfg.py
F2=testPodioSource2_cfg.py

# Create the deliberately small input fixture maintained alongside this test.
# EDM4hep 1.0 no longer installs its former createEDM4hepFile.py helper.
python3 create_test_edm4hep_file.py

# We need a second file to test that multiple files can be read.
# The easiest way to create a second file is to copy the first one.
cp edm4hep.root edm4hep_copy.root

# The following test checks that the expected run, lumi and event
# transitions occur as the Framework processes the file.
# It also checks that run, lumi and event numbers delivered from the
# Run, Lumi and Event objects have the expected values.
# The test depends on the content of the input files. These
# input files are created by create_test_edm4hep_file.py above.
(cmsRun $F1 ) || die "Failure using $F1" $?
(cmsRun $F2 ) || die "Failure using $F2" $?
