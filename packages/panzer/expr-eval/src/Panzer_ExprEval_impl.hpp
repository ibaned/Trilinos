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

#ifndef PANZER_EXPR_EVAL_IMPL_HPP
#define PANZER_EXPR_EVAL_IMPL_HPP

#include <Panzer_ExprEval.hpp>

#include <algorithm>

namespace panzer
{
namespace Expr
{

struct ScalarTernary {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T apply(bool cond, T const& left, T const& right) {
    return cond ? left : right;
  }
};

struct ScalarOr {
  static KOKKOS_FORCEINLINE_FUNCTION
  bool apply(bool left, bool right) {
    return left || right;
  }
};

struct ScalarAnd {
  static KOKKOS_FORCEINLINE_FUNCTION
  bool apply(bool left, bool right) {
    return left && right;
  }
};

struct ScalarGT {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool apply(T const& left, T const& right) {
    return left > right;
  }
};

struct ScalarLT {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool apply(T const& left, T const& right) {
    return left < right;
  }
};

struct ScalarGEQ {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool apply(T const& left, T const& right) {
    return left >= right;
  }
};

struct ScalarLEQ {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool apply(T const& left, T const& right) {
    return left <= right;
  }
};

struct ScalarEQ {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool apply(T const& left, T const& right) {
    return left == right;
  }
};

struct ScalarAdd {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T apply(T const& left, T const& right) {
    return left + right;
  }
};

struct ScalarSub {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T apply(T const& left, T const& right) {
    return left - right;
  }
};

struct ScalarProd {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T apply(T const& left, T const& right) {
    return left * right;
  }
};

struct ScalarDiv {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T apply(T const& left, T const& right) {
    return left / right;
  }
};

struct ScalarPow {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T apply(T const& left, T const& right) {
    using std::pow;
    return pow(left, right);
  }
};

struct ScalarNeg {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T apply(T const& right) {
    return -right;
  }
};

template <typename ViewType>
struct WriteViewType;

template <typename DT, typename ... VP>
struct WriteViewType<Kokkos::View<DT, VP ...>> {
  using type = Kokkos::View<typename RebindDataType<DT, typename Kokkos::View<DT, VP ...>::non_const_value_type>::type, VP ...>;
};

template <typename ViewType>
struct ReadBoolViewType;

template <typename DT, typename ... VP>
struct ReadBoolViewType<Kokkos::View<DT, VP ...>> {
  using type = Kokkos::View<typename RebindDataType<DT, bool const>::type, VP ...>;
};

template <typename ViewType>
struct WriteBoolViewType;

template <typename DT, typename ... VP>
struct WriteBoolViewType<Kokkos::View<DT, VP ...>> {
  using type = Kokkos::View<typename RebindDataType<DT, bool>::type, VP ...>;
};

template <typename Indexed, size_t Rank>
struct Indexer;

template <typename Indexed>
struct Indexer<Indexed, 1> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  Indexed& index(Indexed& x, Integral) { return x; }
};

template <typename DT, typename ... VP>
struct Indexer<Kokkos::View<DT, VP...>, 1> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  typename Kokkos::View<DT, VP...>::reference_type
  index(Kokkos::View<DT, VP...> const& x, Integral i) { return x(i); }
};

template <typename Indexed>
struct Indexer<Indexed, 2> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  Indexed& index(Indexed& x, Integral, Integral) { return x; }
};

template <typename DT, typename ... VP>
struct Indexer<Kokkos::View<DT, VP...>, 2> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  typename Kokkos::View<DT, VP...>::reference_type
  index(Kokkos::View<DT, VP...> const& x, Integral i, Integral j) { return x(i, j); }
};

template <typename T>
struct ExtentsFor {
  static size_t extent(T const&, size_t) { return 1; }
};

