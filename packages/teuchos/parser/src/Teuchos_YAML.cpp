#include "Teuchos_YAML.hpp"

namespace Teuchos {
namespace yaml {

Language make_language() {
  Language out;
  Language::Productions& prods = out.productions;
  Language::Tokens& toks = out.tokens;
  prods.resize(NPRODS);
  toks.resize(NTOKS);
  prods[PROD_DOC]("doc") >> "EQDENT", "prolog", "toplevel", "epilog";
  prods[PROD_PROLOG]("prolog") >> "directive?", "DOC_START";
  prods[PROD_NO_DIRECT]("directive?");
  prods[PROD_ONE_DIRECT]("directive?") >> "directive";
  prods[PROD_DIRECT]("directive") >> "DIRECTIVE", "EQDENT";
  prods[PROD_DOC_START]("DOC_START") >> "-", "-", "-";
  prods[PROD_DIRECTIVE]("DIRECTIVE") >> "%", "CHAR*";
  prods[PROD_EPILOG]("epilog") >> "doc_end";
  prods[PROD_DOC_END]("doc_end") >> ".", ".", ".", "EQDENT";
  prods[PROD_TOP_BMAP]("toplevel") >> "EQDENT", "block_map_items", "EQDENT?";
  prods[PROD_TOP_BSEQ]("toplevel") >> "EQDENT", "block_sequence_items", "EQDENT?";
  prods[PROD_TOP_BLOCK]("toplevel") >> "block_collective";
  prods[PROD_BMAP]("block_collective") >> "INDENT", "block_map_items", "DEDENT";
  prods[PROD_BSEQ]("block_collective") >> "INDENT", "block_sequence_items", "DEDENT";
  prods[PROD_BMAP_ONE_ITEM]("block_map_items") >> "block_map_item";
  prods[PROD_BMAP_ITEMS]("block_map_items") >> "block_map_items", "EQDENT", "block_map_item";
  prods[PROD_BSEQ_ITEM]("block_sequence_items") >> "block_sequence_item";
  prods[PROD_BSEQ_ITEMS]("block_sequence_items") >> "block_sequence_items", "EQDENT", "block_sequence_item";
  prods[PROD_BSEQ_SCALAR]("block_sequence_item") >> "comments", "BLOCK_SEQ", "scalar", "S?";
  prods[PROD_BMAP_ITEM]("block_map_item") >> "comments", "scalar", "S?", ":", "S?", "block_map_value";
  prods[PROD_BMAP_SCALAR]("block_map_value") >> "scalar", "S?";
  prods[PROD_BMAP_BLOCK]("block_map_value") >> "block_collective";
  prods[PROD_BMAP_FLOW]("block_map_value") >> "flow_collective";
  prods[PROD_FSEQ_EMPTY]("flow_collective") >> "[", "S?", "]";
  prods[PROD_FMAP_EMPTY]("flow_collective") >> "{", "S?", "}";
  prods[PROD_FSEQ]("flow_collective") >> "[", "S?", "flow_sequence_items", "]";
  prods[PROD_FMAP]("flow_collective") >> "{", "S?", "flow_map_items", "}";
  prods[PROD_FSEQ_ITEM]("flow_sequence_items") >> "flow_sequence_item";
  prods[PROD_FSEQ_ITEMS]("flow_sequence_items") >> "flow_sequence_items", ",", "S?", "flow_sequence_item";
  prods[PROD_FSEQ_SCALAR]("flow_sequence_item") >> "scalar", "S?";
  prods[PROD_FSEQ_FLOW]("flow_sequence_item") >> "flow_collective", "S?";
  prods[PROD_FMAP_ONE_ITEM]("flow_map_items") >> "flow_map_item";
  prods[PROD_FMAP_ITEMS]("flow_map_items") >> "flow_map_items", ",", "S?", "flow_map_item";
  prods[PROD_FMAP_ITEM]("flow_map_item") >> "scalar", "S?", ":", "S?", "flow_map_value";
  prods[PROD_FMAP_SCALAR]("flow_map_value") >> "scalar", "S?";
  prods[PROD_FMAP_FLOW]("flow_map_value") >> "flow_collective", "S?";
  prods[PROD_NO_SPACE]("S?");
  prods[PROD_SPACE]("S?") >> "S";
  prods[PROD_NO_COMMENTS]("comments");
  prods[PROD_COMMENTS]("comments") >> "comments", "COMMENT", "EQDENT";
  prods[PROD_NO_EQDENT]("EQDENT?");
  prods[PROD_EQDENT]("EQDENT?") >> "EQDENT";
  prods[PROD_RAW]("scalar") >> "RAW_FIRST", "RAW_REST";
  prods[PROD_DQUOTED]("scalar") >> "\"", "DQUOTED_CHAR*", "\"";
  prods[PROD_SQUOTED]("scalar") >> "'", "SQUOTED_CHAR*", "SQUOTED_CONTD*", "'";
  prods[PROD_BLOCK_SEQ]("BLOCK_SEQ") >> "-", "S";
  prods[PROD_COMMENT]("COMMENT") >> "#", "CHAR*";
  prods[PROD_RAW_FIRST_DASH]("RAW_FIRST") >> "-", "RAW_SECOND";
  prods[PROD_RAW_SECOND_OTHER]("RAW_SECOND") >> "OTHERCHAR";
  prods[PROD_RAW_SECOND_DOT]("RAW_SECOND") >> ".";
  prods[PROD_RAW_FIRST_OTHER]("RAW_FIRST") >> "OTHERCHAR";
  prods[PROD_RAW_FIRST_DOT]("RAW_FIRST") >> ".";
  prods[PROD_RAW_NO_REST]("RAW_REST");
  prods[PROD_RAW_REST]("RAW_REST") >> "RAW_MIDDLE*", "RAW_END";
  prods[PROD_NO_RAW_MIDDLE]("RAW_MIDDLE*");
  prods[PROD_NEXT_RAW_MIDDLE]("RAW_MIDDLE*") >> "RAW_MIDDLE*", "RAW_MIDDLE";
  prods[PROD_RAW_MIDDLE_OTHER]("RAW_MIDDLE") >> "OTHERCHAR";
  prods[PROD_RAW_MIDDLE_DASH]("RAW_MIDDLE") >> "-";
  prods[PROD_RAW_MIDDLE_DOT]("RAW_MIDDLE") >> ".";
  prods[PROD_RAW_MIDDLE_WS]("RAW_MIDDLE") >> "WS";
  prods[PROD_RAW_END_OTHER]("RAW_END") >> "OTHERCHAR";
  prods[PROD_RAW_END_DASH]("RAW_END") >> "-";
  prods[PROD_RAW_END_DOT]("RAW_END") >> ".";
  prods[PROD_NO_DQUOTED_CHAR]("DQUOTED_CHAR*");
  prods[PROD_NEXT_DQUOTED_CHAR]("DQUOTED_CHAR*") >> "DQUOTED_CHAR*", "DQUOTED_CHAR";
  prods[PROD_DQUOTED_CHAR_QUOTED]("DQUOTED_CHAR") >> "QUOTED_CHAR";
  prods[PROD_DQUOTED_CHAR_SQUOT]("DQUOTED_CHAR") >> "'";
  prods[PROD_DQUOTED_CHAR_ESCAPE]("DQUOTED_CHAR") >> "\\", "DQUOTED_ESCAPED";
  prods[PROD_DQUOTED_ESCAPED_CHAR]("DQUOTED_ESCAPED") >> "DQUOTED_CHAR";
  prods[PROD_DQUOTED_ESCAPED_BSLASH]("DQUOTED_ESCAPED") >> "\\";
  prods[PROD_NO_SQUOTED_CHAR]("SQUOTED_CHAR*");
  prods[PROD_NEXT_SQUOTED_CHAR]("SQUOTED_CHAR*") >> "SQUOTED_CHAR*", "SQUOTED_CHAR";
  prods[PROD_NO_SQUOTED_CONTD]("SQUOTED_CONTD*");
  prods[PROD_NEXT_SQUOTED_CONTD]("SQUOTED_CONTD*") >> "SQUOTED_CONTD*", "SQUOTED_CONTD";
  prods[PROD_SQUOTED_CONTD]("SQUOTED_CONTD") >> "'", "'", "SQUOTED_CHAR*";
  prods[PROD_SQUOTED_CHAR_QUOTED]("SQUOTED_CHAR") >> "QUOTED_CHAR";
  prods[PROD_SQUOTED_CHAR_DQUOT]("SQUOTED_CHAR") >> "\"";
  prods[PROD_SQUOTED_CHAR_BSLASH]("SQUOTED_CHAR") >> "\\";
  prods[PROD_QUOTED_CHAR_OTHER]("QUOTED_CHAR") >> "OTHERCHAR";
  prods[PROD_QUOTED_CHAR_WS]("QUOTED_CHAR") >> "WS";
  prods[PROD_QUOTED_CHAR_COLON]("QUOTED_CHAR") >> ":";
  prods[PROD_QUOTED_CHAR_COMMA]("QUOTED_CHAR") >> ",";
  prods[PROD_QUOTED_CHAR_LSQUARE]("QUOTED_CHAR") >> "[";
  prods[PROD_QUOTED_CHAR_RSQUARE]("QUOTED_CHAR") >> "]";
  prods[PROD_QUOTED_CHAR_LCURLY]("QUOTED_CHAR") >> "{";
  prods[PROD_QUOTED_CHAR_RCURLY]("QUOTED_CHAR") >> "}";
  prods[PROD_QUOTED_CHAR_POUND]("QUOTED_CHAR") >> "#";
  prods[PROD_QUOTED_CHAR_DASH]("QUOTED_CHAR") >> "-";
  prods[PROD_QUOTED_CHAR_DOT]("QUOTED_CHAR") >> ".";
  prods[PROD_QUOTED_CHAR_PERCENT]("QUOTED_CHAR") >> "%";
  prods[PROD_QUOTED_CHAR_PIPE]("QUOTED_CHAR") >> "|";
  prods[PROD_S_FIRST]("S") >> "WS";
  prods[PROD_S_NEXT]("S") >> "S", "WS";
  prods[PROD_NO_CHAR]("CHAR*");
  prods[PROD_NEXT_CHAR]("CHAR*") >> "CHAR*", "CHAR";
  prods[PROD_CHAR_WS]("CHAR") >> "WS";
  prods[PROD_CHAR_COLON]("CHAR") >> ":";
  prods[PROD_CHAR_COMMA]("CHAR") >> ",";
  prods[PROD_CHAR_LSQUARE]("CHAR") >> "[";
  prods[PROD_CHAR_RSQUARE]("CHAR") >> "]";
  prods[PROD_CHAR_LCURLY]("CHAR") >> "{";
  prods[PROD_CHAR_RCURLY]("CHAR") >> "}";
  prods[PROD_CHAR_POUND]("CHAR") >> "#";
  prods[PROD_CHAR_DASH]("CHAR") >> "-";
  prods[PROD_CHAR_DOT]("CHAR") >> ".";
  prods[PROD_CHAR_PERCENT]("CHAR") >> "%";
  prods[PROD_CHAR_SQUOT]("CHAR") >> "'";
  prods[PROD_CHAR_DQUOT]("CHAR") >> "\"";
  prods[PROD_CHAR_PIPE]("CHAR") >> "|";
  prods[PROD_CHAR_BSLASH]("CHAR") >> "\\";
  prods[PROD_CHAR_OTHER]("CHAR") >> "OTHERCHAR";
  toks[TOK_NODENT]("NODENT", "]NODENT[");
  toks[TOK_INDENT]("INDENT", "]INDENT[");
  toks[TOK_DEDENT]("DEDENT", "]DEDENT[");
  toks[TOK_EQDENT]("EQDENT", "]EQDENT[");
  toks[TOK_SPACE]("WS", "[ \t]");
  toks[TOK_COLON](":", ":");
  toks[TOK_COMMA](",", ",");
  toks[TOK_LSQUARE]("[", "\\[");
  toks[TOK_RSQUARE]("]", "\\]");
  toks[TOK_LCURLY]("{", "{");
  toks[TOK_RCURLY]("}", "}");
  toks[TOK_POUND]("#", "#");
  toks[TOK_DASH]("-", "\\-");
  toks[TOK_DOT](".", "\\.");
  toks[TOK_PERCENT]("%", "%");
  toks[TOK_SQUOT]("'", "'");
  toks[TOK_DQUOT]("\"", "\"");
  toks[TOK_PIPE]("|", "\\|");
  toks[TOK_BSLASH]("\\", "\\\\");
  toks[TOK_OTHERCHAR]("OTHERCHAR", "[^ \t:,\\[\\]{}#\\-\\.%'\"\\|\\\\\n\r]");
  return out;
}

LanguagePtr ask_language() {
  static LanguagePtr ptr;
  if (ptr.strong_count() == 0) {
    ptr.reset(new Language(make_language()));
  }
  return ptr;
}

ReaderTablesPtr ask_reader_tables() {
  static ReaderTablesPtr ptr;
  if (ptr.strong_count() == 0) {
    ptr = make_reader_tables(*(yaml::ask_language()));
  }
  return ptr;
}

}  // end namespace yaml
}  // end namespace Teuchos
