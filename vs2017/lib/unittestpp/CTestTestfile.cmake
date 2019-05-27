# CMake generated Testfile for 
# Source directory: C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp
# Build directory: C:/Users/User/Documents/sentiment_per_tirocinio/sentiment/vs2017/lib/unittestpp
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(TestUnitTest++ "C:/Users/User/Documents/sentiment_per_tirocinio/sentiment/vs2017/lib/unittestpp/Debug/TestUnitTest++.exe")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(TestUnitTest++ "C:/Users/User/Documents/sentiment_per_tirocinio/sentiment/vs2017/lib/unittestpp/Release/TestUnitTest++.exe")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(TestUnitTest++ "C:/Users/User/Documents/sentiment_per_tirocinio/sentiment/vs2017/lib/unittestpp/MinSizeRel/TestUnitTest++.exe")
elseif("${CTEST_CONFIGURATION_TYPE}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(TestUnitTest++ "C:/Users/User/Documents/sentiment_per_tirocinio/sentiment/vs2017/lib/unittestpp/RelWithDebInfo/TestUnitTest++.exe")
else()
  add_test(TestUnitTest++ NOT_AVAILABLE)
endif()
