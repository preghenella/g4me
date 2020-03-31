# Copyright CERN and copyright holders of ALICE O2. This software is distributed
# under the terms of the GNU General Public License v3 (GPL Version 3), copied
# verbatim in the file "COPYING".
#
# See http://alice-o2.web.cern.ch/license for full licensing information.
#
# In applying this license CERN does not waive the privileges and immunities
# granted to it by virtue of its status as an Intergovernmental Organization or
# submit itself to any jurisdiction.

find_path(HepMC3_INCLUDE_DIR
          NAMES HepMC3.h
          PATH_SUFFIXES HepMC3
          PATHS $ENV{HEPMC3_ROOT}/include)

set(HepMC3_INCLUDE_DIR ${HepMC3_INCLUDE_DIR}/..)
	
find_library(HepMC3_LIBRARIES
             NAMES libHepMC3.so 
             PATHS $ENV{HEPMC3_ROOT}/lib)
