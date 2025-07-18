#pragma once

#include <string>
#include <cdk/ast/typed_node.h>
#include <cdk/ast/basic_node.h>
#include <cdk/types/functional_type.h>

namespace udf {

  class function_definition_node: public cdk::typed_node {
    int _qualifier;
    std::string _identifier;
    cdk::sequence_node *_arguments;
    udf::block_node *_block;

  public:
    function_definition_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> ret_type,  
        const std::string &identifier, cdk::sequence_node *arguments, udf::block_node *block) :
        cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments), _block(block) {
      std::vector<std::shared_ptr<cdk::basic_type>> inputs;
      for (size_t i = 0; i < _arguments->size(); i++)
        inputs.push_back(dynamic_cast<cdk::typed_node *>(_arguments->node(i))->type());
      type(cdk::functional_type::create(inputs, ret_type));
    }

    int qualifier() { return _qualifier; }
    
    const std::string& identifier() const { return _identifier; }
    
    cdk::sequence_node* arguments() { return _arguments; }
    
    cdk::typed_node* argument(size_t ax) { return dynamic_cast<cdk::typed_node*>(_arguments->node(ax)); }
    
    udf::block_node* block() { return _block; }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_definition_node(this, level);
    }

  };

} // udf