#include <Teuchos_Assert.hpp>
#include <Teuchos_FiniteAutomaton.hpp>
#include <Teuchos_string.hpp>
#include <Teuchos_Language.hpp>
#include <Teuchos_Parser.hpp>
#include <Teuchos_regex.hpp>
#include <Teuchos_Reader.hpp>
#include <Teuchos_XML.hpp>
#include <Teuchos_YAML.hpp>
#include <Teuchos_MathExpr.hpp>
#include <Teuchos_UnitTestHarness.hpp>

#include <iostream>
#include <sstream>
#include <cstdarg>

namespace {

using namespace Teuchos;

bool accepts(FiniteAutomaton const& fa, std::string const& s, int token = 0) {
  int state = 0;
  for (int i = 0; i < size(s); ++i) {
    char c = at(s, i);
    if (!is_symbol(c)) {
      return false;
    }
    int symbol = get_symbol(c);
    state = step(fa, state, symbol);
    if (state == -1) return false;
  }
  return accepts(fa, state) == token;
}

TEUCHOS_UNIT_TEST( Parser, finite_automaton ) {
  FiniteAutomaton lower;
  make_char_range_nfa(lower, 'a', 'z');
  TEUCHOS_ASSERT(accepts(lower, "a"));
  TEUCHOS_ASSERT(accepts(lower, "q"));
  TEUCHOS_ASSERT(accepts(lower, "z"));
  TEUCHOS_ASSERT(!accepts(lower, "X"));
  TEUCHOS_ASSERT(!accepts(lower, "246"));
  TEUCHOS_ASSERT(!accepts(lower, "abc"));
  TEUCHOS_ASSERT(!accepts(lower, "\xff"));
  FiniteAutomaton single;
  make_char_single_nfa(single, 'q');
  TEUCHOS_ASSERT(!accepts(single, "a"));
  TEUCHOS_ASSERT(accepts(single, "q"));
  TEUCHOS_ASSERT(!accepts(single, "r"));
  TEUCHOS_ASSERT(!accepts(single, "abc"));
  FiniteAutomaton upper;
  make_char_range_nfa(upper, 'A', 'Z');
  FiniteAutomaton alpha_nfa;
  unite(alpha_nfa, lower, upper);
  FiniteAutomaton alpha_dfa;
  make_deterministic(alpha_dfa, alpha_nfa);
  TEUCHOS_ASSERT(get_nstates(alpha_dfa) > 2);
  FiniteAutomaton alpha;
  simplify(alpha, alpha_dfa);
  TEUCHOS_ASSERT(get_nstates(alpha) == 2);
  {
    FiniteAutomaton num;
    make_char_range_nfa(num, '0', '9');
    FiniteAutomaton under;
    make_char_single_nfa(under, '_');
    FiniteAutomaton under_alpha;
    unite(under_alpha, under, alpha);
    FiniteAutomaton under_alnum;
    unite(under_alnum, under_alpha, num);
    FiniteAutomaton under_alnum_star;
    star(under_alnum_star, under_alnum);
    FiniteAutomaton ident_nfa;
    concat(ident_nfa, under_alpha, under_alnum_star);
    FiniteAutomaton ident_dfa;
    make_deterministic(ident_dfa, ident_nfa);
    TEUCHOS_ASSERT(get_nstates(ident_dfa) > 2);
    FiniteAutomaton identifier;
    simplify(identifier, ident_dfa);
    TEUCHOS_ASSERT(get_nstates(identifier) == 2);
    TEUCHOS_ASSERT(accepts(identifier, "_soup"));
    TEUCHOS_ASSERT(!accepts(identifier, "007"));
    TEUCHOS_ASSERT(accepts(identifier, "All_The_Case"));
    TEUCHOS_ASSERT(!accepts(identifier, "fire|hose"));
  }
}

TEUCHOS_UNIT_TEST( Parser, lr0 ) {
  /* The LR(0) grammar $G_1$ on page 20 of Pager's paper */ 
  Language lang;
  lang.tokens.resize(4);
  lang.tokens[0]("a", "a");
  lang.tokens[1]("(", "\\(");
  lang.tokens[2](")", "\\)");
  lang.tokens[3]("+", "\\+");
  lang.productions.resize(5);
  lang.productions[0]("G") >> "(", "E", ")";
  lang.productions[1]("E") >> "E", "+", "T";
  lang.productions[2]("E") >> "T";
  lang.productions[3]("T") >> "(", "E", ")";
  lang.productions[4]("T") >> "a";
  GrammarPtr grammar = make_grammar(lang);
  Parser parser = make_lalr1_parser(grammar);
}

TEUCHOS_UNIT_TEST( Parser, lalr1 ) {
  /* this is a pretty simple language I found in some
     university's homework assignment which is LALR(1)
     but not SLR(1), so it is a good first test of our
     context set derivation */
  Language lang;
  lang.tokens.resize(4);
  lang.tokens[0]("a", "a");
  lang.tokens[1]("b", "b");
  lang.tokens[2]("c", "c");
  lang.tokens[3]("d", "d");
  lang.productions.resize(5);
  lang.productions[0]("S") >> "A", "a";
  lang.productions[1]("S") >> "b", "A", "c";
  lang.productions[2]("S") >> "d", "c";
  lang.productions[3]("S") >> "b", "d", "a";
  lang.productions[4]("A") >> "d";
  GrammarPtr grammar = make_grammar(lang);
  Parser parser = make_lalr1_parser(grammar);
}

TEUCHOS_UNIT_TEST( Parser, regex_lexer ) {
  FiniteAutomaton lexer;
  regex::make_lexer(lexer);
  TEUCHOS_ASSERT(accepts(lexer, "a", regex::TOK_CHAR));
  TEUCHOS_ASSERT(accepts(lexer, ".", regex::TOK_DOT));
  TEUCHOS_ASSERT(accepts(lexer, "?", regex::TOK_MAYBE));
  TEUCHOS_ASSERT(accepts(lexer, "\\.", regex::TOK_CHAR));
  TEUCHOS_ASSERT(accepts(lexer, "\\?", regex::TOK_CHAR));
}

TEUCHOS_UNIT_TEST( Parser, regex_language ) {
  LanguagePtr lang = regex::ask_language();
  GrammarPtr grammar = make_grammar(*lang);
  Parser parser = make_lalr1_parser(grammar);
}

void test_regex_reader(std::string const& regex,
    std::vector<std::string> const& expect_matches,
    std::vector<std::string> const& expect_non_matches) {
  regex::Reader reader(42);
  any result_any;
  reader.read_string(result_any, regex, "test_regex_reader");
  FiniteAutomaton& result = any_ref_cast<FiniteAutomaton>(result_any);
  for (std::vector<std::string>::const_iterator it = expect_matches.begin();
      it != expect_matches.end(); ++it) {
    const std::string& expect_match = *it;
    TEUCHOS_ASSERT(accepts(result, expect_match, 42));
  }
  for (std::vector<std::string>::const_iterator it = expect_non_matches.begin();
      it != expect_non_matches.end(); ++it) {
    const std::string& expect_non_match = *it;
    TEUCHOS_ASSERT(!accepts(result, expect_non_match, 42));
  }
}

/* variadic function to mimic C++11 brace initialization */

std::vector<std::string> sv(std::size_t n, ...) {
  std::vector<std::string> out(n);
  va_list args;
  va_start(args, n);
  for (std::size_t i = 0; i < n; ++i) {
    const char* s = va_arg(args, const char*);
    out[i] = s;
  }
  va_end(args);
  return out;
}

TEUCHOS_UNIT_TEST( Parser, regex_reader ) {
  test_regex_reader("a", sv(1, "a"), sv(2, "b", "aa"));
  test_regex_reader("ab", sv(1, "ab"), sv(3, "a", "b", "abc"));
  test_regex_reader("abc", sv(1, "abc"), sv(4, "a", "ab", "abd", "abcd"));
  test_regex_reader("a?", sv(2, "a", ""), sv(2, "b", "aa"));
  test_regex_reader("[a]", sv(1, "a"), sv(2, "b", "aa"));
  test_regex_reader("[^a]", sv(2, "q", "z"), sv(2, "a", "qq"));
  test_regex_reader("[A-Z]", sv(3, "A", "Q", "Z"), sv(4, "a", "z", "AA", ">"));
  test_regex_reader("\\?", sv(1, "?"), sv(2, "b", "??"));
  test_regex_reader("\\-", sv(1, "-"), sv(2, "b", "--"));
  test_regex_reader("a+", sv(3, "a", "aa", "aaa"), sv(2, "", "??"));
  test_regex_reader("a*", sv(4, "", "a", "aa", "aaa"), sv(1, "??", "b"));
  test_regex_reader("foo|bar", sv(2, "foo", "bar"), sv(2, "??", "foobar"));
  test_regex_reader("[a-zA-Z0-9]",
      sv(9, "a", "q", "z", "A", "Q", "Z", "0", "5", "9"),
      sv(4, "?", "_", "-", "+"));
  test_regex_reader("[_a-zA-Z][_a-zA-Z0-9]*",
      sv(3, "_soup", "All_The_Case", "l33t"),
      sv(3, "007", "fire|hose", "+1"));
  test_regex_reader("\t \n\r", sv(1, "\t \n\r"), sv(2, "abc", "  \n\r"));
  test_regex_reader("ba(na)+",
      sv(3, "bana", "banana", "bananana"),
      sv(2, "banan", "banane"));
  /* detect a C-style comment like this one */
  test_regex_reader("/\\*[^\\*]*\\*+([^/\\*][^\\*]*\\*+)*/",
      sv(1, "/**/", "/*\n *\n */", "/* /* /***/"),
      sv(1, "/*/"));
  /* <!-- detect an XML-style comment like this one -->
   * note: after reading the XML spec, it seems they don't allow
   *       comments to be this general (contain --), and furthermore it doesn't
   *       seem like the right approach to detect comments in the lexer
   *       because XML's character meanings change so much between comments,
   *       attribute values, and elsewhere.
   *       as such, the following is more of an academic exercise in how to generalize
   *       the C comment rule above, but don't expect to see it in Teuchos::xml */
  test_regex_reader("<!\\-\\-([^\\-]*\\-([^\\-]+\\-)*)\\-([^\\->]([^\\-]*\\-([^\\-]+\\-)*)\\-)*>",
      sv(2, "<!-- foo -->", "<!-- <!--\nfoo bar\n>-- -->"),
      sv(1, "<!-- --> -->"));
  /* floating-point literals */
  test_regex_reader("(0|([1-9][0-9]*))(\\.[0-9]*)?([eE]\\-?[1-9][0-9]*)?",
      sv(7, "1", "1.0", "1e6", "3.14159", "2.2e3", "0.0", "0.0001"), sv(2, "a", "-1"));
}

TEUCHOS_UNIT_TEST( Parser, xml_language ) {
  LanguagePtr lang = XML::ask_language();
  GrammarPtr grammar = make_grammar(*lang);
  make_lalr1_parser(grammar);
}

void test_xml_reader(std::string const& str) {
  Reader reader(XML::ask_reader_tables());
  any result;
  reader.read_string(result, str, "test_xml_reader");
}

TEUCHOS_UNIT_TEST( Parser, xml_reader ) {
  test_xml_reader("<the_tag100/>");
  test_xml_reader("<Parameter name=\"force\"/>");
  test_xml_reader("<Parameter name=\"force\"\ttype=\"double\" value=\"1.9\"/>");
  test_xml_reader("<ParameterList name=\"Physics Vars\">\n  <Parameter name=\"force\"\ttype=\"double\" value=\"1.9\"/>\n</ParameterList>");
  test_xml_reader("<P name=\"foo&quot;&#72;bar\"/>");
}

template <typename T>
static T& add(std::vector<T>& v) {
  v.push_back(T());
  return v.back();
}

void test_reader(ReaderTablesPtr tables, std::string const& str, std::string const& name) {
  Reader reader(tables);
  any result;
  reader.read_string(result, str, name);
}

TEUCHOS_UNIT_TEST( Parser, yaml_proxy_language ) {
  Language lang;
  Language::Productions& prods = lang.productions;
  Language::Tokens& toks = lang.tokens;
  add(toks)("NODENT", "]NODENT[");
  add(toks)("INDENT", "]INDENT[");
  add(toks)("DEDENT", "]DEDENT[");
  add(toks)("NEWLINE", "]NEWLINE[");
  add(toks)("WS", "[ \t]");
  add(toks)(":", ":");
  add(toks)(".", "\\.");
  add(toks)("-", "\\-");
  add(toks)("\"", "\"");
  add(toks)("'", "'");
  add(toks)("\\", "\\\\");
  add(toks)("|", "\\|");
  add(toks)("[", "\\[");
  add(toks)("]", "\\]");
  add(toks)("{", "{");
  add(toks)("}", "}");
  add(toks)(",", ",");
  add(toks)("%", "%");
  add(toks)("#", "#");
  add(toks)("!", "!");
  add(toks)("OTHERCHAR", "[^ \t:\\.\\-\"'\\\\\\|\\[\\]{},%#!\n\r]");
  add(prods)("doc") >> "top_items";
  add(prods)("top_items") >> "top_item";
  add(prods)("top_items") >> "top_items", "top_item";
  add(prods)("top_item") >> "%", "any*", "NEWLINE";
  add(prods)("top_item") >> "-", "-", "-", "NEWLINE", "comment*";
  add(prods)("top_item") >> ".", ".", ".", "NEWLINE";
  add(prods)("top_item") >> "bmap_item";
  add(prods)("bmap_items") >> "bmap_item", "comment*";
  add(prods)("bmap_items") >> "bmap_items", "bmap_item", "comment*";
  add(prods)("bmap_item") >> "scalar", ":", "WS*", "tag?", "scalar", "NEWLINE";
  add(prods)("bmap_item") >> "scalar", ":", "WS*", "bscalar";
  add(prods)("bmap_item") >> "scalar", ":", "WS*", "NEWLINE", "INDENT", "comment*", "bmap_items", "DEDENT";
  add(prods)("bmap_item") >> "scalar", ":", "WS*", "NEWLINE", "INDENT", "comment*", "bseq_items", "DEDENT";
  add(prods)("bmap_item") >> "scalar", ":", "WS*", "tag?", "fseq", "NEWLINE";
  add(prods)("bmap_item") >> "scalar", ":", "WS*", "tag?", "fmap", "NEWLINE";
  add(prods)("bseq_items") >> "bseq_item", "comment*";
  add(prods)("bseq_items") >> "bseq_items", "bseq_item", "comment*";
  add(prods)("bseq_item") >> "-", "WS+", "tag?", "scalar", "NEWLINE";
  add(prods)("bseq_item") >> "-", "WS+", "bscalar";
  add(prods)("bseq_item") >> "-", "WS+", "tag?", "fseq", "NEWLINE";
  add(prods)("bseq_item") >> "-", "WS+", "tag?", "fmap", "NEWLINE";
  add(prods)("bseq_item") >> "-", "WS+", "NEWLINE", "INDENT", "comment*", "bseq_items", "DEDENT";
  add(prods)("bseq_item") >> "-", "NEWLINE", "INDENT", "comment*", "bseq_items", "DEDENT";
  add(prods)("bseq_item") >> "-", "WS+", "NEWLINE", "INDENT", "comment*", "bmap_items", "DEDENT";
  add(prods)("bseq_item") >> "-", "NEWLINE", "INDENT", "comment*", "bmap_items", "DEDENT";
  add(prods)("fseq") >> "[", "WS*", "fseq_items", "]", "WS*";
  add(prods)("fmap") >> "{", "WS*", "fmap_items", "}", "WS*";
  add(prods)("fseq") >> "[", "WS*", "]", "WS*";
  add(prods)("fmap") >> "{", "WS*", "}", "WS*";
  add(prods)("fseq_items") >> "fseq_item";
  add(prods)("fseq_items") >> "fseq_items", ",", "WS*", "fseq_item";
  add(prods)("fseq_item") >> "tag?", "scalar";
  add(prods)("fseq_item") >> "tag?", "fseq";
  add(prods)("fseq_item") >> "tag?", "fmap";
  add(prods)("fmap_items") >> "fmap_item";
  add(prods)("fmap_items") >> "fmap_items", ",", "WS*", "fmap_item";
  add(prods)("fmap_item") >> "scalar", ":", "WS*", "tag?", "scalar";
  add(prods)("fmap_item") >> "scalar", ":", "WS*", "tag?", "fseq";
  add(prods)("fmap_item") >> "scalar", ":", "WS*", "tag?", "fmap";
  add(prods)("scalar") >> "OTHERCHAR", "rest*";
  add(prods)("scalar") >> ".", "OTHERCHAR", "rest*";
  add(prods)("scalar") >> "-", "OTHERCHAR", "rest*";
  add(prods)("scalar") >> "\"", "dquoted*", "descape*", "\"";
  add(prods)("scalar") >> "'", "squoted*", "sescape*", "'";
  add(prods)("comment*");
  add(prods)("comment*") >> "comment*", "#", "any*", "NEWLINE";
  add(prods)("tag?");
  add(prods)("tag?") >> "!", "!", "OTHERCHAR+", "WS+";
  add(prods)("bscalar") >> "|", "WS*", "NEWLINE", "INDENT", "bscalar_items", "DEDENT";
  add(prods)("bscalar_items") >> "bscalar_item";
  add(prods)("bscalar_items") >> "bscalar_items", "bscalar_item";
  add(prods)("bscalar_item") >> "any*", "NEWLINE";
  add(prods)("bscalar_item") >> "INDENT", "bscalar_items", "DEDENT";
  add(prods)("dquoted*");
  add(prods)("dquoted*") >> "dquoted*", "dquoted";
  add(prods)("squoted*");
  add(prods)("squoted*") >> "squoted*", "squoted";
  add(prods)("any*");
  add(prods)("any*") >> "any*", "any";
  add(prods)("descape*");
  add(prods)("descape*") >> "descape*", "descape";
  add(prods)("descape") >> "\\", "descaped", "dquoted*";
  add(prods)("sescape*");
  add(prods)("sescape*") >> "sescape*", "sescape";
  add(prods)("sescape") >> "'", "'", "squoted*";
  add(prods)("rest*");
  add(prods)("rest*") >> "rest*", "rest";
  add(prods)("OTHERCHAR+") >> "OTHERCHAR";
  add(prods)("OTHERCHAR+") >> "OTHERCHAR+", "OTHERCHAR";
  add(prods)("rest") >> "WS";
  add(prods)("rest") >> ".";
  add(prods)("rest") >> "-";
  add(prods)("rest") >> "OTHERCHAR";
  add(prods)("descaped") >> "\"";
  add(prods)("descaped") >> "\\";
  add(prods)("descaped") >> "dquoted";
  add(prods)("dquoted") >> "common";
  add(prods)("dquoted") >> "'";
  add(prods)("squoted") >> "common";
  add(prods)("squoted") >> "\"";
  add(prods)("squoted") >> "\\";
  add(prods)("any") >> "common";
  add(prods)("any") >> "\"";
  add(prods)("any") >> "'";
  add(prods)("any") >> "\\";
  add(prods)("common") >> "WS";
  add(prods)("common") >> ":";
  add(prods)("common") >> ".";
  add(prods)("common") >> "-";
  add(prods)("common") >> "|";
  add(prods)("common") >> "[";
  add(prods)("common") >> "]";
  add(prods)("common") >> "{";
  add(prods)("common") >> "}";
  add(prods)("common") >> ",";
  add(prods)("common") >> "%";
  add(prods)("common") >> "#";
  add(prods)("common") >> "!";
  add(prods)("common") >> "OTHERCHAR";
  add(prods)("WS*");
  add(prods)("WS*") >> "WS*", "WS";
  add(prods)("WS+") >> "WS";
  add(prods)("WS+") >> "WS+", "WS";
  GrammarPtr grammar = make_grammar(lang);
  ReaderTablesPtr tables = make_reader_tables(lang);
  test_reader(tables,
      "%YAML 1.2\n"
      "---\n"
      "#top comment always wins\n"
      "a: \n"
      "  b:\n"
      "   - one\n"
      "   - \"Lt. Pete \\\"Maverick\\\" Mitchell\"\n"
      "   -\n"
      "     - \n"
      "       - tres\n"
      "       - [tre, es]\n"
      "       - {tre: ees}\n"
      "       - \n"
      "         #why?\n"
      "         because: we can.\n"
      "     - treees\n"
      "  c: 'single quoting is ''fun'''\n"
      "  todo: [parse yaml, ???, profit]\n"
      "  organisms: { plants: cocoa, animals: [birds, {next: fish}] }\n"
      "e:\n"
      "  that code: |\n"
      "    switch (a[i] - b[i]) {\n"
      "      case 1: return '\\'';\n"
      "      case 2: return '\\\"';\n"
      "    }\n"
      "  g: .125\n"
      "  i: -6.022e-23\n"
      "...\n"
      , "1");
  test_reader(tables,
      "---\n"
      "lvl1-1:\n"
      "  lvl2-1: a\n"
      "lvl1-2:\n"
      "  lvl2-2: b\n"
      "...\n",
      "2");
  test_reader(tables,"---\n"
      "foo:bar\n"
      "...\n",
      "3");
  test_reader(tables,"%YAML 1.2\n"
      "---\n"
      "foo:bar\n"
      "...\n",
      "4");
  test_reader(tables,"---\n"
      "foo:bar\n"
      "far:boo\n"
      "...\n",
      "5");
  test_reader(tables,"---\n"
      "foo:\n"
      "  bar:42\n"
      "  baz:  100\n"
      "...\n",
      "6");
  test_reader(tables,
      "---\n"
      "\"Don Knuth\":bar\n"
      "...\n",
      "7");
  test_reader(tables,
      "---\n"
      "'never say never':true\n"
      "...\n",
      "8");
  test_reader(tables,
      "---\n"
      "'never say ''never''':true\n"
      "...\n",
      "9");
  test_reader(tables,
      "---\n"
      "1:\n"
      " - a\n"
      " - b\n"
      "...\n",
      "11");
  test_reader(tables,
      "---\n"
      "a: {1: 1, 2: 4, 3: 9}\n"
      "...\n",
      "12");
  test_reader(tables,
      "---\n"
      "a: [1, 2, 3]\n"
      "...\n",
      "13");
  test_reader(tables,"---\n"
      "a: {1: [0, 1], 2: [0, 1, 2]}\n"
      "...\n",
      "14");
  test_reader(tables,
      "---\n"
      "assocs: [[bottom, 1], [top, 42]]\n"
      "...\n",
      "15");
  test_reader(tables,
      "---\n"
      "assocs: [ [ bottom , 1 ] , [ top , 42 ] ]\n"
      "...\n",
      "16");
  test_reader(tables,
      "---\n"
      "pressure: -1.9e-6\n"
      "volume: !!double 0.7e+10\n"
      "...\n",
      "17");
  test_reader(tables,
      "---\n"
      "ANONYMOUS:\n"
      "  empty: {}\n"
      "  block with empty lines: |\n"
      "\n"
      "    check one two\n"
      "\n"
      "      more indented\n"
      "...\n",
      "18");
  test_reader(tables,
      "---\r\n"
      "ANONYMOUS:\r\n"
      "  empty: { }\r\n"
      "  block with empty lines: |\r\n"
      "\r\n"
      "    check one two\r\n"
      "\r\n"
      "      more indented\r\n"
      "...\r\n",
      "19");
  test_reader(tables,
      "---\n"
      "Lord of the Rings:\n"
      "  - Sauron\n"
      "...\n",
      "20");
  test_reader(tables,
      "---\n"
      "#top comment\n"
      "top entry: \n"
      "  sub-entry: twelve\n"
      "  # long comment\n"
      "  # about sub-entry2\n"
      "  sub-entry2: green\n"
      "...\n",
      "21");
}

#if 0
TEUCHOS_UNIT_TEST( Parser, yaml_language ) {
  LanguagePtr lang = yaml::ask_language();
  GrammarPtr grammar = make_grammar(*lang);
  make_lalr1_parser(grammar, true);
}

void test_yaml_reader(std::string const& str, std::string const& name) {
  Reader reader(yaml::ask_reader_tables(), std::cout);
  any result;
  reader.read_string(result, str, name);
}

TEUCHOS_UNIT_TEST( Parser, yaml_reader ) {
  test_yaml_reader(
      "---\n"
      "lvl1-1:\n"
      "  lvl2-1: a\n"
      "lvl1-2:\n"
      "  lvl2-2: b\n"
      "...\n");
  test_yaml_reader("---\n"
      "foo:bar\n"
      "...\n"
      "");
  test_yaml_reader("%YAML 1.2\n"
      "---\n"
      "foo:bar\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "foo:bar\n"
      "far:boo\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "foo:\n"
      "  bar:42\n"
      "  baz:  100\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      " foo:bar\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "\"Don Knuth\":bar\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "\"Don \\\"Maverick\\\" Knuth\":bar\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "'never say never':true\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "'never say ''never''':true\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      " - foo\n"
      " - bar\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "1:\n"
      " - a\n"
      " - b\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "a: {1: 1, 2: 4, 3: 9}\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "a: [1, 2, 3]\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "a: {1: [0, 1], 2: [0, 1, 2]}\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "assocs: [[bottom, 1], [top, 42]]\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "assocs: [ [ bottom , 1 ] , [ top , 42 ] ]\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "pressure: -1.9e-6\n"
      "volume: 0.7e+10\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "ANONYMOUS:\n"
      "  empty: {}\n"
      "  non-empty: 1.5\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "ANONYMOUS:\n"
      "  empty: { }\n"
      "  non-empty: 1.5\n"
      "...\n"
      "");
  test_yaml_reader("---\n"
      "Lord of the Rings:\n"
      "  - Sauron\n"
      "...\n"
      "");
  test_yaml_reader(
      "---\n"
      "#top comment\n"
      "top entry: \n"
      "  sub-entry: twelve\n"
      ""
      "  # long comment\n"
      "  # about sub-entry2\n"
      "  sub-entry2: green\n"
      "...\n"
      "");
}
#endif

TEUCHOS_UNIT_TEST( Parser, mathexpr_language ) {
  LanguagePtr lang = MathExpr::ask_language();
  GrammarPtr grammar = make_grammar(*lang);
  make_lalr1_parser(grammar);
}

void test_mathexpr_reader(std::string const& str) {
  Reader reader(MathExpr::ask_reader_tables());
  any result;
  reader.read_string(result, str, "test_mathexpr_reader");
}

TEUCHOS_UNIT_TEST( Parser, mathexpr_reader ) {
  test_mathexpr_reader("1 + 1");
  test_mathexpr_reader("concat(one, two, three)");
  test_mathexpr_reader("x < 0.5 ? 1 : 0");
  test_mathexpr_reader("sqrt(x^2 + y^2) < 0.5 ? 1 : 0");
  test_mathexpr_reader("1.22+30.*exp(-((x^2 + (y-180)^2))/(2.*(2.2)^2))");
  test_mathexpr_reader("1.23e5+8.07e10*exp(-((x^2 + (y-180)^2))/(2.*(2.2)^2))");
  test_mathexpr_reader("---16");
  test_mathexpr_reader("((1 < 2) && (2 < 1)) ? 42 : 9");
}

} // anonymous namespace
