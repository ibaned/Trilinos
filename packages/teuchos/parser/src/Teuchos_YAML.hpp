#ifndef TEUCHOS_YAML_HPP
#define TEUCHOS_YAML_HPP

#include <Teuchos_Language.hpp>
#include <Teuchos_ReaderTables.hpp>

namespace Teuchos {
namespace YAML {

enum {
  PROD_DOC,
  PROD_TOP_FIRST,
  PROD_TOP_NEXT,
  PROD_TOP_DIRECT,
  PROD_TOP_BEGIN,
  PROD_TOP_END,
  PROD_TOP_BMAP,
  PROD_BMAP_FIRST,
  PROD_BMAP_NEXT,
  PROD_BMAP_SCALAR,
  PROD_BMAP_BSCALAR,
  PROD_BMAP_BMAP,
  PROD_BMAP_BSEQ,
  PROD_BMAP_FMAP,
  PROD_BMAP_FSEQ,
  PROD_BSEQ_FIRST,
  PROD_BSEQ_NEXT,
  PROD_BSEQ_SCALAR,
  PROD_BSEQ_BSCALAR,
  PROD_BSEQ_BMAP,
  PROD_BSEQ_BMAP_TRAIL,
  PROD_BSEQ_BSEQ,
  PROD_BSEQ_BSEQ_TRAIL,
  PROD_BSEQ_FMAP,
  PROD_BSEQ_FSEQ,
  PROD_FMAP,
  PROD_FMAP_EMPTY,
  PROD_FMAP_FIRST,
  PROD_FMAP_NEXT,
  PROD_FMAP_SCALAR,
  PROD_FMAP_FMAP,
  PROD_FMAP_FSEQ,
  PROD_FSEQ,
  PROD_FSEQ_EMPTY,
  PROD_FSEQ_FIRST,
  PROD_FSEQ_NEXT,
  PROD_FSEQ_SCALAR,
  PROD_FSEQ_FMAP,
  PROD_FSEQ_FSEQ,
  PROD_SCALAR_NORMAL,
  PROD_SCALAR_DOT,
  PROD_SCALAR_DASH,
  PROD_SCALAR_DQUOTED,
  PROD_SCALAR_SQUOTED,
  PROD_COMMENT_EMPTY,
  PROD_COMMENT_NEXT,
  PROD_TAG_EMPTY,
  PROD_TAG,
  PROD_BSCALAR,
  PROD_BSCALAR_FIRST,
  PROD_BSCALAR_NEXT,
  PROD_BSCALAR_LINE,
  PROD_BSCALAR_INDENT,
  PROD_PIPE_NORMAL,
  PROD_PIPE_DASH,
  PROD_DQUOTED_EMPTY,
  PROD_DQUOTED_NEXT,
  PROD_SQUOTED_EMPTY,
  PROD_SQUOTED_NEXT,
  PROD_ANY_EMPTY,
  PROD_ANY_NEXT,
  PROD_DESCAPE_EMPTY,
  PROD_DESCAPE_NEXT,
  PROD_DESCAPE,
  PROD_SESCAPE_EMPTY,
  PROD_SESCAPE_NEXT,
  PROD_SESCAPE,
  PROD_REST_EMPTY,
  PROD_REST_NEXT,
  PROD_OTHER_FIRST,
  PROD_OTHER_NEXT,
  PROD_REST_SPACE,
  PROD_REST_DOT,
  PROD_REST_DASH,
  PROD_REST_OTHER,
  PROD_DESCAPED_DQUOT,
  PROD_DESCAPED_SLASH,
  PROD_DESCAPED_DQUOTED,
  PROD_DQUOTED_COMMON,
  PROD_DQUOTED_SQUOT,
  PROD_SQUOTED_COMMON,
  PROD_SQUOTED_DQUOT,
  PROD_SQUOTED_SLASH,
  PROD_ANY_COMMON,
  PROD_ANY_DQUOT,
  PROD_ANY_SQUOT,
  PROD_ANY_SLASH,
  PROD_COMMON_SPACE,
  PROD_COMMON_COLON,
  PROD_COMMON_DOT,
  PROD_COMMON_DASH,
  PROD_COMMON_PIPE,
  PROD_COMMON_LSQUARE,
  PROD_COMMON_RSQUARE,
  PROD_COMMON_LCURLY,
  PROD_COMMON_RCURLY,
  PROD_COMMON_COMMA,
  PROD_COMMON_PERCENT,
  PROD_COMMON_POUND,
  PROD_COMMON_EXCL,
  PROD_COMMON_OTHER,
  PROD_SPACE_STAR_EMPTY,
  PROD_SPACE_STAR_NEXT,
  PROD_SPACE_PLUS_FIRST,
  PROD_SPACE_PLUS_NEXT
};

enum { NPRODS = PROD_SPACE_PLUS_NEXT + 1 };

enum {
  TOK_NEWLINE,
  TOK_INDENT,
  TOK_DEDENT,
  TOK_SPACE,
  TOK_COLON,
  TOK_DOT,
  TOK_DASH,
  TOK_DQUOT,
  TOK_SQUOT,
  TOK_SLASH,
  TOK_PIPE,
  TOK_LSQUARE,
  TOK_RSQUARE,
  TOK_LCURLY,
  TOK_RCURLY,
  TOK_COMMA,
  TOK_PERCENT,
  TOK_POUND,
  TOK_EXCL,
  TOK_OTHER
};

enum { NTOKS = TOK_OTHER + 1 };

Language make_language();
LanguagePtr ask_language();
ReaderTablesPtr ask_reader_tables();

}  // end namespace yaml
}  // end namespace Teuchos

#endif
