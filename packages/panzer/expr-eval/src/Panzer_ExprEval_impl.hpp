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

#include <Panzer_ExprEval.hpp>

namespace panzer
{
namespace Expr
{

struct ScalarTernary {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T operator()(bool cond, T const& left, T const& right) {
    return cond ? left : right;
  }
};

struct ScalarOr {
  static KOKKOS_FORCEINLINE_FUNCTION
  bool operator()(bool left, bool right) {
    return left || right;
  }
};

struct ScalarAnd {
  static KOKKOS_FORCEINLINE_FUNCTION
  bool operator()(bool left, bool right) {
    return left && right;
  }
};

struct ScalarGT {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool operator()(T const& left, T const& right) {
    return left > right;
  }
};

struct ScalarLT {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool operator()(T const& left, T const& right) {
    return left < right;
  }
};

struct ScalarGEQ {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool operator()(T const& left, T const& right) {
    return left >= right;
  }
};

struct ScalarLEQ {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool operator()(T const& left, T const& right) {
    return left <= right;
  }
};

struct ScalarEQ {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  bool operator()(T const& left, T const& right) {
    return left == right;
  }
};

struct ScalarAdd {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T operator()(T const& left, T const& right) {
    return left + right;
  }
};

struct ScalarSub {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T operator()(T const& left, T const& right) {
    return left - right;
  }
};

struct ScalarProd {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T operator()(T const& left, T const& right) {
    return left * right;
  }
};

struct ScalarDiv {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T operator()(T const& left, T const& right) {
    return left / right;
  }
};

struct ScalarPow {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T operator()(T const& left, T const& right) {
    using std::pow;
    return pow(left, right);
  }
};

struct ScalarNeg {
  template <typename T>
  static KOKKOS_FORCEINLINE_FUNCTION
  T operator()(T const& right) {
    return -right;
  }
};

template <typename DataType, typename NewScalarType>
struct Rebind {
  using type = NewScalarType;
};

template <typename NestedDataType, typename NewScalarType>
struct RebindDataType<NestedDataType*, NewScalarType> {
  using type = typename RebindDataType<NestedDataType>::type *;
};

template <typename NestedDataType, typename NewScalarType>
struct RebindDataType<NestedDataType[], NewScalarType> {
  using type = typename RebindDataType<NestedDataType>::type [];
};

template <typename NestedDataType, typename NewScalarType, size_t N>
struct RebindDataType<NestedDataType[N], NewScalarType> {
  using type = typename RebindDataType<NestedDataType, NewScalarType>::type [N];
};

template <typename ViewType, typename NewScalarType>
struct RebindViewType;

template <typename DT, typename NewScalarType, typename VP ...>
struct RebindViewType<Kokkos::View<DT, VP ...>> {
  using type = Kokkos::View<typename RebindDataType<DT, NewScalarType>::type, VP ...>;
};

template <typename ViewType>
struct ReadViewType;

template <typename DT, typename VP ...>
struct ReadViewType<Kokkos::View<DT, VP ...>> {
  using type = Kokkos::View<typename RebindDataType<DT, typename Kokkos::View<DT, VP ...>::const_value_type>::type, VP ...>;
};

template <typename ViewType>
struct WriteViewType;

template <typename DT, typename VP ...>
struct WriteViewType<Kokkos::View<DT, VP ...>> {
  using type = Kokkos::View<typename RebindDataType<DT, typename Kokkos::View<DT, VP ...>::non_const_value_type>::type, VP ...>;
};

template <typename ViewType>
struct ReadBoolViewType;

template <typename DT, typename VP ...>
struct ReadBoolViewType<Kokkos::View<DT, VP ...>> {
  using type = Kokkos::View<typename RebindDataType<DT, bool const>::type, VP ...>;
};

template <typename ViewType>
struct WriteBoolViewType;

template <typename DT, typename VP ...>
struct WriteBoolViewType<Kokkos::View<DT, VP ...>> {
  using type = Kokkos::View<typename RebindDataType<DT, bool>::type, VP ...>;
};

struct Indexer;

template <typename Indexed>
struct Indexer<Indexed, 1> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  Indexed& index(Indexed& x, Integral) { return x; }
}

template <typename DT, typename VP ...>
struct Indexer<Kokkos::View<DT, VP...>, 1> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  Kokkos::View<DT, VP...>::reference_type
  index(Kokkos::View<DT, VP...> const& x, Integral i) { return x(i); }
}

template <typename Indexed>
struct Indexer<Indexed, 2> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  Indexed& index(Indexed& x, Integral, Integral) { return x; }
}

template <typename DT, typename VP ...>
struct Indexer<Kokkos::View<DT, VP...>, 2> {
  template <typename Integral>
  static KOKKOS_FORCEINLINE_FUNCTION
  Kokkos::View<DT, VP...>::reference_type
  index(Kokkos::View<DT, VP...> const& x, Integral i, Integral j) { return x(i, j); }
}

template <typename T>
struct ExtentsFor {
  static size_t extent(T const&, size_t) { return 1; }
};

template <typename ST, typename VP ...>
struct ExtentsFor<Kokkos::View<ST, VP...>> {
  static size_t extent(Kokkos::View<ST, VP...> const& view, size_t i) { return view.extent(i); }
};

template <typename T>
struct RankFor {
  static constexpr size_t value = 0;
};

template <typename ST, typename VP ...>
struct RankFor<Kokkos::View<ST, VP...>> {
  static constexpr size_t value = Kokkos::View<ST, VP...>::rank;
};

template <typename T, typename TS ...>
struct RankForAll {
  static constexpr size_t left_value = RankFor<T>::value;
  static constexpr size_t right_value = RankForAll<TS...>::value;
  static constexpr size_t value = left_value > right_value ? left_value : right_value;
};

template <typename T>
struct ExecSpaceFor {
  using type = Kokkos::AnonymousSpace;
};

template <typename ST, typename VP ...>
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

template <typename T, typename TS ...>
struct ExecSpaceForAll {
  using type =
    typename CombineTwoExecSpaces<
        typename ExecSpaceFor<T>,
        typename ExecSpaceForAll<TS...>::type>::type;
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

template <typename Op, typename Left, typename Right, size_t Rank = RankForAll<Left, Right>::value>
struct BinaryFunctor;

template <typename Op, typename Result, typename Left, typename Right>
struct BinaryFunctor<Op, Left, Right, 1> {
  using execution_space =
    typename SetDefaultExecSpace<
      typename ExecSpaceForAll<Result, Left, Right>::type,
      Kokkos::Serial>::type;
  Result result_;
  Left   left_;
  Right  right_;
  KOKKOS_INLINE_FUNCTION
  void operator()(typename execution_space::size_type i) const {
    Indexer<Result>::index(result_, i) =
      Op::operator()(
          Indexer<Left>::index(left_, i),
          Indexer<Right>::index(right_, i));
  }
  BinaryFunctor(std::string const& name) {
    auto extent_0 =
      std::max(
          ExtentsFor<Left>::extent(0),
          ExtentsFor<Right>::extent(0));
    Kokkos::parallel_for(name, Kokkos::RangePolicy<execution_space>(0, extent_0), *this);
  }
}

}} // end namespace panzer::Expr

#endif // PANZER_EXPR_EVAL_IMPL_HPP
