%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expr type */
  //-- don't change *any* of these --- END!

  int                   i;          /* integer value */
  double                d;          /* integer value */
  std::string          *s;          /* symbol name or string literal */
  cdk::basic_node      *node;       /* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;

  udf::block_node      *block;
  std::vector<size_t>  *dims;
};

%token tTYPE_INT tTYPE_REAL tTYPE_STRING tTYPE_VOID tTYPE_TENSOR tTYPE_PTR
%token tPUBLIC tFORWARD tPRIVATE 
%token tAUTO
%token tFOR tIF tELIF tELSE
%token tBREAK tCONTINUE tRETURN
%token tGE tLE tEQ tNE tAND tOR
%token tSIZEOF tOBJECTS
%token tWRITE tWRITELN tINPUT 
%token tRANK tDIMS tDIM tCAPACITY tRESHAPE tCONTRACTION
%token tUNLESS tITERATE tFOR tUSING
%token '?' ':'

%token<i> tINTEGER
%token<s> tID tSTRING
%token<d> tREAL
%token<expression> tNULLPTR 

%nonassoc tIF
%nonassoc tTHEN
%nonassoc tELIF tELSE

%right '='
%left tOR
%left tAND
%right '~'
%left tEQ tNE
%left '<' tGE tLE '>' 
%left '+' '-'
%left '*' '/' '%'
%right '@'
%left tCONTRACTION //FIXME: check precedences later
%left '.'
%right tUNARY

%type<node> program dec instr func_dec func_def var condition iteration arg  if_false for_arg for_auto 
%type<sequence> decs vars exprs instrs opt_exprs args opt_for for_args for_autos opt_vars opt_instr
%type<expression> expr opt_init integer
%type<lvalue> lvalue
%type<block> block

%type<s> string
%type<type> void_type data_type 
%type<dims> dims
%type<node> iteration

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

program   : /* empty */                           { compiler->ast( $$ = new cdk::sequence_node(LINE)); }
          | decs                                  { compiler->ast( $$ = $1); }
          ;

decs      :      dec                              { $$ = new cdk::sequence_node(LINE, $1); }
          | decs dec                              { $$ = new cdk::sequence_node(LINE, $2, $1); }
          ;

dec       : var ';'                               { $$ = $1; }
          | func_dec                              { $$ = $1; }
          | func_def                              { $$ = $1; }
          ;

vars      :      var ';'                          { $$ = new cdk::sequence_node(LINE, $1); }
          | vars var ';'                          { $$ = new cdk::sequence_node(LINE, $2, $1); }
          ;

var       : tFORWARD data_type tID                { $$ = new udf::variable_declaration_node(LINE, tPUBLIC, $2, *$3, nullptr); }
          |          data_type tID opt_init       { $$ = new udf::variable_declaration_node(LINE, tPRIVATE, $1, *$2, $3); }
          | tPUBLIC  data_type tID opt_init       { $$ = new udf::variable_declaration_node(LINE, tPUBLIC, $2, *$3, $4); }
          |          tAUTO     tID '=' expr       { $$ = new udf::variable_declaration_node(LINE, tPRIVATE, nullptr, *$2, $4); }  
          | tPUBLIC  tAUTO     tID '=' expr       { $$ = new udf::variable_declaration_node(LINE, tPUBLIC, nullptr, *$3, $5); }
          ;

opt_init  : /* empty */                           { $$ = nullptr; }
          | '=' expr                              { $$ = $2; }
          ;
          
func_dec  :          data_type  tID '(' args ')'       { $$ = new udf::function_declaration_node(LINE, tPRIVATE, $1, *$2, $4); }
          |          void_type  tID '(' args ')'       { $$ = new udf::function_declaration_node(LINE, tPRIVATE, $1, *$2, $4); }
          |          tAUTO tID '(' args ')'            { $$ = new udf::function_declaration_node(LINE, tPRIVATE, nullptr, *$2, $4); }
          | tPUBLIC  data_type  tID '(' args ')'       { $$ = new udf::function_declaration_node(LINE, tPUBLIC, $2, *$3, $5); }
          | tPUBLIC  void_type  tID '(' args ')'       { $$ = new udf::function_declaration_node(LINE, tPUBLIC, $2, *$3, $5); }
          | tPUBLIC  tAUTO tID '(' args ')'            { $$ = new udf::function_declaration_node(LINE, tPUBLIC, nullptr, *$3, $5); }
          | tFORWARD data_type  tID '(' args ')'       { $$ = new udf::function_declaration_node(LINE, tPUBLIC, $2, *$3, $5); }
          | tFORWARD void_type  tID '(' args ')'       { $$ = new udf::function_declaration_node(LINE, tPUBLIC, $2, *$3, $5); }
          | tFORWARD tAUTO tID '(' args ')'            { $$ = new udf::function_declaration_node(LINE, tPUBLIC, nullptr, *$3, $5); }
          ;

