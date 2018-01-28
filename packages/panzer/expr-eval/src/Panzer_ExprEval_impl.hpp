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

struct ScalarMul {
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

template <typename Indexed, size_t IterationRank, size_t IndexedRank = Indexed::rank>
struct Indexer;

template <typename ViewType>
struct Indexer<ViewType, 1, 0> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  typename ViewType::reference_type index(ViewType const& x, Integral) { return x(); }
};

template <typename ViewType>
struct Indexer<ViewType, 1, 1> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  typename ViewType::reference_type index(ViewType const& x, Integral i) { return x(i); }
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

template <typename T, size_t IterationRank, size_t TRank = T::Rank>
struct Allocator;

template <typename DT, typename ... VP>
struct Allocator<Kokkos::View<DT, VP ...>, 0, 0> {
  static void allocate(std::string const& name, Kokkos::View<DT, VP ...>& view) {
    view = Kokkos::View<DT, VP ...>{Kokkos::ViewAllocateWithoutInitializing(name)};
  }
};

template <typename DT, typename ... VP>
struct Allocator<Kokkos::View<DT, VP ...>, 1, 0> {
  static void allocate(std::string const& name, Kokkos::View<DT, VP ...>& view, size_t n0) {
    view = Kokkos::View<DT, VP ...>{Kokkos::ViewAllocateWithoutInitializing(name)};
  }
};

template <typename DT, typename ... VP>
struct Allocator<Kokkos::View<DT, VP ...>, 1, 1> {
  static void allocate(std::string const& name, Kokkos::View<DT, VP ...>& view, size_t n0) {
    view = Kokkos::View<DT, VP ...>{Kokkos::ViewAllocateWithoutInitializing(name), n0};
  }
};

template <typename Op, typename Left, typename Right, size_t Rank = RankForAll<Left, Right>::value>
struct BinaryFunctor;

template <typename A, typename B>
struct ResultType {
  static constexpr size_t a_rank = A::rank;
  static constexpr size_t b_rank = B::rank;
  using biggest_type = typename std::conditional<(b_rank > a_rank), B, A>::type;
  using const_type = typename RebindViewType<biggest_type, typename biggest_type::const_value_type>::type;
  using type = typename RebindViewType<biggest_type, typename biggest_type::non_const_value_type>::type;
};

template <typename A>
struct ConstViewType;

template <typename DT, typename ... VP>
struct ConstViewType<Kokkos::View<DT, VP...>> {
  using in_type = Kokkos::View<DT, VP...>;
  using type = typename RebindViewType<in_type, typename in_type::const_value_type>::type;
};

template <typename Op, typename Left, typename Right>
struct BinaryFunctor<Op, Left, Right, 0> {
  using Result = typename ResultType<Left, Right>::type;
  using ConstResult = typename ResultType<Left, Right>::const_type;
  Result result_;
  Left   left_;
  Right  right_;
  BinaryFunctor(std::string const& name, Teuchos::any& result, Teuchos::any& left, Teuchos::any& right) {
    left_ = Teuchos::any_cast<Left>(left);
    right_ = Teuchos::any_cast<Right>(right);
    Allocator<Result, 0>::allocate(name, result_);
    result_() = Op::apply(left_(), right_());
    result = ConstResult{result_};
  }
};

template <typename Op, typename Left, typename Right>
struct BinaryFunctor<Op, Left, Right, 1> {
  using Result = typename ResultType<Left, Right>::type;
  using ConstResult = typename ResultType<Left, Right>::const_type;
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
          ExtentsFor<Left>::extent(left_, 0),
          ExtentsFor<Right>::extent(right_, 0));
    Allocator<Result, 1>::allocate(name, result_, extent_0);
    Kokkos::parallel_for(name, Kokkos::RangePolicy<execution_space>(0, extent_0), *this);
    result = ConstResult{result_};
  }
};

