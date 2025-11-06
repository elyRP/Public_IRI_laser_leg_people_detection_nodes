# application source files
SET(sources laserPeopleCommon.cpp laserPeopleDetection.cpp laserPeopleLabel.cpp)
SET(sources_boost boostsrc/BoostedCommittee.cpp boostsrc/SPHypothesis.cpp)

# application header files
SET(headers ../include/laserPeopleCommon.h ../include/laserPeopleDetection.h ../include/laserPeopleLabel.h)
SET(headers_boost boostsrc/BoostedCommittee.h boostsrc/SPHypothesis.h boostsrc/IWeakHypothesis.h)

# application data files
SET(data_boost ../data/classifierData/boostFile.txt ../data/classifierData/boostFile2.txt)

# locate the necessary dependencies

# add the necessary include directories
INCLUDE_DIRECTORIES(../include)
INCLUDE_DIRECTORIES(./boostsrc)

# create the shared library

ADD_LIBRARY(laser_people_detection SHARED ${sources} ${sources_boost})
# link necessary libraries
INSTALL(TARGETS laser_people_detection
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib/iri/${PROJECT_NAME}
        ARCHIVE DESTINATION lib/iri/${PROJECT_NAME})

INSTALL(FILES ${headers} ${headers_boost} DESTINATION include/iri/${PROJECT_NAME})
INSTALL(FILES ${data_boost}  DESTINATION include/lpd/boostData)

INSTALL(FILES ../Findlaser_people_detection.cmake DESTINATION ${CMAKE_ROOT}/Modules/)

#ADD_SUBDIRECTORY(examples)