template <typename ST, typename ... VP>
struct ExtentsFor<Kokkos::View<ST, VP...>> {
  static size_t extent(Kokkos::View<ST, VP...> const& view, size_t i) { return view.extent(i); }
};

template <typename T>
struct RankFor {
  static constexpr size_t value = 0;
};

template <typename ST, typename ... VP>
struct RankFor<Kokkos::View<ST, VP...>> {
  static constexpr size_t value = Kokkos::View<ST, VP...>::rank;
};

template <typename T, typename ... TS>
struct RankForAll;

template <typename T>
struct RankForAll<T> {
  static constexpr size_t value = RankFor<T>::value;
};

template <typename T, typename ... TS>
struct RankForAll {
  static constexpr size_t left_value = RankFor<T>::value;
  static constexpr size_t right_value = RankForAll<TS ...>::value;
  static constexpr size_t value = left_value > right_value ? left_value : right_value;
};

template <typename T>
struct ExecSpaceFor {
  using type = Kokkos::AnonymousSpace;
};

template <typename ST, typename ... VP>
struct ExecSpaceFor<Kokkos::View<ST, VP...>> {
  using type = typename Kokkos::View<ST, VP...>::execution_space;
};

template <typename A, typename B>
struct CombineTwoExecSpaces;

template <typename T>
struct CombineTwoExecSpaces<T, Kokkos::AnonymousSpace> {
  using type = T;
};

template <typename T>
struct CombineTwoExecSpaces<Kokkos::AnonymousSpace, T> {
  using type = T;
};

template <typename T>
struct CombineTwoExecSpaces<T, T> {
  using type = T;
};

template <typename T, typename ... TS>
struct ExecSpaceForAll;

template <typename T>
struct ExecSpaceForAll<T>;

template <typename T, typename ... TS>
struct ExecSpaceForAll {
  using rest_type = typename ExecSpaceForAll<TS ...>::type;
  using type =
    typename CombineTwoExecSpaces<
        typename ExecSpaceFor<T>::type, rest_type>::type;
};

template <typename T>
struct ExecSpaceForAll<T> {
  using type = typename ExecSpaceFor<T>::type;
};

template <typename Current, typename Default>
struct SetDefaultExecSpace {
  using type = Current;
};

template <typename Default>
struct SetDefaultExecSpace<Kokkos::AnonymousSpace, Default> {
  using type = Default;
};

template <typename A, typename B>
struct ResultType;

template <typename A>
struct ResultType<A, A> {
  using type = A;
};

template <typename B, typename DT, typename ... VP>
struct ResultType<Kokkos::View<DT, VP...>, B> {
  using type = typename WriteViewType<Kokkos::View<DT, VP...>>::type;
};

template <typename A, typename DT, typename ... VP>
struct ResultType<A, Kokkos::View<DT, VP...>> {
  using type = typename WriteViewType<Kokkos::View<DT, VP...>>::type;
};

template <typename DT, typename ... VP>
struct ResultType<Kokkos::View<DT, VP...>, Kokkos::View<DT, VP...>> {
  using type = typename WriteViewType<Kokkos::View<DT, VP...>>::type;
};

template <typename Op, typename Left, typename Right, size_t Rank = RankForAll<Left, Right>::value>
struct BinaryFunctor;

template <typename Op, typename Left, typename Right>
struct BinaryFunctor<Op, Left, Right, 0> {
  using Result = typename ResultType<Left, Right>::type;
  Result result_;
  Left   left_;
  Right  right_;
  BinaryFunctor(std::string const& name, Teuchos::any& result, Teuchos::any& left, Teuchos::any& right) {
    left_ = Teuchos::any_cast<Left>(left);
    right_ = Teuchos::any_cast<Right>(right);
    result_ = Op::apply(left_, right_);
    result = result_;
  }
};

