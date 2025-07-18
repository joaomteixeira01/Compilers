#pragma once

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>

namespace udf {

  /**
   * Represents a tensor literal
   */
  class tensor_node: public cdk::expression_node {
    cdk::sequence_node *_fields;

  public:
    inline tensor_node(int lineno, cdk::sequence_node *fields) :
        cdk::expression_node(lineno), _fields(fields) {
    }

  public:
    inline cdk::expression_node* field(size_t ix) {
      return (cdk::expression_node*)_fields->node(ix);
    }

    inline cdk::sequence_node* fields() {
      return _fields;
    }

    inline size_t length() {
      return _fields->size();
    }

  public:
    inline void accept(basic_ast_visitor *sp, int level) {
      sp->do_tensor_node(this, level);
    }

  };

} // udf