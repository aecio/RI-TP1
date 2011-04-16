# CmakeLists in test dir

INCLUDE_DIRECTORIES(${TP1_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${TP1_SOURCE_DIR}/riCode)

ADD_EXECUTABLE(testSuite TestSuite)

# Link the executable to the RICPlib library
TARGET_LINK_LIBRARIES(testSuite z htmlcxx RICPlib TP1)