func_def  :          data_type  tID '(' args ')' block { $$ = new udf::function_definition_node(LINE, tPRIVATE, $1, *$2, $4, $6); }
          |          void_type  tID '(' args ')' block { $$ = new udf::function_definition_node(LINE, tPRIVATE, $1, *$2, $4, $6); }
          |          tAUTO      tID '(' args ')' block { $$ = new udf::function_definition_node(LINE, tPRIVATE, nullptr, *$2, $4, $6); }
          | tPUBLIC  data_type  tID '(' args ')' block { $$ = new udf::function_definition_node(LINE, tPUBLIC, $2, *$3, $5, $7); }
          | tPUBLIC  void_type  tID '(' args ')' block { $$ = new udf::function_definition_node(LINE, tPUBLIC, $2, *$3, $5, $7); }
          | tPUBLIC  tAUTO      tID '(' args ')' block { $$ = new udf::function_definition_node(LINE, tPUBLIC, nullptr, *$3, $5, $7); }
          ;

args      : /* empty */                           { $$ = new cdk::sequence_node(LINE); }
          |          arg                          { $$ = new cdk::sequence_node(LINE, $1); }
          | args ',' arg                          { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

arg       : data_type tID                         { $$ = new udf::variable_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); }
          ;

void_type : tTYPE_VOID                            { $$ = cdk::primitive_type::create(0, cdk::TYPE_VOID); }
          ;

data_type : tTYPE_INT                             { $$ = cdk::primitive_type::create(4, cdk::TYPE_INT); }
          | tTYPE_REAL                            { $$ = cdk::primitive_type::create(8, cdk::TYPE_DOUBLE); }
          | tTYPE_STRING                          { $$ = cdk::primitive_type::create(4, cdk::TYPE_STRING); }
          | tTYPE_PTR '<' data_type '>'           { $$ = cdk::reference_type::create(4, $3); }
          | tTYPE_PTR '<' tAUTO     '>'           { $$ = cdk::reference_type::create(4, nullptr); }
          | tTYPE_TENSOR '<' dims '>'             { $$ = cdk::tensor_type::create(*$3); }
          ;

dims      : tINTEGER                              { $$ = new std::vector<size_t>(); $$->push_back($1); }
          | dims ',' tINTEGER                     { $$ = $1; $$->push_back($3); }
          ;

block     : '{' opt_vars opt_instr '}'            { $$ = new udf::block_node(LINE, $2, $3); }
          ;

opt_vars  : /*empty*/                             { $$ = NULL; }
          | vars                                  { $$ = $1; }
          ;

opt_instr : /*empty*/                             { $$ = new cdk::sequence_node(LINE); }
          | instrs                                { $$ = $1; }
          ;
     
instrs    :        instr                          { $$ = new cdk::sequence_node(LINE, $1); }
          | instrs instr                          { $$ = new cdk::sequence_node(LINE, $2, $1); }
          ;

instr     :           expr  ';'                   { $$ = new udf::evaluation_node(LINE, $1); }
          | tWRITE    exprs ';'                   { $$ = new udf::print_node(LINE, $2, false); }
          | tWRITELN  exprs ';'                   { $$ = new udf::print_node(LINE, $2, true); }
          | tBREAK                                { $$ = new udf::break_node(LINE); }
          | tCONTINUE                             { $$ = new udf::continue_node(LINE); }
          | tRETURN         ';'                   { $$ = new udf::return_node(LINE, nullptr);}
          | tRETURN   expr  ';'                   { $$ = new udf::return_node(LINE, $2); }
          | condition                             { $$ = $1; }
          | iteration                             { $$ = $1; }
          | block                                 { $$ = $1; }
          ;

condition : tIF '(' expr ')' %prec tTHEN instr    { $$ = new udf::if_node(LINE, $3, $5); }
          | tIF '(' expr ')' instr if_false       { $$ = new udf::if_else_node(LINE, $3, $5, $6); }
          ;

if_false  : tELSE                          instr  { $$ = $2; }
          | tELIF '(' expr ')' %prec tTHEN instr  { $$ = new udf::if_node(LINE, $3, $5); }
          | tELIF '(' expr ')' instr if_false     { $$ = new udf::if_else_node(LINE, $3, $5, $6); }
          ;

iteration : tFOR '(' opt_for ';' opt_exprs ';' opt_exprs ')' instr    { $$ = new udf::for_node(LINE, $3, $5, $7, $9); }
          | tUNLESS expr tITERATE expr tFOR expr tUSING expr ';'      { $$ = new udf::iterate_node(LINE, $2, $4, $6, $8); }
          | tUNLESS expr ? expr ':' expr expr                         { $$ = new udf::iterate_node(LINE, $2, $4, $6, $7); }
          ;

opt_for   : /* empty */                           { $$ = new cdk::sequence_node(LINE); }
          | for_args                              { $$ = $1; }
          | tAUTO for_autos                       { $$ = new cdk::sequence_node(LINE, $2); }
          | exprs                                 { $$ = $1; }
          ;

