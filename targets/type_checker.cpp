#include <string>
#include <iostream>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>

#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void udf::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
  //EMPTY
}
void udf::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
  //EMPTY
}
void udf::type_checker::do_break_node(udf::break_node *const node, int lvl) {
  //EMPTY
}
void udf::type_checker::do_continue_node(udf::continue_node *const node, int lvl) {
  //EMPTY
}
void udf::type_checker::do_block_node(udf::block_node *const node, int lvl) {
  //EMPTY
}

//---------------------------------------------------------------------------

void udf::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++)
    node->node(i)->accept(this, lvl);
}



void udf::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else {
    throw std::string("wrong type in unary logical expression");
  }
}
void udf::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary expression");
  }

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary expression");
  }

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}
void udf::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary expression");
  }

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in binary expression");
  }

  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//---------------------------------------------------------------------------

void udf::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

void udf::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void udf::type_checker::do_nullptr_node(udf::nullptr_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->type(cdk::reference_type::create(4, nullptr));
}

//---------------------------------------------------------------------------

void udf::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in argument of unary expression");

  // in UDF, expressions are always int
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_unary_minus_node(cdk::unary_minus_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

void udf::type_checker::do_unary_plus_node(cdk::unary_plus_node *const node, int lvl) {
  processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void udf::type_checker::processBinaryExpression(cdk::binary_operation_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  if (!node->left()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in left argument of binary expression");

  node->right()->accept(this, lvl + 2);
  if (!node->right()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in right argument of binary expression");

  // in UDF, expressions are always int
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->left()->accept(this, lvl + 2);
  node->right()->accept(this, lvl + 2);

  if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) {
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(node->left()->type());
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) {
    node->type(node->right()->type());
  } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else if (node->left()->is_typed(cdk::TYPE_UNSPEC) && node->right()->is_typed(cdk::TYPE_UNSPEC)) {
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->left()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    node->right()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
  } else {
    throw std::string("wrong types in binary expression");
  }
}
void udf::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void udf::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void udf::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void udf::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void udf::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void udf::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void udf::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void udf::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void udf::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}
void udf::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
  processBinaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void udf::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
  ASSERT_UNSPEC; //FIXME1: different from og, might not work needs testing
  const std::string &id = node->name();
  std::shared_ptr<udf::symbol> symbol = _symtab.find(id);

  if (symbol != nullptr) {
    node->type(symbol->type());
  } 
  else {
    throw id;
  }
}

void udf::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
  ASSERT_UNSPEC; //FIXME1: different from og, might not work needs testing
  try { 
    node->lvalue()->accept(this, lvl);
    node->type(node->lvalue()->type());
  } catch (const std::string &id) {
    throw "undeclared variable '" + id + "'";
  }
}

void udf::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
  ASSERT_UNSPEC; //FIXME1: this is the consequence of actions taken above, might be good to just do like og
  node->lvalue()->accept(this, lvl + 4);
  node->rvalue()->accept(this, lvl + 4);
  
  /*try {
    node->lvalue()->accept(this, lvl);
  } catch (const std::string &id) {
    auto symbol = std::make_shared<udf::symbol>(cdk::primitive_type::create(4, cdk::TYPE_INT), id, 0);
    _symtab.insert(id, symbol);
    _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
    node->lvalue()->accept(this, lvl);  //DAVID: bah! //ME: commented cause this is nonsense
  }*/

  /*
  if (!node->lvalue()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in left argument of assignment expression");

  node->rvalue()->accept(this, lvl + 2);
  if (!node->rvalue()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type in right argument of assignment expression");
  */

  // in UDF, expressions are always int
  //node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));


  //FIXME: check and do TENSORS
  if (node->lvalue()->is_typed(cdk::TYPE_INT)) {
    if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } 
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } 
    else {
      throw "wrong assignment to integer";
    }
  } 
  else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {

    //TODO: check pointer level

    if (node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
      node->type(node->rvalue()->type());
    } 
    else if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
      //TODO: check that the integer is a literal and that it is zero
      node->type(cdk::primitive_type::create(4, cdk::TYPE_POINTER));
    } 
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_ERROR));
    } 
    else {
      throw "wrong assignment to pointer";
    }
  } else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {

    if (node->rvalue()->is_typed(cdk::TYPE_DOUBLE) || node->rvalue()->is_typed(cdk::TYPE_INT)) {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } 
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
      node->rvalue()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } 
    else {
      throw "wrong assignment to real";
    }
  } 
  else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {

    if (node->rvalue()->is_typed(cdk::TYPE_STRING)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    } 
    else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
      node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
      node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
    } 
    else {
      throw "wrong assignment to string";
    }
  } 
  else {
    throw "wrong types in assignment";
  }
}

//---------------------------------------------------------------------------

