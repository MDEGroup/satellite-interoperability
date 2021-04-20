#-----------------------------------------------------------------------------
#
# (C) 2021 European Space Agency
# European Space Operations Centre
# Darmstadt, Germany
#
#-----------------------------------------------------------------------------
#
# File Name:     CMakeLists.txt
#
# Author:        UMF
# 
# Description:   CMakeLists for the tasi_sve_dss Package
#
# Note:          This file has been generated using the UMF CMakeLists Generator
#
#-----------------------------------------------------------------------------
#-----------------------------------------------------------------------------

# --OPENING ELEMENT--CMakeLists::header--

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)
SET(THIS_PROJECT tasi.sve.dss)
PROJECT(${THIS_PROJECT}
# MARKER: CMAKE PROJECT SETTINGS: START
# Add additional project settings here
# MARKER: CMAKE PROJECT SETTINGS: END
)

# --CLOSING ELEMENT--CMakeLists::header--

# --OPENING ELEMENT--CMakeLists::include-directories--

# include directories
# tasi.sve.dss_SOURCE_DIR is the "root" directory of tasi.sve.dss
INCLUDE_DIRECTORIES(${tasi.sve.dss_SOURCE_DIR}/ext)
INCLUDE_DIRECTORIES(${tasi.sve.dss_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${SOLUTION_SOURCE_DIR}/tasi.sve.common/ext)
INCLUDE_DIRECTORIES(${SOLUTION_SOURCE_DIR}/tasi.sve.common/src)
INCLUDE_DIRECTORIES(${SOLUTION_SOURCE_DIR}/isis.systemif.interface.smp2/ext)
INCLUDE_DIRECTORIES(${SOLUTION_SOURCE_DIR}/isis.systemif.interface.smp2/src)
# MARKER: CMAKE INCLUDE DIRECTORIES: START
# Add additional include directories here
# MARKER: CMAKE INCLUDE DIRECTORIES: END

# --CLOSING ELEMENT--CMakeLists::include-directories--

# --OPENING ELEMENT--CMakeLists::link-directories--

# link directories for dependent libraries
# MARKER: CMAKE LINK DIRECTORIES: START
# Add additional link directories here
# MARKER: CMAKE LINK DIRECTORIES: END

# --CLOSING ELEMENT--CMakeLists::link-directories--

# --OPENING ELEMENT--CMakeLists::sources--

# MARKER: CMAKE FILES: START
# Add additional file definitions here
# get all files ending with .cpp or .h* in the src and ext directories
FILE(GLOB_RECURSE SRC_SOURCES src/*.cpp src/*.h*)
FILE(GLOB_RECURSE EXT_SOURCES ext/*.cpp ext/*.h*)
# MARKER: CMAKE FILES: END

# --CLOSING ELEMENT--CMakeLists::sources--

# --OPENING ELEMENT--CMakeLists::main-target--

# library
ADD_LIBRARY(${THIS_PROJECT}
            SHARED
            ${SRC_SOURCES}
            ${EXT_SOURCES}
# MARKER: CMAKE LIBRARY FILES: START
# Add additional files for the library here
# MARKER: CMAKE LIBRARY FILES: END
            )

# --CLOSING ELEMENT--CMakeLists::main-target--

# --OPENING ELEMENT--CMakeLists::target-link-libraries--

# link targets for dependent libraries
TARGET_LINK_LIBRARIES(${THIS_PROJECT} tasi.sve.common)
TARGET_LINK_LIBRARIES(${THIS_PROJECT} esa.smp2.mdk)
TARGET_LINK_LIBRARIES(${THIS_PROJECT} esa.smp2.smp)
# MARKER: CMAKE TARGET LINK LIBRARIES: START
# Add additional target link libraries here
# MARKER: CMAKE TARGET LINK LIBRARIES: END

# --CLOSING ELEMENT--CMakeLists::target-link-libraries--

# --OPENING ELEMENT--CMakeLists::custom-settings--

# MARKER: CMAKE ADDITIONAL COMMANDS: START
# Add custom settings here
# MARKER: CMAKE ADDITIONAL COMMANDS: END

# --CLOSING ELEMENT--CMakeLists::custom-settings--

# --OPENING ELEMENT--CMakeLists::installation--

# install the created library and the project's header files
# into the solution's install/lib and install/include directories
INSTALL_LIB_AND_HEADERS()

# install SMDL documents from the umf sub-directory
# into the solution's install/smp directory
INSTALL_UMF_FILES()

# MARKER: CMAKE INSTALL COMMANDS: START
# Add custom installation commands here
# MARKER: CMAKE INSTALL COMMANDS: END

# --CLOSING ELEMENT--CMakeLists::installation--

# --OPENING ELEMENT--CMakeLists::properties--

# properties
SET_TARGET_PROPERTIES(${THIS_PROJECT} PROPERTIES
                      LINKER_LANGUAGE CXX
                      SOVERSION ${SOLUTION_SOVERSION}
                      VERSION ${SOLUTION_VERSION})

# MARKER: CMAKE PROPERTIES: START
# Add custom properties here
# MARKER: CMAKE PROPERTIES: END

# --CLOSING ELEMENT--CMakeLists::properties--
