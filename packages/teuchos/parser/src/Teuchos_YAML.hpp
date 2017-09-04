#ifndef TEUCHOS_YAML_HPP
#define TEUCHOS_YAML_HPP

#include <Teuchos_Language.hpp>
#include <Teuchos_ReaderTables.hpp>

namespace Teuchos {
namespace yaml {

enum {
  PROD_DOC,
  PROD_PROLOG,
  PROD_NO_DIRECT,
  PROD_ONE_DIRECT,
  PROD_DIRECT,
  PROD_DOC_START,
  PROD_DIRECTIVE,
  PROD_EPILOG,
  PROD_DOC_END,
  PROD_TOP_BMAP,
  PROD_TOP_BSEQ,
  PROD_TOP_BLOCK,
  PROD_BMAP,
  PROD_BSEQ,
  PROD_BMAP_ONE_ITEM,
  PROD_BMAP_ITEMS,
  PROD_BSEQ_ITEM,
  PROD_BSEQ_ITEMS,
  PROD_BSEQ_SCALAR,
  PROD_BMAP_ITEM,
  PROD_BMAP_SCALAR,
  PROD_BMAP_BLOCK,
  PROD_BMAP_FLOW,
  PROD_FSEQ_EMPTY,
  PROD_FMAP_EMPTY,
  PROD_FSEQ,
  PROD_FMAP,
  PROD_FSEQ_ITEM,
  PROD_FSEQ_ITEMS,
  PROD_FSEQ_SCALAR,
  PROD_FSEQ_FLOW,
  PROD_FMAP_ONE_ITEM,
  PROD_FMAP_ITEMS,
  PROD_FMAP_ITEM,
  PROD_FMAP_SCALAR,
  PROD_FMAP_FLOW,
  PROD_NO_SPACE,
  PROD_SPACE,
  PROD_NO_COMMENTS,
  PROD_COMMENTS,
  PROD_NO_EQDENT,
  PROD_EQDENT,
  PROD_RAW,
  PROD_DQUOTED,
  PROD_SQUOTED,
  PROD_BLOCK_SEQ,
  PROD_COMMENT,
  PROD_RAW_FIRST_DASH,
  PROD_RAW_SECOND_OTHER,
  PROD_RAW_SECOND_DOT,
  PROD_RAW_FIRST_OTHER,
  PROD_RAW_FIRST_DOT,
  PROD_RAW_NO_REST,
  PROD_RAW_REST,
  PROD_RAW_REST_OTHER,
  PROD_RAW_REST_DOT,
  PROD_RAW_REST_WS,
  PROD_NO_DQUOTED_CHAR,
  PROD_NEXT_DQUOTED_CHAR,
  PROD_DQUOTED_CHAR_QUOTED,
  PROD_DQUOTED_CHAR_SQUOT,
  PROD_DQUOTED_CHAR_ESCAPE,
  PROD_DQUOTED_ESCAPED_CHAR,
  PROD_DQUOTED_ESCAPED_BSLASH,
  PROD_NO_SQUOTED_CHAR,
  PROD_NEXT_SQUOTED_CHAR,
  PROD_NO_SQUOTED_CONTD,
  PROD_NEXT_SQUOTED_CONTD,
  PROD_SQUOTED_CONTD,
  PROD_SQUOTED_CHAR_QUOTED,
  PROD_SQUOTED_CHAR_DQUOT,
  PROD_SQUOTED_CHAR_BSLASH,
  PROD_QUOTED_CHAR_OTHER,
  PROD_QUOTED_CHAR_WS,
  PROD_QUOTED_CHAR_COLON,
  PROD_QUOTED_CHAR_COMMA,
  PROD_QUOTED_CHAR_LSQUARE,
  PROD_QUOTED_CHAR_RSQUARE,
  PROD_QUOTED_CHAR_LCURLY,
  PROD_QUOTED_CHAR_RCURLY,
  PROD_QUOTED_CHAR_POUND,
  PROD_QUOTED_CHAR_DASH,
  PROD_QUOTED_CHAR_DOT,
  PROD_QUOTED_CHAR_PERCENT,
  PROD_QUOTED_CHAR_PIPE,
  PROD_S_FIRST,
  PROD_S_NEXT,
  PROD_NO_CHAR,
  PROD_NEXT_CHAR,
  PROD_CHAR_WS,
  PROD_CHAR_COLON,
  PROD_CHAR_COMMA,
  PROD_CHAR_LSQUARE,
  PROD_CHAR_RSQUARE,
  PROD_CHAR_LCURLY,
  PROD_CHAR_RCURLY,
  PROD_CHAR_POUND,
  PROD_CHAR_DASH,
  PROD_CHAR_DOT,
  PROD_CHAR_PERCENT,
  PROD_CHAR_SQUOT,
  PROD_CHAR_DQUOT,
  PROD_CHAR_PIPE,
  PROD_CHAR_BSLASH,
  PROD_CHAR_OTHER
};

enum { NPRODS = PROD_CHAR_OTHER + 1 };

enum {
  TOK_NODENT,
  TOK_INDENT,
  TOK_DEDENT,
  TOK_EQDENT,
  TOK_SPACE,
  TOK_COLON,
  TOK_COMMA,
  TOK_LSQUARE,
  TOK_RSQUARE,
  TOK_LCURLY,
  TOK_RCURLY,
  TOK_POUND,
  TOK_DASH,
  TOK_DOT,
  TOK_PERCENT,
  TOK_SQUOT,
  TOK_DQUOT,
  TOK_PIPE,
  TOK_BSLASH,
  TOK_OTHERCHAR
};

enum { NTOKS = TOK_OTHERCHAR + 1 };

Language make_language();
LanguagePtr ask_language();
ReaderTablesPtr ask_reader_tables();

}  // end namespace yaml
}  // end namespace Teuchos

#endif
