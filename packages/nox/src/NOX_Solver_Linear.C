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

#include "NOX_Solver_Linear.H"    // class definition

NOX::Solver::Linear::
Linear(const Teuchos::RCP<NOX::Abstract::Group>& xGrp,
       const Teuchos::RCP<Teuchos::ParameterList>& p) :
  globalDataPtr(Teuchos::rcp(new NOX::GlobalData(p))),
  utilsPtr(globalDataPtr->getUtils()),
  solnPtr(xGrp),                         // pointer to xGrp
  oldSolnPtr(xGrp->clone(DeepCopy))      // create via clone
  paramsPtr(p),
  prePostOperator(utilsPtr, paramsPtr->sublist("Solver Options"))
{
  (void) t;
  init();
}

// Protected
void NOX::Solver::Linear::init()
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

void NOX::Solver::Linear::
reset(const NOX::Abstract::Vector& initialGuess)
{
  solnPtr->setX(initialGuess);
  init();
}

void NOX::Solver::Linear::
reset(const NOX::Abstract::Vector& initialGuess,
      const Teuchos::RCP<NOX::StatusTest::Generic>& t)
{
  solnPtr->setX(initialGuess);
  testPtr = t;
  init();
}

NOX::Solver::Linear::~Linear()
{

}

NOX::StatusTest::StatusType NOX::Solver::Linear::getStatus()
{
  return status;
}

NOX::StatusTest::StatusType NOX::Solver::Linear::step()
{
  prePostOperator.runPreIterate(*this);

  // Copy pointers into temporary references
  NOX::Abstract::Group& soln = *solnPtr;

  // Update iteration count.
  nIter ++;

  // Copy current soln to the old soln.
  *oldSolnPtr = *solnPtr;

  NOX::Abstract::Group::ReturnType rtype = soln.computeF();
  if (rtype != NOX::Abstract::Group::Ok)
  {
    utilsPtr->out() << "NOX::Solver::Linear::iterate - unable to compute F" << std::endl;
    status = NOX::StatusTest::Failed;
  }
  else
    status = NOX::StatusTest::Converged;

  prePostOperator.runPostIterate(*this);

  printUpdate();

  return status;
}

NOX::StatusTest::StatusType NOX::Solver::Linear::solve()
{
  prePostOperator.runPreSolve(*this);

  step();

  prePostOperator.runPostSolve(*this);

  return status;
}

const NOX::Abstract::Group&
NOX::Solver::Linear::getSolutionGroup() const
{
  return *solnPtr;
}

const NOX::Abstract::Group&
NOX::Solver::Linear::getPreviousSolutionGroup() const
{
  return *oldSolnPtr;
}

int NOX::Solver::Linear::getNumIterations() const
{
  return nIter;
}

const Teuchos::ParameterList&
NOX::Solver::Linear::getList() const
{
  return *paramsPtr;
}

virtual Teuchos::RCP< const Teuchos::ParameterList >
NOX::Solver::Linear::getListPtr() const
{
   return paramsPtr;
}

// protected
void NOX::Solver::Linear::printUpdate()
{
  if (utilsPtr->isPrintType(NOX::Utils::OuterIteration))
  {
    normSoln = solnPtr->getNormF();
    utilsPtr->out() << "\n" << NOX::Utils::fill(72) << "\n";
    utilsPtr->out() << "-- The \"Nonlinear\" Solver Step -- \n";
    utilsPtr->out() << "||F|| = " << utilsPtr->sciformat(normSoln);
    if (status == NOX::StatusTest::Converged)
      utilsPtr->out() << " (Converged!)";
    if (status == NOX::StatusTest::Failed)
      utilsPtr->out() << " (Failed!)";
    utilsPtr->out() << "\n" << NOX::Utils::fill(72) << "\n" << std::endl;
  }
}
