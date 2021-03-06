

#
# A) Package-specific configuration options
#

TRIBITS_CONFIGURE_FILE(${PACKAGE_NAME}_Config.h)

#
# B) Define the header and source files (and directories)
#

#
# src
#

SET(HEADERS "")
SET(SOURCES "")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET(HEADERS ${HEADERS}
  ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}_Config.h
  )

#
# Core files
#

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/NS)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Epetra)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Tpetra)
IF (ML_ENABLE_TekoSmoothers)
   INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/mlutils)
ENDIF()

APPEND_GLOB(HEADERS
  *.hpp
  NS/*.hpp
  Epetra/*.hpp
  Tpetra/*.hpp
  )

APPEND_GLOB(SOURCES
  *.cpp
  NS/*.cpp
  Epetra/*.cpp
  Tpetra/*.cpp
  )

IF (ML_ENABLE_TekoSmoothers)
   APPEND_GLOB(HEADERS mlutils/*.hpp)
   APPEND_GLOB(SOURCES mlutils/*.cpp)
ENDIF()

######################################

#
# C) Define the targets for package's library(s)
#

TRIBITS_ADD_LIBRARY(
  teko 
  HEADERS ${HEADERS}
  SOURCES ${SOURCES}
  )
