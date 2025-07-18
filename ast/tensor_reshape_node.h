#pragma once

#include <cdk/ast/expression_node.h>
#include <cdk/ast/sequence_node.h>
#include <string>

namespace udf {

    /**
     * Represents a tensor reshape operation: t.reshape(...) 
     */
    class tensor_reshape_node : public cdk::expression_node {
        cdk::expression_node *_tensor;
        cdk::sequence_node *_dimensions; // new list of dimensions (S1, S2, ..., Sk)

    public:
        tensor_reshape_node(int lineno, cdk::expression_node *tensor, cdk::sequence_node *dimensions)
            : cdk::expression_node(lineno), _tensor(tensor), _dimensions(dimensions) {
        }

        cdk::sequence_node *dimensions() const { return _dimensions; }

        cdk::expression_node *tensor() { return _tensor; }

        void accept(basic_ast_visitor *sp, int level) {
            sp->do_tensor_reshape_node(this, level);
        }
    };

} // udf
