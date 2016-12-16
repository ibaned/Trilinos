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

#include "NOX_Solver_OneStep.H"    // class definition
#include "NOX_GlobalData.H"    // class definition
#include "NOX_Abstract_Group.H"    // class definition
#include "NOX_Abstract_Group.H"    // class definition
#include "Teuchos_ParameterList.hpp"  // class data element

NOX::Solver::OneStep::
OneStep(const Teuchos::RCP<NOX::Abstract::Group>& xGrp,
       const Teuchos::RCP<Teuchos::ParameterList>& p) :
  globalDataPtr(Teuchos::rcp(new NOX::GlobalData(p))),
  utilsPtr(globalDataPtr->getUtils()),
  solnPtr(xGrp),                         // pointer to xGrp
  oldSolnPtr(xGrp->clone(DeepCopy)),     // create via clone
  paramsPtr(p),
  prePostOperator(utilsPtr, paramsPtr->sublist("Solver Options"))
{
  init();
}

// Protected
void NOX::Solver::OneStep::init()
{
  // Initialize
  nIter = 0;
  status = NOX::StatusTest::Unconverged;

  // Print out parameters
  if (utilsPtr->isPrintType(NOX::Utils::Parameters))
  {
    utilsPtr->out() << "\n" << NOX::Utils::fill(72) << "\n";
    utilsPtr->out() << "\n-- Parameters Passed to Nonlinear Solver --\n\n";
    paramsPtr->print(utilsPtr->out(),5);
  }

}

void NOX::Solver::OneStep::
reset(const NOX::Abstract::Vector& initialGuess)
{
  solnPtr->setX(initialGuess);
  init();
}

void NOX::Solver::OneStep::
reset(const NOX::Abstract::Vector& initialGuess,
      const Teuchos::RCP<NOX::StatusTest::Generic>&)
{
  solnPtr->setX(initialGuess);
  init();
}

NOX::Solver::OneStep::~OneStep()
{

}

NOX::StatusTest::StatusType NOX::Solver::OneStep::getStatus()
{
  return status;
}

bool NOX::Solver::OneStep::check(NOX::Abstract::Group::ReturnType ret,
    const std::string& task)
{
  if (ret != NOX::Abstract::Group::Ok) {
    if (utils->isPrintType(Utils::Error))
      utilsPtr->out() << "NOX::Solver::OneStep - Unable to " << task << std::endl;
    return false;
  }
  return true;
}

bool NOX::Solver::OneStep::try_step()
{
  if (!check(soln.computeF(), "compute F"))
    return false;
  if (!check(soln.computeJacobian(), "compute Jacobian"))
    return false;
  if (!check(soln.computeNewton(paramsPtr->sublist("Linear Solver")),
             "solve Newton system"))
    return false;
  NOX::Abstract::Vector& dir = soln.getNewton();
  soln.computeX(*oldSolnPtr, dir, 1.0);
  return true;
}

NOX::StatusTest::StatusType NOX::Solver::OneStep::step()
{
  prePostOperator.runPreIterate(*this);

  // Copy pointers into temporary references
  NOX::Abstract::Group& soln = *solnPtr;

  // Update iteration count.
  nIter ++;

  // Copy current soln to the old soln.
  *oldSolnPtr = *solnPtr;

  if (try_step())
    status = NOX::StatusTest::Converged;
  else
    status = NOX::StatusTest::Failed;

  prePostOperator.runPostIterate(*this);

  printUpdate();

  return status;
}

NOX::StatusTest::StatusType NOX::Solver::OneStep::solve()
{
  prePostOperator.runPreSolve(*this);

  step();

  prePostOperator.runPostSolve(*this);

  return status;
}

const NOX::Abstract::Group&
NOX::Solver::OneStep::getSolutionGroup() const
{
  return *solnPtr;
}

Teuchos::RCP< const NOX::Abstract::Group >
NOX::Solver::OneStep::getSolutionGroupPtr() const
{
  return solnPtr;
}

const NOX::Abstract::Group&
NOX::Solver::OneStep::getPreviousSolutionGroup() const
{
  return *oldSolnPtr;
}

Teuchos::RCP< const NOX::Abstract::Group >
NOX::Solver::OneStep::getPreviousSolutionGroupPtr() const
{
  return oldSolnPtr;
}

int NOX::Solver::OneStep::getNumIterations() const
{
  return nIter;
}

const Teuchos::ParameterList&
NOX::Solver::OneStep::getList() const
{
  return *paramsPtr;
}

Teuchos::RCP< const Teuchos::ParameterList >
NOX::Solver::OneStep::getListPtr() const
{
   return paramsPtr;
}

// protected
void NOX::Solver::OneStep::printUpdate()
{
  if (utilsPtr->isPrintType(NOX::Utils::OuterIteration))
  {
    utilsPtr->out() << "\n" << NOX::Utils::fill(72) << "\n";
    utilsPtr->out() << "-- The \"Nonlinear\" Solver Step -- \n";
    if (status == NOX::StatusTest::Converged)
      utilsPtr->out() << " (Converged!)";
    if (status == NOX::StatusTest::Failed)
      utilsPtr->out() << " (Failed!)";
    utilsPtr->out() << "\n" << NOX::Utils::fill(72) << "\n" << std::endl;
  }
}
