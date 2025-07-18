%option c++ prefix="udf_scanner_" outfile="udf_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <cstring>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "udf_parser.tab.h"

// output stream for building string literals
static std::ostringstream strlit;

// don't change this
#define yyerror LexerError
%}

%x X_COMMENT
%x X_STRING X_BACKSLASH X_ZERO
%x X_HEX_INT

%%
  yydebug=1; set_debug(1);

  /* ====================================================================== */
  /* ====[                          COMMENTS                          ]==== */
  /* ====================================================================== */

"//".*$               /* ignore the line */

"/*"                  yy_push_state(X_COMMENT);
<X_COMMENT>"/*"       yy_push_state(X_COMMENT);
<X_COMMENT>"*/"       yy_pop_state();
<X_COMMENT>.|"\n"     ; /* ignore the inside of comments */

  /* ====================================================================== */
  /* ====[                         DATA TYPES                         ]==== */
  /* ====================================================================== */

"int"                 return tTYPE_INT;
"real"                return tTYPE_REAL;
"string"              return tTYPE_STRING;
"void"                return tTYPE_VOID;
"tensor"              return tTYPE_TENSOR;
"ptr"                 return tTYPE_PTR;

  /* ====================================================================== */
  /* ====[                       GLOBAL SYMBOLS                       ]==== */
  /* ====================================================================== */

"public"              return tPUBLIC;
"forward"             return tFORWARD;
"auto"                return tAUTO;

  /* ====================================================================== */
  /* ====[                        INSTRUCTIONS                        ]==== */
  /* ====================================================================== */

"if"                  return tIF;
"elif"                return tELIF;
"else"                return tELSE;
"for"                 return tFOR;
"break"               return tBREAK;
"continue"            return tCONTINUE;
"return"              return tRETURN;
"unless"              return tUNLESS;
"iterate"             return tITERATE;
"for"                 return tFOR;
"using"               return tUSING;

  /* ====================================================================== */
  /* ====[                        EXPRESSIONS                         ]==== */
  /* ====================================================================== */

">="                  return tGE; 
"<="                  return tLE; 
"=="                  return tEQ; 
"!="                  return tNE; 
"&&"                  return tAND;
"||"                  return tOR;

[\[\](),?]            return *yytext;
[-+*/%~<>@?=]         return *yytext;
[#\[\](){};:!,.]      return *yytext;


"nullptr"             return tNULLPTR;
"sizeof"              return tSIZEOF;
"objects"             return tOBJECTS;

  /* ====================================================================== */
  /* ====[                        INPUT-OUTPUT                        ]==== */
  /* ====================================================================== */

"write"               return tWRITE; 
"writeln"             return tWRITELN;
"input"               return tINPUT; 

  /* ====================================================================== */
  /* ====[                          TENSORS                           ]==== */
  /* ====================================================================== */

"rank"                return tRANK;
"dims"                return tDIMS;
"dim"                 return tDIM;
"capacity"            return tCAPACITY;
"reshape"             return tRESHAPE;
"**"                  return tCONTRACTION;                                                        


  /* ====================================================================== */
  /* ====[                          INTEGERS                          ]==== */
  /* ====================================================================== */ 

"0x"                        yy_push_state(X_HEX_INT);
<X_HEX_INT>[[:xdigit:]]+    { 
                              yylval.i = strtoul(yytext, NULL, 16); yy_pop_state();  
                              if (yylval.i > 2147483647) {
                                std::cerr << "WARNING: overflow integer data on line "<< lineno() << std::endl; exit(1);
                              }
                              return tINTEGER;
                            }
<X_HEX_INT>.|"\n"           std::cerr << "WARNING: bad hexadecimal data on line "<< lineno() << std::endl; exit(1);

[[:digit:]]+                {

                              yylval.i = strtoul(yytext, NULL, 10); 
                              if (yylval.i > 2147483647) {
                                std::cerr << "WARNING: overflow integer data on line "<< lineno() << std::endl; exit(1);
                              }
                              return tINTEGER;
                            }

  /* ====================================================================== */
  /* ====[                           REALS                            ]==== */
  /* ====================================================================== */

([0-9]*\.[0-9]+|[0-9]+\.[0-9]*)([eE][-+]?[0-9]+)? { 
                                                    yylval.d = strtod(yytext, NULL); 
                                                    //FIXME: overflow double
                                                    //if (yylval.d > DBL_MAX) {
                                                    //  std::cerr << "WARNING: overflow double data on line "<< lineno() << std::endl; exit(1);
                                                    //}
                                                    return tREAL; 
                                                  }

([[:digit:]]+)([eE][-+]?[0-9]+)                   { yylval.d = strtod(yytext, NULL); return tREAL; }

  /* ====================================================================== */
  /* ====[                        IDENTIFIERS                         ]==== */
  /* ====================================================================== */

[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tID;

  /* ====================================================================== */
  /* ====[                          STRINGS                           ]==== */
  /* ====================================================================== */

"\""                              yy_push_state(X_STRING);
<X_STRING>\\                      yy_push_state(X_BACKSLASH);
<X_STRING>"\""                    {
                                    yylval.s = new std::string(strlit.str());
                                    strlit.str("");
                                    yy_pop_state();
                                    return tSTRING;
                                  }
<X_STRING>.                       strlit << *yytext;
<X_BACKSLASH>n                    strlit << '\n'; yy_pop_state();
<X_BACKSLASH>r                    strlit << '\r'; yy_pop_state();
<X_BACKSLASH>t                    strlit << '\t'; yy_pop_state();
<X_BACKSLASH>0                    yy_push_state(X_ZERO);
<X_BACKSLASH>\\                   strlit << '\\'; yy_pop_state();
<X_BACKSLASH>[[:xdigit:]]{1,2}    {
                                    strlit << (char)(unsigned char)strtoul(yytext, NULL, 16);
                                    yy_pop_state();
                                  }
<X_BACKSLASH>.                    strlit << *yytext; yy_pop_state(); 
<X_ZERO>"\""                      {
                                    yylval.s = new std::string(strlit.str());
                                    strlit.str("");
                                    yy_pop_state(); //leave X_ZERO
                                    yy_pop_state(); //leave X_BACKSLASH
                                    yy_pop_state(); //leave X_STRING
                                    return tSTRING;
                                  } 
<X_ZERO>.                         ;
  

  /* TODO: think about "ab\0xz" == "ab" */  
  /* TODO: think about why any char can be with backslash */

  /* ====================================================================== */
  /* ====[                     (other characters)                     ]==== */
  /* ====================================================================== */

[[:space:]]         /* ignore remaining white space inside the program */
.                   yyerror(yytext);    /* all other chars: error! */

  /* ====================================================================== */
  /* ====[                          THE END                           ]==== */
  /* ====================================================================== */

%%
