// @HEADER
//
// ***********************************************************************
//
//                    Teuchos: Common Tools Package
//                 Copyright (2004) Sandia Corporation
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
// Questions? Contact
//                    Dan Ibanez        (daibane@sandia.gov)
//
// ***********************************************************************
//
// @HEADER

#ifndef TEUCHOS_YAMLPARSER_DEF_H_
#define TEUCHOS_YAMLPARSER_DEF_H_

#include <iostream>
#include <iomanip>
#include <ios>
#include <sstream>

#include "Teuchos_YamlParser_decl.hpp"
#include "Teuchos_XMLParameterListCoreHelpers.hpp"
#include "Teuchos_YamlParameterListCoreHelpers.hpp"
#include "Teuchos_TwoDArray.hpp"

namespace Teuchos {

template <typename T>
bool is_parseable_as(std::string const& text) {
  std::istringstream ss(text);
  T value;
  return (ss >> value);
}

template <typename T>
T parse_as(std::string const& text) {
  std::istringstream ss(text);
  T value;
  ss >> value;
  return value;
}

struct PLPair {
  std::string key;
  ParameterEntry value;
};

struct Scalar {
  enum Source {
    RAW,
    DQUOTED,
    SQUOTED,
    BLOCK
  };
  /* order matters, a higher type should be convertible to a lower type */
  enum Type {
    STRING = 0,
    DOUBLE = 1,
    INT    = 2
  };
  int source;
  int tagged_type;
  std::string text;
  int infer_type() const {
    if (tagged_type != -1) return tagged_type;
    if (source != RAW) return STRING;
    if (is_parseable_as<int>(text)) return INT;
    if (is_parseable_as<double>(text)) return DOUBLE;
    return STRING;
  }
};

bool operator==(PLPair const&, PLPair const&) { return false; }
bool operator<<(std::ostream& os, PLPair const&) { return os; }

bool operator==(Scalar const&, Scalar const&) { return false; }
bool operator<<(std::ostream& os, Scalar const&) { return os; }

namespace YAMLParameterList {

class Reader : public Teuchos::Reader {
 public:
  Reader():Teuchos::Reader(Teuchos::YAML::ask_reader_tables()) {}
  virtual ~Reader() {}
 protected:
  virtual void at_shift(any& result_any, int token, std::string& text) {
    using std::swap;
    switch (token) {
      case Teuchos::YAML::TOK_NEWLINE:
        std::string& result = make_any_ref<std::string>(result_any);
        swap(result, text);
        break;
      case Teuchos::YAML::TOK_SPACE:
      case Teuchos::YAML::TOK_OTHERCHAR:
        result_any = text.at(0);
        break;
    }
  }
  virtual void at_reduce(any& result_any, int prod, std::vector<any>& rhs) {
    using std::swap;
    switch (prod) {
      case Teuchos::YAML::PROD_DOC:
      case Teuchos::YAML::PROD_TOP_BMAP: {
        swap(result_any, rhs.at(0));
        break;
      }
      case Teuchos::YAML::PROD_TOP_FIRST: {
        if (rhs.at(0).type() != typeid(PLPair)) break;
        map_first_item(result_any, rhs.at(0));
        break;
      }
      case Teuchos::YAML::PROD_TOP_NEXT: {
        if (rhs.at(1).type() != typeid(PLPair)) break;
        if (rhs.at(0).type() == typeid(ParameterList)) {
          map_next_item(result_any, rhs.at(0), rhs.at(1));
        } else {
          map_first_item(result_any, rhs.at(0));
        }
        break;
      }
      case Teuchos::YAML::PROD_BMAP_FIRST: {
        map_first_item(result_any, rhs.at(0));
        break;
      }
      case Teuchos::YAML::PROD_BMAP_NEXT: {
        map_next_item(result_any, rhs.at(0), rhs.at(1));
        break;
      }
      case Teuchos::YAML::PROD_BMAP_SCALAR: {
        int scalar_type = interpret_tag(rhs.at(3));
        map_item(result_any, rhs.at(0), rhs.at(4), scalar_type);
        break;
      }
      case Teuchos::YAML::PROD_BMAP_BCALAR: {
        map_item(result_any, rhs.at(0), rhs.at(3), Scalar::STRING);
        break;
      }
      case Teuchos::YAML::PROD_BMAP_BMAP: {
        map_item(result_any, rhs.at(0), rhs.at(6), Scalar::STRING);
        break;
      }
      case Teuchos::YAML::PROD_BMAP_BSEQ: {
        map_item(result_any, rhs.at(0), rhs.at(6), Scalar::STRING);
        break;
      }
      case Teuchos::YAML::PROD_BMAP_FMAP: {
        map_item(result_any, rhs.at(0), rhs.at(4), Scalar::STRING);
        break;
      }
      case Teuchos::YAML::PROD_BMAP_FSEQ: {
        int scalar_type = interpret_tag(rhs.at(3));
        map_item(result_any, rhs.at(0), rhs.at(4), scalar_type);
        break;
      }
      case Teuchos::YAML::PROD_BSEQ_FIRST: {
        seq_first_item(result_any, rhs.at(0));
        break;
      }
      case Teuchos::YAML::PROD_BSEQ_NEXT: {
        seq_next_item(result_any, rhs.at(0), rhs.at(1));
        break;
      }
      case Teuchos::YAML::PROD_BSEQ_SCALAR: {
        swap(result_any, rhs.at(3));
        Scalar& scalar = any_ref_cast<Scalar>(result_any);
        scalar.tagged_type = interpret_tag(rhs.at(2));
        break;
      }
      case Teuchos::YAML::PROD_BSEQ_BSCALAR: {
        swap(result_any, rhs.at(2));
        break;
      }
      case Teuchos::YAML::PROD_BSEQ_BMAP:
      case Teuchos::YAML::PROD_BSEQ_BMAP_TRAIL:
      case Teuchos::YAML::PROD_BSEQ_FMAP: {
        throw ParserFail("Can't interpret a map inside a sequence as a Teuchos Parameter");
      }
      case Teuchos::YAML::PROD_BSEQ_BSEQ: {
        swap(result_any, rhs.at(4));
        break;
      }
      case Teuchos::YAML::PROD_BSEQ_BSEQ_TRAIL: {
        swap(result_any, rhs.at(5));
        break;
      }
      case Teuchos::YAML::PROD_BSEQ_BSEQ_FSEQ: {
        swap(result_any, rhs.at(3));
        break;
      }
      case Teuchos::YAML::PROD_FMAP: {
        swap(result_any, rhs.at(2));
        break;
      }
      case Teuchos::YAML::PROD_FMAP_EMPTY: {
        ParameterList& list = make_any_ref<ParameterList>(result_any);
        break;
      }
      case Teuchos::YAML::PROD_FSEQ_EMPTY: {
        Array<Scalar>& array = make_any_ref<Array<Scalar> >(result_any);
        break;
      }
      case Teuchos::YAML::PROD_FSEQ_FIRST: {
        seq_first_item(result_any, rhs.at(0));
        break;
      }
      case Teuchos::YAML::PROD_FSEQ_NEXT: {
        seq_first_item(result_any, rhs.at(0), rhs.at(3));
        break;
      }
      case Teuchos::YAML::PROD_FSEQ_SCALAR: {
        swap(result_any, rhs.at(1));
        Scalar& scalar = any_ref_cast<Scalar>(result_any);
        scalar.tagged_type = interpret_tag(rhs.at(0));
        break;
      }
      case Teuchos::YAML::PROD_FSEQ_FSEQ:
      case Teuchos::YAML::PROD_FSEQ_FMAP: {
        swap(result_any, rhs.at(1));
        break;
      }
      case Teuchos::YAML::PROD_SCALAR_NORMAL: {
        char first = any_cast<char>(rhs.at(0));
        std::string& str = any_ref_cast<std::string>(rhs.at(1));
        Scalar& scalar = make_any_ref<Scalar>(result_any);
        scalar.text += first;
        scalar.text += str;
        scalar.source = Scalar::RAW;
        break;
      }
      case Teuchos::YAML::PROD_SCALAR_DOT:
      case Teuchos::YAML::PROD_SCALAR_DASH: {
        char second = any_cast<char>(rhs.at(1));
        std::string& rest = any_ref_cast<std::string>(rhs.at(2));
        Scalar& scalar = make_any_ref<Scalar>(result_any);
        if (prod == Teuchos::YAML::PROD_SCALAR_DOT) scalar.text += '.';
        if (prod == Teuchos::YAML::PROD_SCALAR_DASH) scalar.text += '-';
        scalar.text += second;
        scalar.text += rest;
        scalar.source = Scalar::RAW;
        break;
      }
      case Teuchos::YAML::PROD_SCALAR_DQUOTED:
      case Teuchos::YAML::PROD_SCALAR_SQUOTED: {
        std::string& first = any_ref_cast<std::string>(rhs.at(1));
        std::string& rest = any_ref_cast<std::string>(rhs.at(2));
        Scalar& scalar = make_any_ref<Scalar>(result_any);
        scalar.text += first;
        scalar.text += rest;
        if (prod == Teuchos::YAML::PROD_SCALAR_DQUOTED) {
          scalar.source = Scalar::DQUOTED;
        } else if (prod == Teuchos::YAML::PROD_SCALAR_SQUOTED) {
          scalar.source = Scalar::SQUOTED;
        }
        break;
      }
      case Teuchos::YAML::PROD_TAG: {
        swap(rest, rhs.at(2));
        break;
      }
      case Teuchos::YAML::PROD_BSCALAR: {
        std::string& leading_indent = any_ref_cast<std::string>(rhs.at(2));
        std::string& str = any_ref_cast<std::string>(rhs.at(4));
        std::string newline;
        if (leading_indent[0] == '\r') newline = "\r\n";
        else newline = "\n";
        // leading_indent may start with multiple newlines
        leading_indent = newline + leading_indent.substr(leading_indent.find_first_of(" \t");
        // un-indent the entire block of text
        std::size_t next_spot;
        while ((next_spot = str.find(leading_indent)) != std::string::npos) {
          str = str.substr(0, next_spot) + newline + str.substr(next_spot + leading_indent.size());
        }
        Scalar& scalar = make_any_ref<Scalar>(result_any);
        swap(scalar.text, str);
        scalar.source = Scalar::BLOCK;
        break;
      }
      case Teuchos::YAML::PROD_BSCALAR_FIRST: {
        seq_first_item(result_any, rhs.at(0));
        break;
      }
      // all these cases reduce to concatenating two strings
      case Teuchos::YAML::PROD_BSCALAR_NEXT:
      case Teuchos::YAML::PROD_BSCALAR_LINE:
      case Teuchos::YAML::PROD_DESCAPE_NEXT:
      case Teuchos::YAML::PROD_SESCAPE_NEXT: {
        swap(result_any, rhs.at(0));
        std::string& str = any_ref_cast<std::string>(result_any);
        str += any_ref_cast<std::string>(rhs.at(1));
        break;
      }
      case Teuchos::YAML::PROD_BSCALAR_INDENT: {
        swap(result_any, rhs.at(1));
        break;
      }
      case Teuchos::YAML::PROD_DESCAPE: {
        std::string& str = make_any_ref<std::string>(result_any);
        std::string& rest = any_ref_cast<std::string>(rhs.at(2));
        str += any_cast<char>(rhs.at(1));
        str += rest;
        break;
      }
      case Teuchos::YAML::PROD_SESCAPE: {
        std::string& str = make_any_ref<std::string>(result_any);
        std::string& rest = any_ref_cast<std::string>(rhs.at(2));
        str += '\'';
        str += rest;
        break;
      }
      case Teuchos::YAML::PROD_OTHER_FIRST:
      case Teuchos::YAML::PROD_REST_SPACE:
      case Teuchos::YAML::PROD_REST_OTHER:
      case Teuchos::YAML::PROD_DESCAPED_DQUOTED:
      case Teuchos::YAML::PROD_DQUOTED_COMMON:
      case Teuchos::YAML::PROD_SQUOTED_COMMON:
      case Teuchos::YAML::PROD_ANY_COMMON:
      case Teuchos::YAML::PROD_COMMON_SPACE:
      case Teuchos::YAML::PROD_COMMON_OTHER:
      case Teuchos::YAML::PROD_SPACE_PLUS_FIRST: {
        swap(result_any, rhs.at(0));
        break;
      }
      // all these cases reduce to appending a character
      case Teuchos::YAML::PROD_BSCALAR_DQUOTED_NEXT:
      case Teuchos::YAML::PROD_BSCALAR_SQUOTED_NEXT:
      case Teuchos::YAML::PROD_BSCALAR_ANY_NEXT:
      case Teuchos::YAML::PROD_BSCALAR_REST_NEXT:
      case Teuchos::YAML::PROD_BSCALAR_OTHER_NEXT:
      case Teuchos::YAML::PROD_SPACE_STAR_NEXT:
      case Teuchos::YAML::PROD_SPACE_PLUS_NEXT: {
        swap(result_any, rhs.at(0));
        std::string& str = any_ref_cast<std::string>(result_any);
        str += any_cast<char>(rhs.at(1));
        break;
      }
      case Teuchos::YAML::PROD_DQUOTED_EMPTY:
      case Teuchos::YAML::PROD_SQUOTED_EMPTY:
      case Teuchos::YAML::PROD_DESCAPE_EMPTY:
      case Teuchos::YAML::PROD_SESCAPE_EMPTY:
      case Teuchos::YAML::PROD_REST_EMPTY:
      case Teuchos::YAML::PROD_SPACE_STAR_EMPTY: {
        result_any = std::string();
        break;
      }
      case Teuchos::YAML::PROD_DESCAPED_DQUOT:
      case Teuchos::YAML::PROD_SQUOTED_DQUOT:
      case Teuchos::YAML::PROD_ANY_DQUOT: {
        result_any = '"';
        break;
      }
      case Teuchos::YAML::PROD_DESCAPED_SLASH:
      case Teuchos::YAML::PROD_SQUOTED_SLASH:
      case Teuchos::YAML::PROD_ANY_SLASH: {
        result_any = '\\';
        break;
      }
      case Teuchos::YAML::PROD_DQUOTED_SQUOT:
      case Teuchos::YAML::PROD_ANY_SQUOT: {
        result_any = '\'';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_COLON: {
        result_any = ':';
        break;
      }
      case Teuchos::YAML::PROD_REST_DOT:
      case Teuchos::YAML::PROD_COMMON_DOT: {
        result_any = '.';
        break;
      }
      case Teuchos::YAML::PROD_REST_DASH:
      case Teuchos::YAML::PROD_COMMON_DASH: {
        result_any = '-';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_PIPE: {
        result_any = '|';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_LSQUARE: {
        result_any = '[';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_RSQUARE: {
        result_any = ']';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_LCURLY: {
        result_any = '{';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_RCURLY: {
        result_any = '}';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_COMMA: {
        result_any = ',';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_PERCENT: {
        result_any = '%';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_POUND: {
        result_any = '#';
        break;
      }
      case Teuchos::YAML::PROD_COMMON_EXCL: {
        result_any = '!';
        break;
      }
    }
  }
  void map_first_item(any& result_any, any& first_item) {
    ParameterList& list = make_any_ref<ParameterList>(result_any);
    PLPair& pair = any_ref_cast<PLPair>(first_item);
    list.set(pair.key, pair.value);
  }
  void map_next_item(any& result_any, any& items, any& next_item) {
    using std::swap;
    swap(result_any, items);
    ParameterList& list = any_ref_cast<ParameterList>(result_any);
    PLPair& pair = any_ref_cast<PLPair>(next_item);
    list.set(pair.key, pair.value);
  }
  void map_item(any& result_any, any& key_any, any& value_any) {
    using std::swap;
    PLPair& result = make_any_ref<PLPair>(result_any);
    {
      std::string& key = any_ref_cast<Scalar>(key_any).text;
      swap(result.key, key);
    }
    resolve_map_value(value_any);
    if (value_any.type() == typeid(int)) {
      result.value = any_cast<int>(value_any);
    } else if (value_any.type() == typeid(double)) {
      result.value = any_cast<double>(value_any);
    } else if (value_any.type() == typeid(std::string)) {
      std::string& value = any_ref_cast<std::string >(value_any);
      result.value = ParameterEntry(value);
    } else if (value_any.type() == typeid(Array<int>)) {
      Array<int>& value = any_ref_cast<Array<int> >(value_any);
      result.value = ParameterEntry(value);
    } else if (value_any.type() == typeid(Array<double>)) {
      Array<double>& value = any_ref_cast<Array<double> >(value_any);
      result.value = ParameterEntry(value);
    } else if (value_any.type() == typeid(Array<std::string>)) {
      Array<std::string>& value = any_ref_cast<Array<std::string> >(value_any);
      result.value = ParameterEntry(value);
    } else if (value_any.type() == typeid(TwoDArray<int>)) {
      TwoDArray<int>& value = any_ref_cast<TwoDArray<int> >(value_any);
      result.value = ParameterEntry(value);
    } else if (value_any.type() == typeid(TwoDArray<double>)) {
      TwoDArray<double>& value = any_ref_cast<TwoDArray<double> >(value_any);
      result.value = ParameterEntry(value);
    } else if (value_any.type() == typeid(TwoDArray<std::string>)) {
      TwoDArray<std::string>& value = any_ref_cast<TwoDArray<std::string> >(value_any);
      result.value = ParameterEntry(value);
    } else if (value_any.type() == typeid(ParameterList)) {
      ParameterList& value = any_ref_cast<ParameterList>(value_any);
      ParameterList& result_pl = result.value.setList();
      swap(result_pl, value);
    } else {
      std::string msg = "unexpected YAML map value type ";
      msg += value_any.type().name();
      msg += " for key \"";
      msg += result.key;
      msg += "\"\n";
      throw ParserFail(msg);
    }
  }
  void resolve_map_value(any& value_any, int scalar_type = -1) const {
    if (value_any.type() == typeid(Scalar)) {
      Scalar& scalar_value = any_ref_cast<Scalar>(value_any);
      if (scalar_type == -1) {
        scalar_type = scalar_value.infer_type();
      }
      if (scalar_type == Scalar::INT) {
        value_any = parse_as<int>(scalar_value.text);
      } else if (scalar_type == Scalar::DOUBLE) {
        value_any = parse_as<double>(scalar_value.text);
      } else {
        value_any = scalar_value.text;
      }
    } else if (value_any.type() == typeid(Array<Scalar>)) {
      Array<Scalar>& scalars = any_ref_cast<Array<Scalar> >(value_any);
      if (scalars.size() == 0) {
        throw ParserFail("implicitly typed arrays can't be empty\n"
                         "(need to determine their element type)\n");
      }
      for (Teuchos_Ordinal i = 0; i < scalars.size(); ++i) {
        scalar_type = std::min(scalar_type, scalars[i].infer_type());
      }
      if (scalar_type == Scalar::INT) {
        Array<int> result(scalars.size());
        for (Teuchos_Ordinal i = 0; i < scalars.size(); ++i) {
          result[i] = parse_as<int>(scalars[i].text);
        }
        value_any = result;
      } else if (scalar_type == Scalar::DOUBLE) {
        Array<double> result(scalars.size());
        for (Teuchos_Ordinal i = 0; i < scalars.size(); ++i) {
          result[i] = parse_as<double>(scalars[i].text);
        }
        value_any = result;
      } else if (scalar_type == Scalar::STRING) {
        Array<std::string> result(scalars.size());
        for (Teuchos_Ordinal i = 0; i < scalars.size(); ++i) {
          result[i] = scalars[i].text;
        }
        value_any = result;
      }
    } else if (value_any.type() == typeid(Array<Array<Scalar> >)) {
      Array<Array<Scalar> >& scalars = any_ref_cast<Array<Array<Scalar> > >(value_any);
      if (scalar_type == -1) {
        if (scalars.size() == 0) {
          throw ParserFail("implicitly typed 2D arrays can't be empty\n"
                           "(need to determine their element type)\n");
        }
        for (Teuchos_Ordinal i = 0; i < scalars.size(); ++i) {
          if (scalars[0].size() == 0) {
            throw ParserFail("implicitly typed 2D arrays can't have empty rows\n"
                             "(need to determine their element type)\n");
          }
          if (scalars[i].size() != scalars[0].size()) {
            throw ParserFail("2D array: sub-arrays are different sizes");
          }
          for (Teuchos_Ordinal j = 0; j < scalars.size(); ++j) {
            scalar_type = std::min(scalar_type, scalars[i][j].infer_type());
          }
        }
      }
      if (scalar_type == Scalar::INT) {
        TwoDArray<int> result(scalars.size(), scalars[0].size());
        for (Teuchos_Ordinal i = 0; i < scalars.size(); ++i) {
          for (Teuchos_Ordinal j = 0; j < scalars[0].size(); ++j) {
            result(i, j) = parse_as<int>(scalars[i][j].text);
          }
        }
        value_any = result;
      } else if (scalar_type == Scalar::DOUBLE) {
        TwoDArray<double> result(scalars.size(), scalars[0].size());
        for (Teuchos_Ordinal i = 0; i < scalars.size(); ++i) {
          for (Teuchos_Ordinal j = 0; j < scalars[0].size(); ++j) {
            result(i, j) = parse_as<double>(scalars[i][j].text);
          }
        }
        value_any = result;
      } else if (scalar_type == Scalar::STRING) {
        TwoDArray<std::string> result(scalars.size(), scalars[0].size());
        for (Teuchos_Ordinal i = 0; i < scalars.size(); ++i) {
          for (Teuchos_Ordinal j = 0; j < scalars[0].size(); ++j) {
            result(i, j) = scalars[i][j].text;
          }
        }
        value_any = result;
      }
    }
  }
  int interpret_tag(any& tag_any) {
    if (tag_any.type() != typeid(std::string)) return -1;
    std::string& text = any_ref_cast<std:string>(tag_any);
    if (text.find("int") != std::string::npos) return Scalar::INT;
    else if (text.find("double") != std::string::npos) return Scalar::DOUBLE;
    else if (text.find("string") != std::string::npos) return Scalar::STRING;
    else {
      std::string msg = "Unable to parse type tag \"";
      msg += text
      msg += "\"\n"
      throw ParserFail(msg);
    }
  }
  void seq_first_item(any& result_any, any& first_any) {
    using std::swap;
    if (first_any.type() == typeid(Scalar)) {
      Array<Scalar>& a = make_any_ref<Array<Scalar> >(result_any);
      Scalar& v = any_ref_cast<Scalar>(first_any);
      a.push_back(Scalar());
      swap(a.back(), v);
    } else if (first_any.type() == typeid(Array<Scalar>)) {
      Array<Array<Scalar> >& a = make_any_ref<Array<Array<Scalar> > >(result_any);
      Array<Scalar>& v = any_ref_cast<Array<Scalar> >(first_any);
      a.push_back(Array<Scalar>());
      swap(a.back(), v);
    } else {
      throw Teuchos::ParserFail(
          "bug in YAMLParameterList::Reader: unexpected type for first sequence item");
    }
  }
  void seq_next_item(any& result_any, any& items, any& next_item) {
    using std::swap;
    swap(result_any, items);
    if (result_any.type() == typeid(Array<Scalar>)) {
      Array<Scalar>& a = any_ref_cast<Array<Scalar> >(result_any);
      Scalar& val = any_ref_cast<Scalar>(next_item);
      a.push_back(Scalar());
      swap(a.back(), val);
    } else if (result_any.type() == typeid(Array<Array<Scalar> >)) {
      Array<Array<Scalar> >& a = any_ref_cast<Array<Array<Scalar> > >(result_any);
      Array<Scalar>& v = any_ref_cast<Array<Scalar> >(next_item);
      a.push_back(Array<Scalar>());
      swap(a.back(), v);
    } else {
      throw Teuchos::ParserFail(
          "bug in YAMLParameterList::Reader: unexpected type for next sequence item");
    }
  }
};

/* see https://github.com/jbeder/yaml-cpp/issues/261
   there are times when we want to insist that a parameter
   value be interpreted as a string despite it being parseable
   as a number.
   the standard way to do this in YAML is to put the number in quotes,
   i.e. '1e-3' instead of 1e-3.
   however, the usual YAML::Node::as<T> system doesn't respect quoting
   when trying to cast to numbers.
   so, this is our own version of as<T>, called quoted_as<T>, using
   the Tag workaround suggested in the issue linked above. */

template <typename T>
struct QuotedAs {
  static T eval(YAML::Node const& node) {
    // this "!" tag apparently denotes that the value was quoted
    if (node.Tag() == "!") {
      throw std::runtime_error("quoted_as from quoted string to number");
    }
    return node.as<T>();
  }
};

template <>
struct QuotedAs<std::string> {
  // only a cast to string will succeed if quoted
  static std::string eval(YAML::Node const& node) { return node.as<std::string>(); }
};

template <typename T>
static T quoted_as(YAML::Node const& node) { return QuotedAs<T>::eval(node); }

template<typename T> Teuchos::Array<T> getYamlArray(const YAML::Node& node)
{
  Teuchos::Array<T> arr;
  for(YAML::const_iterator it = node.begin(); it != node.end(); it++)
  {
    arr.push_back(quoted_as<T>(*it));
  }
  return arr;
}

void checkYamlTwoDArray(const YAML::Node& node, const std::string& key)
{
  for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
  {
    if (it->size() != node.begin()->size())
    {
      throw YamlSequenceError(std::string("TwoDArray \"") + key + "\" has irregular sizes");
    }
  }
}

template<typename T> Teuchos::TwoDArray<T> getYamlTwoDArray(const YAML::Node& node)
{
  Teuchos::TwoDArray<T> arr;
  typename Teuchos::TwoDArray<T>::size_type i, j;
  arr.resizeRows(node.size());
  arr.resizeCols(node.begin()->size());
  i = 0;
  for (YAML::const_iterator rit = node.begin(); rit != node.end(); ++rit)
  {
    j = 0;
    for (YAML::const_iterator cit = rit->begin(); cit != rit->end(); ++cit)
    {
      arr(i, j) = quoted_as<T>(*cit);
      ++j;
    }
    ++i;
  }
  return arr;
}

/* Helper functions */

void updateParametersFromYamlFile(const std::string& yamlFileName,
                                  const Teuchos::Ptr<Teuchos::ParameterList>& paramList)
{
  //load the YAML file in as a new param list
  Teuchos::RCP<Teuchos::ParameterList> updated = YAMLParameterList::parseYamlFile(yamlFileName);
  //now update the original list (overwriting values with same key)
  paramList->setParameters(*updated);
}

void updateParametersFromYamlCString(const char* const data,
                                     const Teuchos::Ptr<Teuchos::ParameterList>& paramList,
                                     bool overwrite)
{
  Teuchos::RCP<Teuchos::ParameterList> updated = YAMLParameterList::parseYamlText(data);
  if(overwrite)
  {
    paramList->setParameters(*updated);
  }
  else
  {
    paramList->setParametersNotAlreadySet(*updated);
  }
}

void updateParametersFromYamlString(const std::string& yamlData,
                                  const Teuchos::Ptr<Teuchos::ParameterList>& paramList,
                                  bool overwrite)
{
  Teuchos::RCP<Teuchos::ParameterList> updated = YAMLParameterList::parseYamlText(yamlData);
  if(overwrite)
  {
    paramList->setParameters(*updated);
  }
  else
  {
    paramList->setParametersNotAlreadySet(*updated);
  }
}

Teuchos::RCP<Teuchos::ParameterList> getParametersFromYamlFile(const std::string& yamlFileName)
{
  return YAMLParameterList::parseYamlFile(yamlFileName);
}

Teuchos::RCP<Teuchos::ParameterList> getParametersFromYamlString(const std::string& yamlStr)
{
  std::stringstream ss(yamlStr);
  return YAMLParameterList::parseYamlStream(ss);
}

void writeParameterListToYamlOStream(
  const ParameterList &paramList,
  std::ostream &yamlOut
  )
{
  YAMLParameterList::writeYamlStream(yamlOut, paramList);
}

void writeParameterListToYamlFile(
  const ParameterList &paramList,
  const std::string &yamlFileName
  )
{
  YAMLParameterList::writeYamlFile(yamlFileName, paramList);
}

std::string convertXmlToYaml(const std::string& xmlFileName)
{
  //load the parameter list from xml
  Teuchos::RCP<Teuchos::ParameterList> toConvert = Teuchos::getParametersFromXmlFile(xmlFileName);
  //replace the file extension ".xml" with ".yaml", or append it if there was no extension
  std::string yamlFileName;
  if(xmlFileName.find(".xml") == std::string::npos)
  {
    yamlFileName = xmlFileName + ".yaml";
  }
  else
  {
    yamlFileName = xmlFileName.substr(0, xmlFileName.length() - 4) + ".yaml";
  }
  YAMLParameterList::writeYamlFile(yamlFileName, *toConvert);
  return yamlFileName;
}

void convertXmlToYaml(const std::string& xmlFileName, const std::string& yamlFileName)
{
  //load the parameter list from xml
  Teuchos::RCP<Teuchos::ParameterList> toConvert = Teuchos::getParametersFromXmlFile(xmlFileName);
  //replace the file extension ".xml" with ".yaml", or append it if there was no extension
  YAMLParameterList::writeYamlFile(yamlFileName, *toConvert);
}

void convertXmlToYaml(std::istream& xmlStream, std::ostream& yamlStream)
{
  //read xmlStream into a string until EOF
  std::string xmlString(std::istreambuf_iterator<char>(xmlStream), {});
  //load the parameter list from xml
  Teuchos::RCP<Teuchos::ParameterList> toConvert = Teuchos::getParametersFromXmlString(xmlString);
  //replace the file extension ".xml" with ".yaml", or append it if there was no extension
  YAMLParameterList::writeYamlStream(yamlStream, *toConvert);
}

bool haveSameValuesUnordered(const Teuchos::ParameterList& lhs, const Teuchos::ParameterList& rhs, bool verbose)
{
  typedef Teuchos::ParameterList::ConstIterator Iter;
  Iter i = lhs.begin();
  Iter j = rhs.begin();
  if(lhs.name() != rhs.name())
  {
    if(verbose)
    {
      std::cout << "Parameter list names: \"" << lhs.name() << "\" and \"" << rhs.name() << "\".\n";
    }
    return false;
  }
  for(; i != lhs.end(); i++)
  {
    const std::string& key = lhs.name(i);
    const Teuchos::ParameterEntry& val1 = lhs.entry(i);
    //check that rhs also contains this key
    if(!rhs.isParameter(key))
    {
      if(verbose)
      {
        std::cout << "One list is missing parameter: \"" << key << "\"\n";
      }
      return false;
    }
    const Teuchos::ParameterEntry& val2 = rhs.getEntry(key);
    const Teuchos::any& any1 = val1.getAny(false);
    const Teuchos::any& any2 = val2.getAny(false);
    //check that types match
    if(any1.type() != any2.type())
    {
      if(verbose)
      {
        std::cout << "Values for key \"" << key << "\" have different types: \"" <<
          any1.typeName() << "\" vs \"" << any2.typeName() << "\"\n";
      }
      return false;
    }
    //check for parameter list special case (don't use operator==)
    if(any1.type() == typeid(Teuchos::ParameterList))
    {
      if(!haveSameValuesUnordered(Teuchos::any_cast<Teuchos::ParameterList>(any1), Teuchos::any_cast<Teuchos::ParameterList>(any2), verbose))
      {
        //Don't need to print message here, the deepest list not matching will do that
        return false;
      }
    }
    else
    {
      //otherwise, use == to compare the values
      if(!(val1 == val2))
      {
        if(verbose)
        {
          std::cout << "Values for key \"" << key << "\" are different.\n";
        }
        return false;
      }
    }
    j++;
  }
  //lists must have same # of entries
  if(j != rhs.end())
  {
    if(verbose)
    {
      std::cout << "Lists \"" << lhs.name() << "\" and \"" << rhs.name() << "\" have different number of parameters.\n";
    }
    return false;
  }
  return true;
}

namespace YAMLParameterList
{

Teuchos::RCP<Teuchos::ParameterList> parseYamlText(const std::string& text)
{
  Teuchos::ParameterList pl;
  std::vector<YAML::Node> baseMap = YAML::LoadAll(text);
  return readParams(baseMap);
}

Teuchos::RCP<Teuchos::ParameterList> parseYamlText(const char* text)
{
  Teuchos::ParameterList pl;
  std::vector<YAML::Node> baseMap = YAML::LoadAll(text);
  return readParams(baseMap);
}

Teuchos::RCP<Teuchos::ParameterList> parseYamlFile(const std::string& yamlFile)
{
  std::vector<YAML::Node> baseMap = YAML::LoadAllFromFile(yamlFile);
  return readParams(baseMap);
}

Teuchos::RCP<Teuchos::ParameterList> parseYamlStream(std::istream& yaml)
{
  std::vector<YAML::Node> baseMap = YAML::LoadAll(yaml);
  return readParams(baseMap);
}

Teuchos::RCP<Teuchos::ParameterList> readParams(std::vector<YAML::Node>& lists)
{
  Teuchos::RCP<Teuchos::ParameterList> pl = rcp(new Teuchos::ParameterList); //pl is the root ParameterList to be returned
  //If there is exactly one element in "lists", assume it is the anonymous top-level parameter list
  //If there are more than one, place them all in the anonymous top-level list
  for(size_t i = 0; i < lists.size(); i++)
  {
    processMapNode(lists[i], *pl, true);
  }
  return pl;
}

void processMapNode(const YAML::Node& node, Teuchos::ParameterList& parent, bool topLevel)
{
  if(node.Type() != YAML::NodeType::Map)
  {
    throw YamlStructureError("All top-level elements of the YAML file must be maps.");
  }
  if(topLevel)
  {
    parent.setName("ANONYMOUS");
    processMapNode(node.begin()->second, parent);
  }
  else
  {
    for(YAML::const_iterator i = node.begin(); i != node.end(); i++)
    {
      //make sure the key type is a string
      if(i->first.Type() != YAML::NodeType::Scalar)
      {
        throw YamlKeyError("Keys must be plain strings");
      }
      //if this conversion fails and throws for any reason (shouldn't), let the caller handle it
      const std::string key = quoted_as<std::string>(i->first);
      processKeyValueNode(key, i->second, parent, topLevel);
    }
  }
}

void processKeyValueNode(const std::string& key, const YAML::Node& node, Teuchos::ParameterList& parent, bool topLevel)
{
  //node (value) type can be a map (for nested param lists),
  //a scalar (int, double, string), or a sequence of doubles (vector<double>)
  if(node.Type() == YAML::NodeType::Scalar)
  {
    try
    {
      parent.set(key, quoted_as<int>(node));
    }
    catch(...)
    {
      try
      {
        parent.set(key, quoted_as<double>(node));
      }
      catch(...)
      {
        try
        {
          std::string rawString = quoted_as<std::string>(node);
          if(rawString == "true")
          {
            parent.set<bool>(key, true);
          }
          else if(rawString == "false")
          {
            parent.set<bool>(key, false);
          }
          else
          {
            parent.set(key, rawString);
          }
        }
        catch(...)
        {
          throw YamlScalarError("YAML scalars must be int, double, bool or string.");
        }
      }
    }
  }
  else if(node.Type() == YAML::NodeType::Map)
  {
    if(topLevel)
    {
      processMapNode(node, parent);
    }
    else
    {
      Teuchos::ParameterList& sublist = parent.sublist(key);
      processMapNode(node, sublist);
    }
  }
  else if(node.Type() == YAML::NodeType::Sequence)
  {
    if (node.begin()->Type() == YAML::NodeType::Sequence) {
      checkYamlTwoDArray(node, key);
      YAML::Node const& first_value = *(node.begin()->begin());
      try
      {
        quoted_as<int>(first_value);
        parent.set(key, getYamlTwoDArray<int>(node));
      }
      catch(...)
      {
        try
        {
          quoted_as<double>(first_value);
          parent.set(key, getYamlTwoDArray<double>(node));
        }
        catch(...)
        {
          try
          {
            quoted_as<std::string>(first_value);
            parent.set(key, getYamlTwoDArray<std::string>(node));
          }
          catch(...)
          {
            throw YamlSequenceError(std::string("TwoDArray \"") + key + "\" must contain int, double, bool or string");
          }
        }
      }
    } else {
      YAML::Node const& first_value = *(node.begin());
      try
      {
        quoted_as<int>(first_value);
        parent.set(key, getYamlArray<int>(node));
      }
      catch(...)
      {
        try
        {
          quoted_as<double>(first_value);
          parent.set(key, getYamlArray<double>(node));
        }
        catch(...)
        {
          try
          {
            quoted_as<std::string>(first_value);
            parent.set(key, getYamlArray<std::string>(node));
          }
          catch(...)
          {
            throw YamlSequenceError(std::string("Array \"") + key + "\" must contain int, double, bool or string");
          }
        }
      }
    }
  }
  else if(node.Type() == YAML::NodeType::Null)
  {
    //treat NULL as empty string (not an error)
    parent.set(key, std::string());
  }
  else
  {
    //Undefined
    throw YamlUndefinedNodeError("Value type in a key-value pair must be one of: int, double, string, array, sublist.");
  }
}

void writeYamlStream(std::ostream& yaml, const Teuchos::ParameterList& pl)
{
  //warn the user if floats/doubles with integer values will be printed incorrectly
  auto flags = yaml.flags();
  //make temporary stringstream to test flags
  std::ostringstream testStream;
  testStream.flags(flags);
  double testVal = 1;
  testStream << testVal;
  bool popFlags = false;
  if(testStream.str() == "1")
  {
    //must add showpoint to flags while writing yaml
    //this will always disambiguate (double) n and n, even if stream precision is 0
    //prints as "n.0000" where the number of trailing zeros is the stream precision
    //note: in YAML, "5." is a double but not an int
    std::cout << "Warning: yaml stream format flags would confuse double with integer value with int.\n";
    std::cout << "Setting std::ios::showpoint on the stream to fix this (will restore flags when done)\n";
    auto flagsCopy = flags;
    flagsCopy |= std::ios::showpoint;
    popFlags = true;
  }
  yaml << "%YAML 1.1\n---\n";
  yaml << "ANONYMOUS:";         //original top-level list name is not stored by ParameterList
  if(pl.numParams() == 0)
  {
    yaml << " { }\n";
  }
  else
  {
    writeParameterList(pl, yaml, 2);
  }
  yaml << "...\n";
  //restore flags
  if(popFlags)
  {
    yaml.flags(flags);
  }
}

void writeYamlFile(const std::string& yamlFile, const Teuchos::ParameterList& pl)
{
  std::ofstream yaml(yamlFile);
  /* set default floating-point style:
     1. 17 decimal places to ensure the value remains the same
     2. scientific: this prevents floating-point values that happen
        to be integers from being printed as integers, because YAML
        will then read that value back typed as an integer.
   */
  yaml << std::scientific << std::setprecision(17);
  writeYamlStream(yaml, pl);
}

void writeParameterList(const Teuchos::ParameterList& pl, std::ostream& yaml, int indentLevel)
{
  if(pl.begin() == pl.end())
  {
    yaml << "{ }\n";
  }
  else
  {
    yaml << '\n';
    for(PLIter it = pl.begin(); it != pl.end(); it++)
    {
      writeParameter(pl.name(it), pl.entry(it), yaml, indentLevel);
    }
  }
}

template <typename T>
struct YamlWrite {
  static void write(T const& x, std::ostream& stream) {
    stream << x;
  }
};

template <>
struct YamlWrite<double> {
  static void write(double const& x, std::ostream& stream) {
    generalWriteDouble(x, stream);
  }
};

template <>
struct YamlWrite<std::string> {
  static void write(std::string const& x, std::ostream& stream) {
    generalWriteString(x, stream);
  }
};

template <typename T>
void writeYamlTwoDArray(Teuchos::TwoDArray<T> const& arr, std::ostream& stream)
{
  typename Teuchos::TwoDArray<T>::size_type i, j;
  stream << '[';
  for (i = 0; i < arr.getNumRows(); ++i)
  {
    if (i) stream << ", ";
    stream << '[';
    for (j = 0; j < arr.getNumCols(); ++j)
    {
      if (j) stream << ", ";
      YamlWrite<T>::write(arr(i, j), stream);
    }
    stream << ']';
  }
  stream << ']';
}

void writeParameter(const std::string& paramName, const Teuchos::ParameterEntry& entry, std::ostream& yaml, int indentLevel)
{
  for(int i = 0; i < indentLevel; i++)
  {
    yaml << ' ';
  }
  generalWriteString(paramName, yaml);
  yaml << ": ";
  if(entry.isList())
  {
    writeParameterList(Teuchos::getValue<Teuchos::ParameterList>(entry), yaml, indentLevel + 2);
    return;
  }
  else if(entry.isArray())
  {
    yaml << '[';
    if(entry.isType<Teuchos::Array<int> >())
    {
      Teuchos::Array<int>& arr = Teuchos::getValue<Teuchos::Array<int> >(entry);
      for(int i = 0; i < arr.size(); i++)
      {
        yaml << arr[i];
        if(i != arr.size() - 1)
          yaml << ", ";
      }
    }
    else if(entry.isType<Teuchos::Array<double> >())
    {
      Teuchos::Array<double>& arr = Teuchos::getValue<Teuchos::Array<double> >(entry);
      for(int i = 0; i < arr.size(); i++)
      {
        generalWriteDouble(arr[i], yaml);
        if(i != arr.size() - 1)
          yaml << ", ";
      }
    }
    else if(entry.isType<Teuchos::Array<std::string> >())
    {
      Teuchos::Array<std::string>& arr = Teuchos::getValue<Teuchos::Array<std::string> >(entry);
      for(int i = 0; i < arr.size(); i++)
      {
        generalWriteString(arr[i], yaml);
        if(i != arr.size() - 1)
          yaml << ", ";
      }
    }
    yaml << ']';
  }
  else if(entry.isTwoDArray())
  {
    if(entry.isType<Teuchos::TwoDArray<int> >())
    {
      writeYamlTwoDArray<int>(
          Teuchos::getValue<Teuchos::TwoDArray<int> >(entry), yaml);
    }
    else if(entry.isType<Teuchos::TwoDArray<double> >())
    {
      writeYamlTwoDArray<double>(
          Teuchos::getValue<Teuchos::TwoDArray<double> >(entry), yaml);
    }
    else if(entry.isType<Teuchos::TwoDArray<std::string> >())
    {
      writeYamlTwoDArray<std::string>(
          Teuchos::getValue<Teuchos::TwoDArray<std::string> >(entry), yaml);
    }
  }
  else if(entry.isType<int>())
  {
    yaml << Teuchos::getValue<int>(entry);
  }
  else if(entry.isType<double>())
  {
    generalWriteDouble(Teuchos::getValue<double>(entry), yaml);
  }
  else if(entry.isType<std::string>())
  {
    std::string& str = Teuchos::getValue<std::string>(entry);
    if(strchr(str.c_str(), '\n'))
    {
      //need explicit indentation so that indentation in the string is preserved
      yaml << "|2-\n";    
      //for each line, apply indent then print the line verbatim
      size_t index = 0;
      while(true)
      {
        size_t next = str.find('\n', index);
        for(int i = 0; i < indentLevel + 2; i++)
        {
          yaml << ' ';
        }
        if(next == std::string::npos)
        {
          yaml << str.substr(index, std::string::npos);
          break;
        }
        else
        {
          yaml << str.substr(index, next - index) << '\n';
        }
        index = next + 1;
      }
    }
    else
    {
      generalWriteString(str, yaml);
    }
  }
  else if(entry.isType<bool>())
  {
    yaml << (Teuchos::getValue<bool>(entry) ? "true" : "false");
  }
  yaml << '\n';
}

void generalWriteString(const std::string& str, std::ostream& yaml)
{
  if(stringNeedsQuotes(str))
  {
    yaml << '\'' << str << '\'';
  }
  else
  {
    yaml << str;
  }
}

void generalWriteDouble(double d, std::ostream& yaml)
{
  yaml << d;
}

template <typename T>
static bool canBeParsedAs(std::string const& s) {
  std::istringstream iss(s);
  T val;
  iss >> std::noskipws >> val;
  return iss.eof() && !iss.fail();
}

static bool containsSpecialCharacters(std::string const& s) {
  char const* const control_chars = ":{}[],&*#?|-<>=!%@\\";
  return s.find_first_of(control_chars) != std::string::npos;
}

bool stringNeedsQuotes(const std::string& s)
{
  return containsSpecialCharacters(s) ||
         canBeParsedAs<int>(s) ||
         canBeParsedAs<double>(s);
}

} //namespace YAMLParameterList

} //namespace Teuchos

#endif
