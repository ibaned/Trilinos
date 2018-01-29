// @HEADER
// ***********************************************************************
//
//           Panzer: A partial differential equation assembly
//       engine for strongly coupled complex multiphysics systems
//                 Copyright (2011) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
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
// Questions? Contact Roger P. Pawlowski (rppawlo@sandia.gov) and
// Eric C. Cyr (eccyr@sandia.gov)
// ***********************************************************************
// @HEADER

#include <Teuchos_UnitTestHarness.hpp>

#include "Panzer_ExprEval.hpp"

namespace panzer {

TEUCHOS_UNIT_TEST(ExprEval, test_1_plus_1)
{
  Expr::Eval<double*> eval;
  Teuchos::any result;
  eval.read_string(result, "1+1", "one plus one");
  Kokkos::View<double const> x = Teuchos::any_cast<Kokkos::View<double const>>(result);
  TEUCHOS_ASSERT(x() == 2.0);
  eval.set("a", 5.0);
  eval.read_string(result, "a+a", "a plus a");
  x = Teuchos::any_cast<Kokkos::View<double const>>(result);
  TEUCHOS_ASSERT(x() == 10.0);
  eval.read_string(result, "b=4;\na+b", "a plus b");
  x = Teuchos::any_cast<Kokkos::View<double const>>(result);
  TEUCHOS_ASSERT(x() == 9.0);
  eval.read_string(result, "a-b", "a minus b");
  x = Teuchos::any_cast<Kokkos::View<double const>>(result);
  TEUCHOS_ASSERT(x() == 1.0);
  eval.read_string(result, "a^2 - b^2", "a^2 - b^2");
  x = Teuchos::any_cast<Kokkos::View<double const>>(result);
  TEUCHOS_ASSERT(x() == 9.0);
  auto y = Kokkos::View<double*>{"y", 3};
  y(0) = 1.0;
  y(1) = 2.0;
  y(2) = 3.0;
  eval.set("y", y);
  eval.read_string(result, "2 + y", "2 + y");
  auto z = Teuchos::any_cast<Kokkos::View<double const*>>(result);
  TEUCHOS_ASSERT(z(0) == 3.0);
  TEUCHOS_ASSERT(z(1) == 4.0);
  TEUCHOS_ASSERT(z(2) == 5.0);
  eval.read_string(result, "y * a", "y * a");
  z = Teuchos::any_cast<Kokkos::View<double const*>>(result);
  TEUCHOS_ASSERT(z(0) == 5.0);
  TEUCHOS_ASSERT(z(1) == 10.0);
  TEUCHOS_ASSERT(z(2) == 15.0);
  eval.read_string(result, "y + y^2", "y + y^2");
  z = Teuchos::any_cast<Kokkos::View<double const*>>(result);
  TEUCHOS_ASSERT(z(0) == 2.0);
  TEUCHOS_ASSERT(z(1) == 6.0);
  TEUCHOS_ASSERT(z(2) == 12.0);
  eval.read_string(result, "0.5 < 1.0 ? 0.5 : 1.0", "0.5 < 1.0 ? 0.5 : 1.0");
  x = Teuchos::any_cast<Kokkos::View<double const>>(result);
  TEUCHOS_ASSERT(x() == 0.5);
  eval.read_string(result, "y < 1.5 ? 0.0 : 1.0", "y < 1.5 ? 0.0 : 1.0");
  z = Teuchos::any_cast<Kokkos::View<double const*>>(result);
  TEUCHOS_ASSERT(z(0) == 0.0);
  TEUCHOS_ASSERT(z(1) == 1.0);
  TEUCHOS_ASSERT(z(2) == 1.0);
  eval.read_string(result, "(-y) + (-a)", "(-y) + (-a)");
  z = Teuchos::any_cast<Kokkos::View<double const*>>(result);
  TEUCHOS_ASSERT(z(0) == -6.0);
  TEUCHOS_ASSERT(z(1) == -7.0);
  TEUCHOS_ASSERT(z(2) == -8.0);
}

}
