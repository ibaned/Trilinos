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

#ifndef PANZER_EXPR_EVAL_HPP
#define PANZER_EXPR_EVAL_HPP

#include <functional>

#include <Teuchos_Reader.hpp>

#include <Kokkos_Core.hpp>

namespace panzer
{
namespace Expr
{

enum class BinaryOpCode {
  OR,
  AND,
  GT,
  LT,
  GEQ,
  LEQ,
  EQ,
  ADD,
  SUB,
  PROD,
  DIV,
};

class EvalBase : public Teuchos::Reader {
 public:
  EvalBase();
 protected:
  void at_shift(Teuchos::any& result, int token, std::string& text) override;
  void at_reduce(Teuchos::any& result, int prod, std::vector<Teuchos::any>& rhs) override;
 public:
  using Function = std::function<void(Teuchos::any&, std::vector<Teuchos::any>& rhs)>;
  std::map<std::string, Teuchos::any> symbol_map;
 protected:
  void ternary_op(Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right);
  void binary_op(Teuchos::any& result, Teuchos::any& left, Teuchos::any& right);
  void neg_op(Teuchos::any& result, Teuchos::any& right);
 protected:
  virtual void inspect_arg(Teuchos::any const& arg, bool& is_view, bool& is_bool) = 0;
  virtual void single_single_ternary_op(Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void single_view_ternary_op(Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void view_single_ternary_op(Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void view_view_ternary_op(Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void single_single_binary_op(BinaryOpCode code, Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void single_view_binary_op(BinaryOpCode code, Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void view_single_binary_op(BinaryOpCode code, Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void view_view_binary_op(BinaryOpCode code, Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void single_single_binary_op_bool(BinaryOpCode code, Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void single_view_binary_op_bool(BinaryOpCode code, Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void view_view_binary_op_bool(BinaryOpCode code, Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) = 0;
  virtual void view_neg_op(Teuchos::any& result, Teuchos::any& right) = 0;
  virtual void single_neg_op(Teuchos::any& result, Teuchos::any& right) = 0;
};

template <typename ViewType>
class Eval : public EvalBase {
 public:
  Eval();
};

}} // end namespace panzer::Expr

#endif // PANZER_EXPR_EVAL_HPP
