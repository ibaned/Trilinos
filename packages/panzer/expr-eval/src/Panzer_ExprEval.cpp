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

#include <Panzer_ExprEval.hpp>

#include <cstdlib>

#include <Teuchos_MathExpr.hpp>

namespace panzer
{

ExprEvalBase::ExprEvalBase()
  : Teuchos::Reader(Teuchos::MathExpr::ask_reader_tables()) {
}

void ExprEvalBase::at_shift(Teuchos::any& result, int token, std::string& text) {
  using std::swap;
  switch (token) {
    case Teuchos::MathExpr::TOK_NAME: {
      std::string& result = Teuchos::make_any_ref<std::string>(result_any);
      swap(result, text);
      return;
    }
    case Teuchos::MathExpr::TOK_CONST: {
      result_any = std::atof(text.c_str());
      return;
    }
  }
}

virtual void at_reduce(Teuchos::any& result, int prod, std::vector<Teuchos::any>& rhs) {
  using std::swap;
  switch (prod) {
    case Teuchos::MathExpr::PROD_PROGRAM: {
      swap(result, rhs.at(1));
      break;
    }
    case Teuchos::MathExpr::PROD_NO_STATEMENTS:
    case Teuchos::MathExpr::PROD_NO_EXPR:
    case Teuchos::MathExpr::PROD_NEXT_STATEMENT: {
      break;
    }
    case Teuchos::MathExpr::PROD_ASSIGN: {
      std::string const& name = Teuchos::any_ref_cast<std::string>(rhs.at(0));
      swap(symbols[name], rhs.at(4));
      break;
    }
    case Teuchos::MathExpr::PROD_YES_EXPR:
    case Teuchos::MathExpr::PROD_EXPR:
    case Teuchos::MathExpr::PROD_TERNARY_DECAY:
    case Teuchos::MathExpr::PROD_OR_DECAY:
    case Teuchos::MathExpr::PROD_AND_DECAY:
    case Teuchos::MathExpr::PROD_ADD_SUB_DECAY:
    case Teuchos::MathExpr::PROD_MUL_DIV_DECAY:
    case Teuchos::MathExpr::PROD_POW_DECAY:
    case Teuchos::MathExpr::PROD_NEG_DECAY:
    case Teuchos::MathExpr::PROD_SOME_ARGS:
      swap(result, rhs.at(0));
      break;
    case Teuchos::MathExpr::PROD_TERNARY:
      this->ternary_op(result, rhs.at(0), rhs.at(3), rhs.at(6));
      break;
    case Teuchos::MathExpr::PROD_OR:
      this->or_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_AND:
      this->and_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_GT:
      this->gt_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_LT:
      this->lt_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_GEQ:
      this->geq_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_LEQ:
      this->leq_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_EQ:
      this->eq_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_BOOL_PARENS:
      swap(result, rhs.at(2));
      break;
    case Teuchos::MathExpr::PROD_ADD:
      this->add_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_SUB:
      this->sub_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_MUL:
      this->prod_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_DIV:
      this->div_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_POW:
      this->pow_op(result, rhs.at(0), rhs.at(3));
      break;
    case Teuchos::MathExpr::PROD_CALL: {
      std::string const& name = Teuchos::any_ref_cast<std::string>(rhs.at(0));
      auto it = symbols.find(name);
      TEUCHOS_TEST_FOR_EXCEPTION(it == symbols.end(), Teuchos::ParserFail,
          "symbol \"" << name << "\" being called doesn't exist!");
      Function& func = Teuchos::any_ref_cast<Function>(it->second);
      std::vector<Teuchos::any>& args = Teuchos::any_ref_cast<std::vector<Teuchos::any>>>(rhs.at(4));
      func(result, args);
      break;
    }
    case Teuchos::MathExpr::PROD_NO_ARGS: {
      result = std::vector<Teuchos::any>{};
      break;
    }
    case Teuchos::MathExpr::PROD_FIRST_ARG: {
      std::vector<Teuchos::any>& args = Teuchos::make_any_ref<std::vector<Teuchos::any>>(result);
      args.push_back();
      swap(args.back(), rhs.at(0));
      break;
    }
    case Teuchos::MathExpr::PROD_NEXT_ARG: {
      swap(result, rhs.at(0));
      std::vector<Teuchos::any>& args = Teuchos::any_ref_cast<std::vector<Teuchos::any>>(result);
      args.push_back();
      swap(args.back(), rhs.at(3));
      break;
    }
    case Teuchos::MathExpr::PROD_NEG:
      this->neg_op(result, rhs.at(2));
      break;
    case Teuchos::MathExpr::PROD_VAL_PARENS:
      swap(result, rhs.at(2));
      break;
    case Teuchos::MathExpr::PROD_CONST:
      swap(result, rhs.at(0));
      break;
    case Teuchos::MathExpr::PROD_VAR:
      std::string const& name = Teuchos::any_ref_cast<std::string>(rhs.at(0));
      auto it = symbols.find(name);
      TEUCHOS_TEST_FOR_EXCEPTION(it == symbols.end(), Teuchos::ParserFail,
          "symbol " << name << " being referenced doesn't exist!");
      swap(result, it->second);
      break;
  }
}

void Eval::ternary_op(Teuchos::any& result, Teuchos::any& cond, Teuchos::any& left, Teuchos::any& right) {
  bool cond_is_view;
  bool cond_is_bool;
  this->inspect_arg(cond, cond_is_view, cond_is_bool);
  TEUCHOS_TEST_FOR_EXCEPTION(!cond_is_bool, Teuchos::ParserFail,
      "Ternary condition is not of boolean type!");
  bool is_view[2];
  bool is_bool[2];
  this->inspect_arg(left, is_view[0], is_bool[0]);
  this->inspect_arg(right, is_view[1], is_bool[1]);
  TEUCHOS_TEST_FOR_EXCEPTION(is_bool[0], Teuchos::ParserFail,
      "Boolean values in ternary operator not yet supported");
  if (!cond_is_view) {
    bool cond_value = Teuchos::any_cast<bool>(cond);
    if (cond_value) {
      swap(result, left);
    } else {
      swap(result, right);
    }
  } else {
    if (!is_view[0] && !is_view[1]) {
      this->single_single_ternary_op(result, cond, left, right);
    } else if (!is_view[0] && is_view[1]) {
      this->single_view_ternary_op(result, cond, left, right);
    } else if (is_view[0] && !is_view[1]) {
      this->view_single_ternary_op(result, cond, left, right);
    } else if (is_view[0] && is_view[1]) {
      this->view_view_ternary_op(result, cond, left, right);
    }
  }
}

static const char* get_op_syntax(BinaryOpCode code) {
  switch (code) {
    case OR: return "||";
    case AND: return "&&";
    case GT: return ">";
    case LT: return "<";
    case GEQ: return ">=";
    case LEQ: return "<=";
    case EQ: return "==";
    case ADD: return "+";
    case SUB: return "-";
    case PROD: return "*";
    case DIV: return "/";
  }
}

void Eval::binary_op(BinaryOpCode code, Teuchos::any& result, Teuchos::any& left, Teuchos::any& right) {
  bool is_view[2];
  bool is_bool[2];
  this->inspect_arg(left, is_view[0], is_bool[0]);
  this->inspect_arg(right, is_view[1], is_bool[1]);
  bool expect_booleans = (code == AND || code == OR);
  TEUCHOS_TEST_FOR_EXCEPTION(is_bool[0] != expect_booleans, Teuchos::ParserFail,
      "Left argument to '" << get_op_syntax(code) << "' is " << (is_bool[0] ? "" : "not") << " boolean!");
  TEUCHOS_TEST_FOR_EXCEPTION(is_bool[1] != expect_booleans, Teuchos::ParserFail,
      "Right argument to '" << get_op_syntax(code) << "' is " << (is_bool[0] ? "" : "not") << " boolean!");
  if (expect_booleans) {
    if (!is_view[0] && !is_view[1]) {
      this->single_single_binary_op_bool(code, result, left, right);
    } else if (!is_view[0] && is_view[1]) {
      this->single_view_binary_op_bool(code, result, left, right);
    } else if (is_view[0] && !is_view[1]) {
      /* throwing in the assumption that all boolean binary operators are commutative */
      this->single_view_binary_op_bool(code, result, left, right);
    } else if (is_view[0] && is_view[1]) {
      this->view_view_binary_op_bool(code, result, left, right);
    }
  } else {
    if (!is_view[0] && !is_view[1]) {
      this->single_single_binary_op(code, result, left, right);
    } else if (!is_view[0] && is_view[1]) {
      this->single_view_binary_op(code, result, left, right);
    } else if (is_view[0] && !is_view[1]) {
      this->view_single_binary_op(code, result, left, right);
    } else if (is_view[0] && is_view[1]) {
      this->view_view_binary_op(code, result, left, right);
    }
  }
}

void neg_op(Teuchos::any& result, Teuchos::any& right) {
  bool is_view;
  bool is_bool;
  this->inspect_arg(left, is_view, is_bool);
  TEUCHOS_TEST_FOR_EXCEPTION(is_bool, Teuchos::ParserFail,
      "Can't negate a boolean");
  if (is_view) {
    this->view_neg_op(result, right);
  } else {
    this->single_neg_op(result, right);
  }
}

} // end namespace panzer