template <typename DT, typename ... VP>
Eval<DT, VP ...>::Eval()
  : EvalBase() 
{
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::set(std::string const& name, bool value) {
  single_bool_view_type view{Kokkos::ViewAllocateWithoutInitializing{name}};
  auto host_view = Kokkos::create_mirror_view(view);
  host_view() = value;
  Kokkos::deep_copy(view, host_view);
  symbol_map[name] = const_single_bool_view_type{view};
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::set(std::string const& name, scalar_type const& value) {
  single_view_type view{Kokkos::ViewAllocateWithoutInitializing{name}};
  auto host_view = Kokkos::create_mirror_view(view);
  host_view() = value;
  Kokkos::deep_copy(view, host_view);
  symbol_map[name] = const_single_view_type{view};
  bool a, b;
  this->inspect_arg(symbol_map[name], a, b);
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::set(std::string const& name, const_view_type const& value) {
  symbol_map[name] = value;
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::make_constant(Teuchos::any& result, double value) {
  single_view_type view{Kokkos::ViewAllocateWithoutInitializing{"constant"}};
  auto host_view = Kokkos::create_mirror_view(view);
  host_view() = value;
  Kokkos::deep_copy(view, host_view);
  result = const_single_view_type{view};
  bool a, b;
  this->inspect_arg(result, a, b);
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::inspect_arg(Teuchos::any const& arg, bool& is_many, bool& is_bool) {
  if (arg.type() == typeid(const_single_bool_view_type)) {
    is_many = false;
    is_bool = true;
  } else if (arg.type() == typeid(const_single_view_type)) {
    is_many = false;
    is_bool = false;
  } else if (arg.type() == typeid(const_view_type)) {
    is_many = true;
    is_bool = false;
  } else if (arg.type() == typeid(const_bool_view_type)) {
    is_many = true;
    is_bool = true;
  } else {
    TEUCHOS_TEST_FOR_EXCEPTION(true, Teuchos::ParserFail,
        "value is of illegal type " << arg.typeName() << ", view type is "
        << typeid(const_view_type).name());
  }
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::single_single_ternary_op(Teuchos::any&, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::single_many_ternary_op(Teuchos::any&, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::many_single_ternary_op(Teuchos::any&, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::many_many_ternary_op(Teuchos::any&, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::single_single_binary_op(BinaryOpCode code, Teuchos::any& result, Teuchos::any& left, Teuchos::any& right) {
  using SingleBool = const_single_bool_view_type;
  using Single = const_single_view_type;
  switch (code) {
    case BinaryOpCode::OR:  BinaryFunctor<ScalarOr , SingleBool, SingleBool>("single||single", result, left, right); break;
    case BinaryOpCode::AND: BinaryFunctor<ScalarAnd, SingleBool, SingleBool>("single&&single", result, left, right); break;
    case BinaryOpCode::LT:  BinaryFunctor<ScalarLT , Single, Single>("single< single", result, left, right); break;
    case BinaryOpCode::GT:  BinaryFunctor<ScalarGT , Single, Single>("single> single", result, left, right); break;
    case BinaryOpCode::GEQ: BinaryFunctor<ScalarGEQ, Single, Single>("single>=single", result, left, right); break;
    case BinaryOpCode::LEQ: BinaryFunctor<ScalarLEQ, Single, Single>("single<=single", result, left, right); break;
    case BinaryOpCode::EQ:  BinaryFunctor<ScalarEQ , Single, Single>("single==single", result, left, right); break;
    case BinaryOpCode::MUL: BinaryFunctor<ScalarMul, Single, Single>("single* single", result, left, right); break;
    case BinaryOpCode::DIV: BinaryFunctor<ScalarDiv, Single, Single>("single/ single", result, left, right); break;
    case BinaryOpCode::ADD: BinaryFunctor<ScalarAdd, Single, Single>("single+ single", result, left, right); break;
    case BinaryOpCode::SUB: BinaryFunctor<ScalarSub, Single, Single>("single- single", result, left, right); break;
    case BinaryOpCode::POW: BinaryFunctor<ScalarPow, Single, Single>("single^ single", result, left, right); break;
  }
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::single_many_binary_op(BinaryOpCode code, Teuchos::any& result, Teuchos::any& left, Teuchos::any& right) {
  using Single = const_single_view_type;
  using SingleBool = const_single_bool_view_type;
  using Many = const_view_type;
  using ManyBool = const_bool_view_type;
  switch (code) {
    case BinaryOpCode::OR:  BinaryFunctor<ScalarOr , SingleBool, ManyBool>("single||many", result, left, right); break;
    case BinaryOpCode::AND: BinaryFunctor<ScalarAnd, SingleBool, ManyBool>("single&&many", result, left, right); break;
    case BinaryOpCode::LT:  BinaryFunctor<ScalarLT , Single, Many>("single< many", result, left, right); break;
    case BinaryOpCode::GT:  BinaryFunctor<ScalarGT , Single, Many>("single> many", result, left, right); break;
    case BinaryOpCode::GEQ: BinaryFunctor<ScalarGEQ, Single, Many>("single>=many", result, left, right); break;
    case BinaryOpCode::LEQ: BinaryFunctor<ScalarLEQ, Single, Many>("single<=many", result, left, right); break;
    case BinaryOpCode::EQ:  BinaryFunctor<ScalarEQ , Single, Many>("single==many", result, left, right); break;
    case BinaryOpCode::MUL: BinaryFunctor<ScalarMul, Single, Many>("single* many", result, left, right); break;
    case BinaryOpCode::DIV: BinaryFunctor<ScalarDiv, Single, Many>("single/ many", result, left, right); break;
    case BinaryOpCode::ADD: BinaryFunctor<ScalarAdd, Single, Many>("single+ many", result, left, right); break;
    case BinaryOpCode::SUB: BinaryFunctor<ScalarSub, Single, Many>("single- many", result, left, right); break;
    case BinaryOpCode::POW: BinaryFunctor<ScalarPow, Single, Many>("single^ many", result, left, right); break;
  }
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::many_single_binary_op(BinaryOpCode code, Teuchos::any& result, Teuchos::any& left, Teuchos::any& right) {
  using Single = const_single_view_type;
  using SingleBool = const_single_bool_view_type;
  using Many = const_view_type;
  using ManyBool = const_bool_view_type;
  switch (code) {
    case BinaryOpCode::OR:  BinaryFunctor<ScalarOr , ManyBool, SingleBool>("many||single", result, left, right); break;
    case BinaryOpCode::AND: BinaryFunctor<ScalarAnd, ManyBool, SingleBool>("many&&single", result, left, right); break;
    case BinaryOpCode::LT:  BinaryFunctor<ScalarLT , Many, Single>("many< single", result, left, right); break;
    case BinaryOpCode::GT:  BinaryFunctor<ScalarGT , Many, Single>("many> single", result, left, right); break;
    case BinaryOpCode::GEQ: BinaryFunctor<ScalarGEQ, Many, Single>("many>=single", result, left, right); break;
    case BinaryOpCode::LEQ: BinaryFunctor<ScalarLEQ, Many, Single>("many<=single", result, left, right); break;
    case BinaryOpCode::EQ:  BinaryFunctor<ScalarEQ , Many, Single>("many==single", result, left, right); break;
    case BinaryOpCode::MUL: BinaryFunctor<ScalarMul, Many, Single>("many* single", result, left, right); break;
    case BinaryOpCode::DIV: BinaryFunctor<ScalarDiv, Many, Single>("many/ single", result, left, right); break;
    case BinaryOpCode::ADD: BinaryFunctor<ScalarAdd, Many, Single>("many+ single", result, left, right); break;
    case BinaryOpCode::SUB: BinaryFunctor<ScalarSub, Many, Single>("many- single", result, left, right); break;
    case BinaryOpCode::POW: BinaryFunctor<ScalarPow, Many, Single>("many^ single", result, left, right); break;
  }
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::many_many_binary_op(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::single_single_binary_op_bool(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::single_many_binary_op_bool(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::many_many_binary_op_bool(BinaryOpCode code, Teuchos::any&, Teuchos::any&, Teuchos::any&) {
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::many_neg_op(Teuchos::any&, Teuchos::any&) {
}

template <typename DT, typename ... VP>
void Eval<DT, VP ...>::single_neg_op(Teuchos::any&, Teuchos::any&) {
}

}} // end namespace panzer::Expr

#endif // PANZER_EXPR_EVAL_IMPL_HPP
