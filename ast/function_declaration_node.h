#pragma once

#include <cdk/ast/basic_node.h>

namespace udf {

  class function_declaration_node: public cdk::typed_node {
    int _qualifier;
    std::string _identifier;
    cdk::sequence_node *_arguments;

  public:

    function_declaration_node(int lineno, int qualifier, std::shared_ptr<cdk::basic_type> ret_type,
     const std::string &identifier, cdk::sequence_node *arguments) :
        cdk::typed_node(lineno), _qualifier(qualifier), _identifier(identifier), _arguments(arguments) {
            std::vector<std::shared_ptr<cdk::basic_type>> inputs;
      for (size_t i = 0; i < _arguments->size(); i++)
        inputs.push_back(dynamic_cast<cdk::typed_node *>(_arguments->node(i))->type());
      type(cdk::functional_type::create(inputs, ret_type));
    }

    int qualifier() { return _qualifier; }
    
    const std::string& identifier() const { return _identifier; }

    cdk::typed_node* argument(size_t ax) { return dynamic_cast<cdk::typed_node*>(_arguments->node(ax)); }
    
    cdk::sequence_node* arguments() { return _arguments; }

    void accept(basic_ast_visitor *sp, int level) {
      sp->do_function_declaration_node(this, level);
    }

  };

} // udf