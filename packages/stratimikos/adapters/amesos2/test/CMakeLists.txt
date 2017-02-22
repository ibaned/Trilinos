
TRIBITS_INCLUDE_DIRECTORIES(REQUIRED_DURING_INSTALLATION_TESTING ${CMAKE_CURRENT_BINARY_DIR}/../../src)

TRIBITS_ADD_EXECUTABLE(
  test_single_amesos2_tpetra_solver_driver
  SOURCES
    test_single_amesos2_tpetra_solver_driver.cpp
    test_single_amesos2_tpetra_solver.cpp
  COMM serial mpi
  )

TRIBITS_ADD_TEST(
  test_single_amesos2_tpetra_solver_driver
  POSTFIX_AND_ARGS_0 FourByFour
    --show-all-tests  --matrix-file=FourByFour.mtx
  COMM serial mpi
  NUM_MPI_PROCS 1
  )

TRIBITS_COPY_FILES_TO_BINARY_DIR(CopyBelosTestFiles
  DEST_FILES
    FourByFour.mtx                          
  SOURCE_PREFIX
    "In_"
  SOURCE_DIR
    "${PACKAGE_SOURCE_DIR}/../amesos/test/Test_Basic"
  EXEDEPS
    test_single_amesos2_tpetra_solver_driver
  )
