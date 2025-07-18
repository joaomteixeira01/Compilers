#pragma once

#include <string>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>

namespace udf {

  class tensor_index_node: public cdk::lvalue_node {
    cdk::expression_node *_tensor;
    cdk::sequence_node *_position;

  public:
    tensor_index_node(int lineno, cdk::expression_node *tensor, cdk::sequence_node *position) :
        cdk::lvalue_node(lineno), _tensor(tensor), _position(position) {
    }

    cdk::expression_node *tensor() { return _tensor; }
    
    cdk::sequence_node *position() { return _position; }

    void accept(basic_ast_visitor *sp, int level) { 
        sp->do_tensor_index_node(this, level);
    }

  };

} // udf