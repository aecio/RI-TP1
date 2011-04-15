# CmakeLists in test dir

# Make sure the compiler can find include from our RICPlib library
INCLUDE_DIRECTORIES(${TP1_SOURCE_DIR}/src)

# Add binary called "ircpReader" that is built from the source file "test.cpp"
ADD_EXECUTABLE(test test)

