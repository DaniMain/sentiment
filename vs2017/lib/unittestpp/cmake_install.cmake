# Install script for directory: C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Aquila")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/User/Documents/sentiment_per_tirocinio/sentiment/vs2017/lib/unittestpp/Debug/UnitTest++.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/User/Documents/sentiment_per_tirocinio/sentiment/vs2017/lib/unittestpp/Release/UnitTest++.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/User/Documents/sentiment_per_tirocinio/sentiment/vs2017/lib/unittestpp/MinSizeRel/UnitTest++.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/User/Documents/sentiment_per_tirocinio/sentiment/vs2017/lib/unittestpp/RelWithDebInfo/UnitTest++.lib")
  endif()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/UnitTest++" TYPE FILE FILES
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/AssertException.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/CheckMacros.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/Checks.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/CompositeTestReporter.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/Config.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/CurrentTest.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/DeferredTestReporter.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/DeferredTestResult.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/ExceptionMacros.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/ExecuteTest.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/HelperMacros.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/MemoryOutStream.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/ReportAssert.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/ReportAssertImpl.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/Test.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TestDetails.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TestList.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TestMacros.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TestReporter.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TestReporterStdout.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TestResults.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TestRunner.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TestSuite.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TimeConstraint.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/TimeHelpers.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/UnitTest++.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/UnitTestPP.h"
    "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/XmlTestReporter.h"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/UnitTest++/Win32" TYPE FILE FILES "C:/Users/User/Documents/aquila/aquila-src/lib/unittestpp/UnitTest++/Win32/TimeHelpers.h")
endif()