template <typename Op, typename Left, typename Right>
struct BinaryFunctor<Op, Left, Right, 1> {
  using Result = typename ResultType<Left, Right>::type;
  using execution_space =
    typename SetDefaultExecSpace<
      typename ExecSpaceForAll<Left, Right>::type,
      Kokkos::Serial>::type;
  Result result_;
  Left   left_;
  Right  right_;
  KOKKOS_INLINE_FUNCTION
  void operator()(typename execution_space::size_type i) const {
    Indexer<Result, 1>::index(result_, i) =
      Op::apply(
          Indexer<Left, 1>::index(left_, i),
          Indexer<Right, 1>::index(right_, i));
  }
  BinaryFunctor(std::string const& name, Teuchos::any& result, Teuchos::any& left, Teuchos::any& right) {
    left_ = Teuchos::any_cast<Left>(left);
    right_ = Teuchos::any_cast<Right>(right);
    auto extent_0 =
      std::max(
          ExtentsFor<Left>::extent(0),
          ExtentsFor<Right>::extent(0));
    Kokkos::parallel_for(name, Kokkos::RangePolicy<execution_space>(0, extent_0), *this);
    result = result_;
  }
};

template <typename ViewType>
Eval<ViewType>::Eval()
  : EvalBase() 
{
}

template <typename ViewType>
void Eval<ViewType>::set(std::string const& name, scalar_type const& value) {
  symbol_map[name] = value;
}

template <typename ViewType>
void Eval<ViewType>::set(std::string const& name, read_view_type const& value) {
  symbol_map[name] = value;
}

template <typename ViewType>
void Eval<ViewType>::inspect_arg(Teuchos::any const& arg, bool& is_view, bool& is_bool) {
  if (arg.type() == typeid(bool)) {
    is_view = false;
    is_bool = true;
  } else if (arg.type() == typeid(typename ViewType::value_type)) {
    is_view = false;
    is_bool = false;
  } else if (arg.type() == typeid(typename ReadViewType<ViewType>::type)) {
    is_view = true;
    is_bool = false;
  } else if (arg.type() == typeid(typename ReadBoolViewType<ViewType>::type)) {
    is_view = true;
    is_bool = true;
  } else {
    TEUCHOS_TEST_FOR_EXCEPTION(true, Teuchos::ParserFail,
        "value is of illegal type");
  }
}

template <typename ViewType>
void Eval<ViewType>::single_single_ternary_op(Teuchos::any&, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::single_view_ternary_op(Teuchos::any&, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::view_single_ternary_op(Teuchos::any&, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::view_view_ternary_op(Teuchos::any&, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::single_single_binary_op(BinaryOpCode code, Teuchos::any& result, Teuchos::any& left, Teuchos::any& right) {
  using Single = typename ViewType::value_type;
  switch (code) {
    case BinaryOpCode::ADD: BinaryFunctor<ScalarAdd, Single, Single>("single+single", result, left, right); break;
    case BinaryOpCode::OR:
    case BinaryOpCode::AND:
    case BinaryOpCode::LT:
    case BinaryOpCode::GT:
    case BinaryOpCode::GEQ:
    case BinaryOpCode::LEQ:
    case BinaryOpCode::EQ:
    case BinaryOpCode::MUL:
    case BinaryOpCode::DIV:
    case BinaryOpCode::SUB:
    case BinaryOpCode::POW:
      TEUCHOS_TEST_FOR_EXCEPTION(true, Teuchos::ParserFail, "not yet implemented");
  }
}

template <typename ViewType>
void Eval<ViewType>::single_view_binary_op(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::view_single_binary_op(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::view_view_binary_op(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::single_single_binary_op_bool(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::single_view_binary_op_bool(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::view_view_binary_op_bool(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::view_neg_op(Teuchos::any&, Teuchos::any&) {
}

template <typename ViewType>
void Eval<ViewType>::single_neg_op(Teuchos::any&, Teuchos::any&) {
}

}} // end namespace panzer::Expr

#endif // PANZER_EXPR_EVAL_IMPL_HPP