for_autos :               for_auto                { $$ = new cdk::sequence_node(LINE, $1); }
          | for_autos ',' for_auto                { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

for_auto  : tID '=' expr                          { $$ = new udf::variable_declaration_node(LINE, tPRIVATE, nullptr, *$1, $3); }
          ;

for_args  :              for_arg                  { $$ = new cdk::sequence_node(LINE, $1); }
          | for_args ',' for_arg                  { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

for_arg   : data_type tID '=' expr                { $$ = new udf::variable_declaration_node(LINE, tPRIVATE, $1, *$2, $4); }
          ;

opt_exprs : /* empty */                           { $$ = new cdk::sequence_node(LINE); }
          | exprs                                 { $$ = $1; }
          ;

exprs     :           expr                        { $$ = new cdk::sequence_node(LINE, $1);     }
          | exprs ',' expr                        { $$ = new cdk::sequence_node(LINE, $3, $1); }
          ;

expr      : integer                               { $$ = $1; }
          | tREAL                                 { $$ = new cdk::double_node(LINE, $1);  }
          | string                                { $$ = new cdk::string_node(LINE, $1); }
          | tNULLPTR                              { $$ = new udf::nullptr_node(LINE); }
          | lvalue                                { $$ = new cdk::rvalue_node(LINE, $1); }
          | lvalue '=' expr                       { $$ = new cdk::assignment_node(LINE, $1, $3); }
          | expr '+' expr                         { $$ = new cdk::add_node(LINE, $1, $3); }
          | expr '-' expr                         { $$ = new cdk::sub_node(LINE, $1, $3); }
          | expr '*' expr                         { $$ = new cdk::mul_node(LINE, $1, $3); }
          | expr '/' expr                         { $$ = new cdk::div_node(LINE, $1, $3); }
          | expr '%' expr                         { $$ = new cdk::mod_node(LINE, $1, $3); }
          | '-' expr %prec tUNARY                 { $$ = new cdk::unary_minus_node(LINE, $2); }
          | '+' expr %prec tUNARY                 { $$ = new cdk::unary_plus_node(LINE, $2); }
          | '~' expr                              { $$ = new cdk::not_node(LINE, $2); }
          | expr '<' expr                         { $$ = new cdk::lt_node(LINE, $1, $3); }
          | expr tLE expr                         { $$ = new cdk::le_node(LINE, $1, $3); }
          | expr tEQ expr                         { $$ = new cdk::eq_node(LINE, $1, $3); }
          | expr tGE expr                         { $$ = new cdk::ge_node(LINE, $1, $3); }
          | expr '>' expr                         { $$ = new cdk::gt_node(LINE, $1, $3); }
          | expr tNE expr                         { $$ = new cdk::ne_node(LINE, $1, $3); }
          | expr tAND expr                        { $$ = new cdk::and_node(LINE, $1, $3); }
          | expr tOR  expr                        { $$ = new cdk::or_node(LINE, $1, $3); }
          | tINPUT                                { $$ = new udf::input_node(LINE); }
          | tID '(' opt_exprs ')'                 { $$ = new udf::function_call_node(LINE, *$1, $3); }
          | tSIZEOF '(' expr ')'                  { $$ = new udf::sizeof_node(LINE, $3); }
          | '(' expr ')'                          { $$ = $2; }
          | '[' exprs ']'                         { $$ = new udf::tensor_node(LINE, $2); }
          | tOBJECTS '(' expr ')'                 { $$ = new udf::stack_alloc_node(LINE, $3); }
          | lvalue '?'                            { $$ = new udf::address_of_node(LINE, $1); }
          | expr '.' tRANK                        { $$ = new udf::tensor_rank_node(LINE, $1); }
          | expr '.' tDIMS                        { $$ = new udf::tensor_dims_node(LINE, $1); }
          | expr '.' tDIM '(' expr ')'            { $$ = new udf::tensor_dim_node(LINE, $1, $5); }
          | expr '.' tCAPACITY                    { $$ = new udf::tensor_capacity_node(LINE, $1); }
          | expr '.' tRESHAPE '(' exprs ')'       { $$ = new udf::tensor_reshape_node(LINE, $1, $5); }
          | expr tCONTRACTION expr                { $$ = new udf::tensor_contraction_node(LINE, $1, $3); }
          ;


lvalue    : tID                                   { $$ = new cdk::variable_node(LINE, $1); }
          | lvalue  '[' expr ']'                  { $$ = new udf::index_node(LINE, new cdk::rvalue_node(LINE, $1), $3); }
          | '(' expr ')' '[' expr ']'             { $$ = new udf::index_node(LINE, $2, $5); }
          | tID '(' opt_exprs ')' '[' expr ']'    { $$ = new udf::index_node(LINE, new udf::function_call_node(LINE, *$1, $3), $6); }
          | expr '@' '(' exprs ')'                { $$ = new udf::tensor_index_node(LINE, $1, $4); }
          ; 

string    : tSTRING                               { $$ = $1; }
          | string tSTRING                        { $$ = $1; $$->append(*$2); }
          ;

integer   : tINTEGER                              { $$ = new cdk::integer_node(LINE, $1); }
          ;
%%
