#pragma once

#include <cdk/ast/expression_node.h>

namespace udf {
    class iterate_node : public cdk::basic_node {
        cdk::expression_node *_condition;
        cdk::expression_node *_vector;
        cdk::expression_node *_count;
        cdk::expression_node *_function;

    public:
        iterate_node(int lineno, cdk::expresssion_node *condition, cdk::expression_node *vector,
            cdk::expression_node *count, cdk::expression_node *function) :
            basic_node(lineno), _condition(condition), _vector(vector), _count(count), _function(function) {
            }

        cdk::expression_node *condition()   { return _condition;    }
        cdk::expression_node *vector()      { return _vector;       }
        cdk::expression_node *count()       { return _count;        }
        cdk::expression_node *function()    { return _function;     }

        void accept(basic_ast_visitor *sp, int lvl) {
            sp->do_iterate_node(this, lvl);
        }
    }
} // udf