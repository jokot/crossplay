add_executable(TlsHeadroomTest TlsHeadroomTest.cpp)

target_include_directories(TlsHeadroomTest PRIVATE
  ${REPO_ROOT}/src
)

target_link_libraries(TlsHeadroomTest PRIVATE
  crosspoint_test_common
  GTest::gtest_main
)

gtest_discover_tests(TlsHeadroomTest)
