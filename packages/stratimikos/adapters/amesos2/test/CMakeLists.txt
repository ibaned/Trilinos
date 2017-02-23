
TRIBITS_INCLUDE_DIRECTORIES(REQUIRED_DURING_INSTALLATION_TESTING ${CMAKE_CURRENT_BINARY_DIR}/../../src)

TRIBITS_ADD_EXECUTABLE(
  test_single_amesos2_tpetra_solver_driver
  SOURCES
    test_single_amesos2_tpetra_solver_driver.cpp
    test_single_amesos2_tpetra_solver.cpp
  COMM serial mpi
  )

IF(Amesos2_ENABLE_SuperLU)
  TRIBITS_ADD_TEST(
    test_single_amesos2_tpetra_solver_driver
    POSTFIX_AND_ARGS_0 A
    --show-all-tests --solver-type=Superlu --verbose --matrix-file=A.mm
    COMM serial mpi
    NUM_MPI_PROCS 1
    )
ENDIF()

IF(Amesos2_ENABLE_SuperLUMT)
  TRIBITS_ADD_TEST(
    test_single_amesos2_tpetra_solver_driver
    POSTFIX_AND_ARGS_0 A
    --show-all-tests --solver-type=Superlumt --verbose --matrix-file=A.mm
    COMM serial mpi
    NUM_MPI_PROCS 1
    )
ENDIF()

IF(Amesos2_ENABLE_SuperLUDist)
  TRIBITS_ADD_TEST(
    test_single_amesos2_tpetra_solver_driver
    POSTFIX_AND_ARGS_0 A
    --show-all-tests --solver-type=Superludist --verbose --matrix-file=A.mm
    COMM serial mpi
    NUM_MPI_PROCS 1
    )
ENDIF()

IF(Amesos2_ENABLE_PARDISO_MKL)
  TRIBITS_ADD_TEST(
    test_single_amesos2_tpetra_solver_driver
    POSTFIX_AND_ARGS_0 A
    --show-all-tests --solver-type=Pardiso_mkl --verbose --matrix-file=A.mm
    COMM serial mpi
    NUM_MPI_PROCS 1
    )
ENDIF()

IF(Amesos2_ENABLE_Cholmod)
  TRIBITS_ADD_TEST(
    test_single_amesos2_tpetra_solver_driver
    POSTFIX_AND_ARGS_0 A
    --show-all-tests --solver-type=Cholmod --verbose --matrix-file=A.mm
    COMM serial mpi
    NUM_MPI_PROCS 1
    )
ENDIF()

IF(Amesos2_ENABLE_MUMPS)
  TRIBITS_ADD_TEST(
    test_single_amesos2_tpetra_solver_driver
    POSTFIX_AND_ARGS_0 A
    --show-all-tests --solver-type=Mumps --verbose --matrix-file=A.mm
    COMM serial mpi
    NUM_MPI_PROCS 1
    )
ENDIF()

TRIBITS_COPY_FILES_TO_BINARY_DIR(CopyAmesos2TestFiles
  DEST_FILES
    A.mm                          
  SOURCE_DIR
    "${PACKAGE_SOURCE_DIR}/../ml/examples/BasicExamples"
  EXEDEPS
    test_single_amesos2_tpetra_solver_driver
  )