void udf::type_checker::do_evaluation_node(udf::evaluation_node *const node, int lvl) {
  node->argument()->accept(this, lvl + 2);
}

//---------------------------------------------------------------------------

void udf::type_checker::do_print_node(udf::print_node *const node, int lvl) {
  node->arguments()->accept(this, lvl + 2);
  //FIXME:maybe not cause its boolean?
  //node->newline()->accept(this, lvl + 2);
}

void udf::type_checker::do_input_node(udf::input_node *const node, int lvl) {
  //FIXME: CAN ONLY READ INT OR DOUBLE 
  node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------


void udf::type_checker::do_for_node(udf::for_node *const node, int lvl) {
  //FIXME2: is there anything to do?  Does everything need to be int?
  node->init()->accept(this, lvl + 4);
  node->condition()->accept(this, lvl + 4);
  node->increment()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void udf::type_checker::do_if_node(udf::if_node *const node, int lvl) {
  //FIXME2: is there anything to do?  Does everything need to be int?
  node->condition()->accept(this, lvl + 4);
}

void udf::type_checker::do_if_else_node(udf::if_else_node *const node, int lvl) {
  //FIXME2: is there anything to do?  Does everything need to be int?
  node->condition()->accept(this, lvl + 4);
}

//---------------------------------------------------------------------------

void udf::type_checker::do_return_node(udf::return_node *const node, int lvl) {
  if (node->retval() != nullptr) node->retval()->accept(this, lvl);
  //FIXME: using is_typed(cdk::functional_type::create(cdk::TYPE_INT))
  /*if (node->retval()) { //FIXME: tensor
    if (_function->type() != nullptr && _function->is_typed(cdk::TYPE_VOID)) {
      std::cout << "here\n";
      throw std::string("initializer specified for void function.");
    }
    
    node->retval()->accept(this, lvl + 2);
    
    // function is auto: copy type of first return expression
    if (_function->type() == nullptr) {
      _function->set_type(node->retval()->type());
      return; // simply set the type
    }
    std::cout << "here\n";
    //FIXME: does it have a problem with int/double thing?
    if (_inBlockReturnType == nullptr) {
      _inBlockReturnType = node->retval()->type();
    } 
    else {
      if (_inBlockReturnType != node->retval()->type()) {
        _function->set_type(cdk::primitive_type::create(0, cdk::TYPE_ERROR));  // probably irrelevant
        throw std::string("all return statements in a function must return the same type.");
      }
    }

    //FIXME: left-over stuff for debugging??
    std::cout << "FUNCT TYPE " << (_function->type() == nullptr ? "auto" : cdk::to_string(_function->type())) << std::endl;
    std::cout << "RETVAL TYPE " << cdk::to_string(node->retval()->type()) << std::endl;

    // se setar TYPE_FUNCTIONAL, o xml roda, ou seja toda function é functional type
    if (_function->is_typed(cdk::TYPE_INT)) { 
      if (!node->retval()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type for initializer (integer expected).");
    } 
    else if (_function->is_typed(cdk::TYPE_DOUBLE)) {
      if (!node->retval()->is_typed(cdk::TYPE_INT) && !node->retval()->is_typed(cdk::TYPE_DOUBLE)) {
        throw std::string("wrong type for initializer (integer or double expected).");
      }
    } 
    else if (_function->is_typed(cdk::TYPE_STRING)) {
      if (!node->retval()->is_typed(cdk::TYPE_STRING)) {
        throw std::string("wrong type for initializer (string expected).");
      }
    } 
    else if (_function->is_typed(cdk::TYPE_POINTER)) { //FIXME: crazy code ahead
      //DAVID: FIXME: trouble!!! ME: what throuble?!?!?
      int ft = 0, rt = 0;
      auto ftype = _function->type();
      while (ftype->name() == cdk::TYPE_POINTER) { // get function type referenced by (possibly recurssive) pointers
        ft++;
        ftype = cdk::reference_type::cast(ftype)->referenced();
      }
      auto rtype = node->retval()->type();
      while (rtype != nullptr && rtype->name() == cdk::TYPE_POINTER) { // get return type referenced by (possibly recurssive) pointers
        rt++;
        rtype = cdk::reference_type::cast(rtype)->referenced();
      }

      std::cout << "FUNCT TYPE " << cdk::to_string(_function->type()) << " --- " << ft << " -- " << ftype->name() << std::endl;
      std::cout << "RETVAL TYPE " << cdk::to_string(node->retval()->type()) << " --- " << rt << " -- " << cdk::to_string(rtype)
          << std::endl;

      // check if they are compatible
      bool compatible = (ft == rt) && (rtype == nullptr || (rtype != nullptr && ftype->name() == rtype->name()));
      if (!compatible) throw std::string("wrong type for return expression (pointer expected).");
    } 
    else {
      throw std::string("unknown type for initializer.");
    }
  }*/
}

//---------------------------------------------------------------------------

void udf::type_checker::do_variable_declaration_node(udf::variable_declaration_node *const node, int lvl) {
  //FIXME auto support here and postfix wirter
  if (node->initializer() != nullptr) {
    node->initializer()->accept(this, lvl + 2);

    if (node->is_typed(cdk::TYPE_INT)) {
      if (!node->initializer()->is_typed(cdk::TYPE_INT)) throw std::string("wrong type for initializer (integer expected).");
    } 
    else if (node->is_typed(cdk::TYPE_DOUBLE)) {
      if (!node->initializer()->is_typed(cdk::TYPE_INT) && !node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
        throw std::string("wrong type for initializer (integer or double expected).");
      }
    } 
    else if (node->is_typed(cdk::TYPE_STRING)) {
      if (!node->initializer()->is_typed(cdk::TYPE_STRING)) {
        throw std::string("wrong type for initializer (string expected).");
      }
    }
    else if (node->is_typed(cdk::TYPE_TENSOR)) {
      if (!node->initializer()->is_typed(cdk::TYPE_TENSOR)) { //FIXME: assuming that its not udf::tensor_node 
        throw std::string("wrong type for initializer (string expected).");
      }
    }
    else if (node->is_typed(cdk::TYPE_POINTER)) {
      //DAVID: FIXME: trouble!!! ME: dont know what to do different
      if (!node->initializer()->is_typed(cdk::TYPE_POINTER)) {
        auto in = (cdk::literal_node<int>*)node->initializer();
        if (in == nullptr || in->value() != 0) throw std::string("wrong type for initializer (pointer expected).");
      }
    } 
    else {
      throw std::string("unknown type for initializer.");
    }
  }

  const std::string &id = node->identifier();
  //FIXME: need to modify constructor and create a function in symbol.h (just like og)
  auto symbol = udf::make_symbol(false, node->qualifier(), node->type(), id, (bool)node->initializer(), false);
  if (_symtab.insert(id, symbol)) {
    _parent->set_new_symbol(symbol);  // advise parent that a symbol has been inserted
  } //FIXME: check if i need to redeclared forward vars (I dont think so but...)
  else {
    throw std::string("variable '" + id + "' redeclared");
  }
}


//---------------------------------------------------------------------------

void udf::type_checker::do_index_node(udf::index_node *const node, int lvl) {
  //FIXME: I straight up dont know what is happening here, im assuming its correct
  ASSERT_UNSPEC;
  std::shared_ptr < cdk::reference_type > btype;

  if (node->base()) {
    node->base()->accept(this, lvl + 2);
    btype = cdk::reference_type::cast(node->base()->type());
    if (!node->base()->is_typed(cdk::TYPE_POINTER)) throw std::string("pointer expression expected in index left-value");
  } 
  else {
    //_function not declared, need to once again modify symbol.h
    btype = cdk::reference_type::cast(_function->type());
    if (!_function->is_typed(cdk::TYPE_POINTER)) throw std::string("return pointer expression expected in index left-value");
    
  }

  node->index()->accept(this, lvl + 2);
  if (!node->index()->is_typed(cdk::TYPE_INT)) throw std::string("integer expression expected in left-value index");

  node->type(btype->referenced());
}

//---------------------------------------------------------------------------

void udf::type_checker::do_sizeof_node(udf::sizeof_node *const node, int lvl) {
  ASSERT_UNSPEC
  node->expression()->accept(this, lvl + 2);
  node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void udf::type_checker::do_stack_alloc_node(udf::stack_alloc_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->argument()->accept(this, lvl + 2);
  if (!node->argument()->is_typed(cdk::TYPE_INT)) {
    throw std::string("integer expression expected in allocation expression");
  }
  //DAVID TODO FIXME || ME: wtf?
  auto mytype = cdk::reference_type::create(4, cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
  node->type(mytype);
}

void udf::type_checker::do_address_of_node(udf::address_of_node *const node, int lvl) {
  ASSERT_UNSPEC;
  node->lvalue()->accept(this, lvl + 2);
  node->type(cdk::reference_type::create(4, node->lvalue()->type()));
}

//---------------------------------------------------------------------------

void udf::type_checker::do_function_call_node(udf::function_call_node *const node, int lvl) {
  ASSERT_UNSPEC;

  const std::string &id = node->identifier();
  auto symbol = _symtab.find(id);
  if (symbol == nullptr) throw std::string("symbol '" + id + "' is undeclared.");
  if (!symbol->function()) throw std::string("symbol '" + id + "' is not a function.");

  /*if (symbol->is_typed(cdk::TYPE_STRUCT)) {
    // declare return variable for passing to function call
    const std::string return_var_name = "$return_" + id;
    auto return_symbol = og::make_symbol(false, symbol->qualifier(), symbol->type(), return_var_name, false, false);
    if (_symtab.insert(return_var_name, return_symbol)) {
    } else {
      // if already declared, ignore new insertion
    }
  }*/

  auto f_type = cdk::functional_type::cast(symbol->type())->output(0);

  node->type(f_type);

  if (node->arguments()->size() == symbol->number_of_arguments()) {
    node->arguments()->accept(this, lvl + 4);
    for (size_t ax = 0; ax < node->arguments()->size(); ax++) {
      if (node->argument(ax)->type() == symbol->argument_type(ax)) continue;
      if (symbol->argument_is_typed(ax, cdk::TYPE_DOUBLE) && node->argument(ax)->is_typed(cdk::TYPE_INT)) continue;
      throw std::string("type mismatch for argument " + std::to_string(ax + 1) + " of '" + id + "'.");
    }
  } else {
    throw std::string(
        "number of arguments in call (" + std::to_string(node->arguments()->size()) + ") must match declaration ("
            + std::to_string(symbol->number_of_arguments()) + ").");
  }
}

void udf::type_checker::do_function_definition_node(udf::function_definition_node *const node, int lvl) {
  std::string id;

  // "fix" naming issues...
  if (node->identifier() == "udf")
    id = "_main";
  /*else if (node->identifier() == "_main")
    id = "._main";*/
  else
    id = node->identifier();

  // remember symbol so that args know
  auto function = udf::make_symbol(false, node->qualifier(), node->type(), id, false, true, true);

  std::vector < std::shared_ptr < cdk::basic_type >> argtypes;
  for (size_t ax = 0; ax < node->arguments()->size(); ax++)
    argtypes.push_back(node->argument(ax)->type());
  function->set_argument_types(argtypes);

  std::shared_ptr<udf::symbol> previous = _symtab.find(function->name());
  if (previous) {
    if (false /*DAVID: FIXME: should verify fields*/) {
      throw std::string("conflicting declaration for '" + function->name() + "'");
    }
  } else {
    _symtab.insert(function->name(), function);
    _parent->set_new_symbol(function);
  }
}

void udf::type_checker::do_function_declaration_node(udf::function_declaration_node *const node, int lvl) {
  //TODO
  // naming issues, tecnically, you can substitute udf for _main, and you dont need to substitute _main for ._main
  // because _main is not a valid id that the user can write (it has to start with a letter)
  // also why do we need to subsitute at all? Is it cause i can call a variable udf? No, cause
  // symbol (in og) is more complex, and diferentiates between variable and function...
  std::string id;

  // "fix" naming issues...
  if (node->identifier() == "udf")
    id = "_main";
  else
    id = node->identifier();

  // remember symbol so that args know
  auto function = udf::make_symbol(false, node->qualifier(), node->type(), id, false, true, true);

  std::vector < std::shared_ptr < cdk::basic_type >> argtypes;
  for (size_t ax = 0; ax < node->arguments()->size(); ax++)
    argtypes.push_back(node->argument(ax)->type());
  function->set_argument_types(argtypes);

  std::shared_ptr<udf::symbol> previous = _symtab.find(function->name());
  if (previous) {
    if (false /*DAVID: FIXME: should verify fields*/) {
      throw std::string("conflicting declaration for '" + function->name() + "'");
    }
  } else {
    _symtab.insert(function->name(), function);
    _parent->set_new_symbol(function);
  }
}

//---------------------------------------------------------------------------

void udf::type_checker::do_tensor_node(udf::tensor_node *const node, int lvl) {
  //TODO
}
void udf::type_checker::do_tensor_index_node(udf::tensor_index_node *const node, int lvl) {
  //TODO
}

void udf::type_checker::do_tensor_capacity_node(udf::tensor_capacity_node *const node, int lvl) {
  // TODO
}

void udf::type_checker::do_tensor_contraction_node(udf::tensor_contraction_node *const node, int lvl) {
  // TODO
}

void udf::type_checker::do_tensor_dim_node(udf::tensor_dim_node *const node, int lvl) {
  // TODO
}

void udf::type_checker::do_tensor_dims_node(udf::tensor_dims_node *const node, int lvl) {
  // TODO
}

void udf::type_checker::do_tensor_rank_node(udf::tensor_rank_node *const node, int lvl) {
  // TODO
}

void udf::type_checker::do_tensor_reshape_node(udf::tensor_reshape_node *const node, int lvl) {
  // TODO
}

//---------------------------------------------------------------------------

void udf::type_checker::do_iterate_node(udf::iterate_node *const node, int lvl) {
  // TODO
}
