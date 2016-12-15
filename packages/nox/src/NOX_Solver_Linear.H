// $Id$
// $Source$

//@HEADER
// ************************************************************************
//
//            NOX: An Object-Oriented Nonlinear Solver Package
//                 Copyright (2002) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Roger Pawlowski (rppawlo@sandia.gov) or
// Eric Phipps (etphipp@sandia.gov), Sandia National Laboratories.
// ************************************************************************
//  CVS Information
//  $Source$
//  $Author$
//  $Date$
//  $Revision$
// ************************************************************************
//@HEADER

#ifndef NOX_SOLVER_LINEAR_H
#define NOX_SOLVER_LINEAR_H

#include "NOX_Solver_Generic.H"  // for NOX::StatusTest::StatusType

namespace NOX {
namespace Solver {

/*!
  \brief Light wrapper over linear solver, executes exactly one iteration.

  This class exists to support applications which always instantiate a NOX
  Nonlinear solver via the NOX::Solver::Factory, but may be used to
  solve known linear problems.
  In that case, it is convenient to create a NOX::Solver object which requires
  no status tests and simply executes the linear solver.
 */
class Linear {

public:

  //! Constructor
  Linear();

  //! Destructor
  virtual ~Linear();

  //! Resets the solver and sets a new initial guess.
  virtual void reset(const NOX::Abstract::Vector& initial_guess);

  //! Resets the solver, sets a new status test, and sets a new initial guess.
  virtual void reset(const NOX::Abstract::Vector& initial_guess,
             const Teuchos::RCP<NOX::StatusTest::Generic>& test) = 0;

  //! Check current convergence and failure status.
  virtual NOX::StatusTest::StatusType getStatus() = 0;

  //! Do one nonlinear step in the iteration sequence and return status.
  virtual NOX::StatusTest::StatusType step() = 0;

  /*!
    \brief Solve the nonlinear problem and return final status.

    By "solve", we call iterate() until the NOX::StatusTest value is
    either NOX::StatusTest::Converged or NOX::StatusTest::Failed.
  */
  virtual NOX::StatusTest::StatusType solve() = 0;

  //! Return a reference to the current solution group.
  virtual const NOX::Abstract::Group& getSolutionGroup() const = 0;

  //! Return a reference to the previous solution group.
  virtual const NOX::Abstract::Group& getPreviousSolutionGroup() const = 0;

  //! Get number of iterations.
  virtual int getNumIterations() const = 0;

  //! Return a reference to the solver parameters.
  virtual const Teuchos::ParameterList& getList() const = 0;

  //@{ \name PyTrilinos Interface functions (require RCPs)

  //! Return a RCP to the solution group
  virtual Teuchos::RCP< const NOX::Abstract::Group > getSolutionGroupPtr() const
  {
     TEUCHOS_TEST_FOR_EXCEPTION(true,std::logic_error,
            "NOX::Generic::Solver::getSolutionGroupPtr() not implemented!");
     return Teuchos::null;
  }

  //! Return a RCP to the previous solution group
  virtual Teuchos::RCP< const NOX::Abstract::Group > getPreviousSolutionGroupPtr() const
  {
    TEUCHOS_TEST_FOR_EXCEPTION(true,std::logic_error,
               "NOX::Generic::Solver::getPreviousSolutionGroupPtr() not implemented!");
     return Teuchos::null;
  }

  //! Return a RCP to the solver parameters.
  virtual Teuchos::RCP< const Teuchos::ParameterList > getListPtr() const
  {
    TEUCHOS_TEST_FOR_EXCEPTION(true,std::logic_error,
               "NOX::Generic::Solver::getListPtr() not implemented!");
     return Teuchos::null;
  }

  //@}

};
} // namespace Solver
} // namespace NOX


#endif

