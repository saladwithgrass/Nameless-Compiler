﻿#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <vector>

std::ifstream program_stream;
std::string program_text;
std::stack<std::string> op;
bool opisanie_function = false;

enum states_num {
  operat,
  begin,
  id,
  punct,
  string,
  num,
  num_double,
  reserved,
  operat2
};

std::string returning_type_now = "";
int start_of_the_main = -1;
std::vector<std::pair<std::string, states_num>> devide_by_leksems(
    std::string str) {
  int i = 0;
  states_num state = begin;
  std::string leksem;
  std::vector<std::pair<std::string, states_num>> leksems;
  while (i < str.length()) {
    if (state == begin) {
      if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t') {
        ++i;
      }
      if ((str[i] <= 'z' && str[i] >= 'a') ||
          (str[i] <= 'Z' && str[i] >= 'A')) {
        state = id;
      }
      if (str[i] <= '9' && str[i] >= '0') {
        state = num;
      }
      if (str[i] == '&' || str[i] == '|' || str[i] == '!' || str[i] == '=' ||
          str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' ||
          str[i] == '>' || str[i] == '<' || str[i] == '#' || str[i] == '~' ||
          str[i] == '^' || str[i] == '%') {
        state = operat;
      }
      if (str[i] == ',' || str[i] == '(' || str[i] == ')' || str[i] == '{' ||
          str[i] == '}' || str[i] == ';' || str[i] == '[' || str[i] == ']') {
        state = punct;
      }
      if (str[i] == '\'' || str[i] == '"') {
        state = string;
        leksem.push_back(str[i]);
        ++i;
      }
    }
    if (state == num) {
      if (str[i] >= '0' && str[i] <= '9') {
        leksem.push_back(str[i]);
        ++i;
      } else {
        if (str[i] == '.') {
          state = num_double;
          leksem.push_back(str[i]);
          ++i;
        } else {
          state = begin;
          leksems.push_back(std::pair<std::string, states_num>(leksem, num));
          leksem.clear();
        }
      }
    }
    if (state == id) {
      if ((str[i] >= 'a' && str[i] <= 'z') ||
          (str[i] >= 'A' && str[i] <= 'Z') ||
          (str[i] >= '0' && str[i] <= '9')) {
        leksem.push_back(str[i]);
        ++i;
      } else {
        state = begin;
        std::ifstream reserved_words;
        reserved_words.open("reserved_words.txt");
        std::string s;
        bool word_is_reserved = false;
        while (std::getline(reserved_words, s)) {
          if (s == leksem) {
            word_is_reserved = true;
            break;
          }
        }
        reserved_words.close();
        if (!word_is_reserved) {
          leksems.push_back(std::pair<std::string, states_num>(leksem, id));
          leksem.clear();
        } else {
          leksems.push_back(
              std::pair<std::string, states_num>(leksem, reserved));
          leksem.clear();
        }
      }
    }
    if (state == num_double) {
      if (str[i] >= '0' && str[i] <= '9') {
        leksem.push_back(str[i]);
        ++i;
      } else {
        state = begin;
        leksems.push_back(std::pair<std::string, states_num>(leksem, num));
        leksem.clear();
      }
    }
    if (state == operat) {
      if (str[i] == '&' || str[i] == '|' || str[i] == '!' || str[i] == '=' ||
          str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' ||
          str[i] == '>' || str[i] == '<' || str[i] == '#' || str[i] == '~' ||
          str[i] == '^' || str[i] == '%') {
        leksem.push_back(str[i]);
        ++i;
        state = operat2;
      } else {
        state = begin;
        leksems.push_back(std::pair<std::string, states_num>(leksem, operat));
        leksem.clear();
      }
    }
    if (state == operat2) {
      if (str[i] == '=') {
        leksem.push_back(str[i]);
        ++i;
        state = begin;
        leksems.push_back(std::pair<std::string, states_num>(leksem, operat));
        leksem.clear();
      } else {
        if ((str[i - 1] == '+' && str[i] == '+') ||
            (str[i - 1] == '-' && str[i] == '-') ||
            (str[i - 1] == '|' && str[i] == '|') ||
            (str[i - 1] == '&' && str[i] == '&')) {
          leksem.push_back(str[i]);
          ++i;
          state = begin;
          leksems.push_back(std::pair<std::string, states_num>(leksem, operat));
          leksem.clear();
        } else {
          state = begin;
          leksems.push_back(std::pair<std::string, states_num>(leksem, operat));
          leksem.clear();
        }
      }
    }
    if (state == punct) {
      if (str[i] == ',' || str[i] == '(' || str[i] == ')' || str[i] == '{' ||
          str[i] == '}' || str[i] == ';' || str[i] == '[' || str[i] == ']') {
        state = begin;
        leksem.push_back(str[i]);
        leksems.push_back(std::pair<std::string, states_num>(leksem, punct));
        ++i;
        leksem.clear();
      }
    }
    if (state == string) {
      if (str[i] != '\'' && str[i] != '\"') {
        if (str[i] == '\n') {
          throw std::string("wrong string");
        }
        leksem.push_back(str[i]);
        ++i;
      } else {
        leksem.push_back(str[i]);
        state = begin;
        ++i;
        leksems.push_back(std::pair<std::string, states_num>(leksem, string));
        leksem.clear();
      }
    }
  }
  return leksems;
}

void program(), block(bool in_main = 0, bool to_push_map = 1),
    type_name(bool func_allowed = 1);
void do_while_operator();

struct lexeme {
  std::string lex;
  states_num type;
  inline bool operator==(const std::string& other) { return other == lex; }
  inline bool operator!=(const std::string& other) { return other != lex; }
};

enum error_type {
  unexpected_lexeme,
  unexpected_lexeme_type,
  out_of_bounds_forth,
  out_of_bounds_back,
  type_incompatibility,
  prohibited_operation,
  unknown_id,
  not_enuph_operands,
  wrong_operand_type
};

std::vector<lexeme> lexemes;
int current_index = 0;
lexeme cur;  //  = lexemes[0];

bool was_in_function =
    0;  // again, just to check whether ';' is needeed after type_name();

//  error dignostics system consists of two strings
//  i estimate that two layers are enough to pinpoint the error
//  layer 1 shows last major function
//  layer 2 shows last minor function like type or operator and the current
//  lexeme

std::string layer1, layer2, layer3;
// layer1  shows the name of a function with error
// layer2  shows a complex place which consists of many tokens like if operator
// etc. layer3  shows the exact place of problem like type or name etc.

class error {
  /*error(error_type type_p = unexpected_lexeme, const std::string& addition_p =
  "", const std::string& second_addition_p = "") : type(type_p),
  addition(addition_p), second_addition(second_addition_p) { layers[0] = layer1;
    layers[1] = layer2;
    layers[2] = layer3;
    problem = cur;
    problem_index = current_index;
  }*/
  // error_type type;
 public:
  error() {
    problem_index = current_index;
    problem = cur;
    layers[0] = layer1;
    layers[1] = layer2;
    layers[2] = layer3;
  }
  lexeme problem;
  std::string second_addition;
  std::string addition;
  int64_t problem_index;
  std::string layers[3];
};

class unexpected_lexeme_error : public error {
 public:
  unexpected_lexeme_error() {}  // please don't use it when throwing
  unexpected_lexeme_error(
      const std::string&
          expected_p) {  // expected_p is which lexeme is expected;
    addition = expected_p;
  }
};

class unexpected_lexeme_type_error : public error {
 public:
  unexpected_lexeme_type_error() {}  // please don't use it when throwing
  unexpected_lexeme_type_error(
      const std::string&
          expected_p) {  // expected_p is which type of lexeme is expected;
    addition = expected_p;
  }
};

class out_of_bounds_forth_error : public error {
 public:
  out_of_bounds_forth_error() {}
};

class out_of_bounds_back_error : public error {
 public:
  out_of_bounds_back_error() {}
};

class type_incompatibility_error
    : public error {  // used when operantor can't be applied to this type or
                      // when operand types are incompatible with this operator;
 public:
  type_incompatibility_error() {}  // please don't use it when throwing
  std::string problematic_operator;
  // type_1_p = type of first operand
  // problem_op_p = operator which is used; can be while or if
  // type_2_p is used in case the types are incompatible? but the operator
  // itself is ok
  type_incompatibility_error(const std::string& type_1_p,
                             const std::string& problem_op_p,
                             const std::string& type_2_p = "") {
    addition = type_1_p;
    second_addition = type_2_p;
    problematic_operator = problem_op_p;
  }
};

class prohbited_operation_error
    : public error {  // not really used anywhere, better use
                      // type_incompatibility with type_2_p = "";
 public:
  prohbited_operation_error() {}  // please don't use it when throwing
  prohbited_operation_error(const std::string& addition_p) {
    addition = addition_p;
  }
};

class unknown_id_error : public error {
 public:
  unknown_id_error() {}  // please don't use it when throwing
                         // unknown_name_p = id that wasn't found in TIDs
  unknown_id_error(const std::string& unknown_name_p,
                   const std::string& addition_p = "") {
    addition = unknown_name_p;
    if (addition_p == "") addition += "(" + addition_p + ")";
  }
};

class not_enough_operands_error
    : public error {  // is used when function is called with too few operators
 public:
  not_enough_operands_error() {}  // please don't use it when throwing
  // called_p = name of an operator or a function that was called with too few
  // parameters;
  not_enough_operands_error(const std::string& called_p) {
    addition = called_p;
  }
};

class wrong_operand_type_error : public error {
  // not really used anywhere
  // you'd better use type_incompatibility_error
 public:
  wrong_operand_type_error() {}  // please don't use it when throwing
  wrong_operand_type_error(const std::string& expected_type_p) {
    addition = expected_type_p;
  }
};

class wrong_parameters_error
    : public error {  // is used when function with given parameter list is not
                      // found e.g. too many, wrong types;
 public:
  wrong_parameters_error() {}  // please don't use it when throwing
  wrong_parameters_error(const std::string& addition_p,
                         const std::string& second_addition_p = "") {
    addition = addition_p;
    second_addition = second_addition_p;
  }
};

class not_allowed_error : public error {
 public:
  not_allowed_error() {}  // please don't use it when throwing
  // not_allowed_p = the thing that is not allowed e.g. function, double const;
  not_allowed_error(const std::string& not_allowed_p) {
    addition = not_allowed_p;
  }
};

class wrong_returning_type_error : public error {
 public:
  wrong_returning_type_error() {}  // don't use when throwing
  wrong_returning_type_error(const std::string& expected_type,
                             const std::string& received_type) {
    addition = expected_type;
    second_addition = received_type;
  }
};

struct tid {
  std::vector<std::string> type;
  tid* parent;
  std::vector<std::string> name;
  std::vector<tid*> child;
  tid(tid* par) {
    type.clear();
    parent = par;
    child.clear();
  }
};

struct functid {
  std::vector<std::string> returning_type;
  std::vector<std::vector<std::string>> parametr;
  std::vector<std::string> name;
  std::vector<bool> prototype_only;
  std::vector<std::vector<std::string>> parameter_name;
};

std::vector<void*> consts;

struct poliz_unit {
 public:
  poliz_unit(const std::string& val_p, const int& type_p,
             void* const_val_p = nullptr)
      : val(val_p), type(type_p), const_val(const_val_p) {}
  std::string val;
  int type;
  // 0 - var
  // 1 - operator
  // 2 - const
  // 4 - declaration
  void* const_val;
};

struct func_place {
 public:
  func_place(const std::string& name_p, const int& ind_p)
      : name(name_p), index(ind_p) {}
  int index;
  std::string name;
};

int current_pol_index;

std::vector<func_place> funcs;

int find_func_place(const std::string& func_name_p) {
  for (int co = 0; co < funcs.size(); ++co) {
    if (func_name_p == funcs[co].name) return funcs[co].index;
  }
}

std::stack<poliz_unit> opers;

std::vector<poliz_unit> pol;

enum priority_groups {
  brace,
  equals,
  func_call,
  comparison,
  additional,
  multiplicational,
  bits,
  power,
  unary
};

// brace('(') has the lowest priority for practical use
// when dumping stack it will find its priority to be the lowest
// and will stop because the priority has to be >= than current

struct wrong_to_check_error : public error {
  wrong_to_check_error() {}
};

priority_groups priority_of_oper(const std::string& to_check) {
  if (to_check == "(") return brace;
  if (to_check == "=") return equals;
  if (to_check == "==" || to_check == "!=" || to_check == ">" ||
      to_check == "<" || to_check == ">=" || to_check == "<=")
    return comparison;
  if (to_check == "+" || to_check == "-" || to_check == "||") return additional;
  if (to_check == "*" || to_check == "/" || to_check == "&&")
    return multiplicational;
  if (to_check == "^")
    return power;
  else {
    std::ifstream unOps("unOp.txt");
    std::string cur_op;
    while (!unOps.eof()) {
      unOps >> cur_op;
      if (cur_op == to_check) {
        unOps.close();
        return unary;
      }
    }
    unOps.close();
    // throw wrong_to_check_error();
    return func_call;
  }
}

void dump_with_priority(std::stack<poliz_unit>& opers,
                        std::vector<poliz_unit>& pol,
                        const std::string& pivot_oper = "") {
  while (opers.size() &&
         priority_of_oper(opers.top().val) >= priority_of_oper(pivot_oper)) {
    pol.push_back(opers.top());
    opers.pop();
  }
}

void dump_until(std::stack<poliz_unit>& opers, std::vector<poliz_unit>& pol,
                const std::string& until = "") {
  while (opers.size() && opers.top().val != until) {
    pol.push_back(opers.top());
    opers.pop();
  }
}

void dump_all(std::stack<poliz_unit>& opers, std::vector<poliz_unit>& pol) {
  while (opers.size()) {
    if (opers.top().val == "(") {
      opers.pop();
      continue;
    }
    pol.push_back(opers.top());
    opers.pop();
  }
}

functid func_tid;
tid* alltids = nullptr;

int check_func_id(std::string name) {
  for (int i = 0; i < func_tid.name.size(); ++i) {
    if (name == func_tid.name[i]) {
      return i;
    }
  }
  throw unknown_id_error(name);
}

void check_func_conflict(const std::string& name) {
  for (int co = 0; co < func_tid.name.size(); ++co) {
    if (name == func_tid.name[co])
      throw not_allowed_error("function redefinition");
  }
}

void check_func_op(std::string name) {
  int j = check_func_id(name);
  for (int i = func_tid.parametr[j].size() - 1; i >= 0; --i) {
    std::string s = op.top();
    if (op.top() != func_tid.parametr[j][i]) {
      throw wrong_parameters_error(func_tid.name[j]);
    }
    op.pop();
  }
  if (func_tid.returning_type[j] != "void") {
    op.push(func_tid.returning_type[j]);
  }
}

void check_var_conflict(std::string name) {
  for (int i = 0; i < alltids->name.size(); ++i) {
    if (alltids->name[i] == name) {
      throw not_allowed_error("double declaration of " + name);
      std::cout << "var conflict " << name;
      exit(0);
    }
  }
}

int get_casting_prior(std::string type) {
  if (type == "float") {
    return 1;
  }
  if (type == "float16") {
    return 2;
  }
  if (type == "float64") {
    return 3;
  }

  if (type == "int") {
    return 4;
  }
  if (type == "int16") {
    return 5;
  }
  if (type == "int64") {
    return 6;
  }
  if (type == "string") {
    return -3;
  }
  int pos = type.find("*");
  if (pos != -1) {
    return -1;
  }
  pos = type.find("&");
  if (pos != -1) {
    return -2;
  }
  return 0;
}

void check_parametrs(std::string name) {
  int i = check_func_id(name);
  for (int j = 0; j < func_tid.parametr[i].size(); ++j) {
    if (op.empty()) {
      throw not_enough_operands_error(name);
    }
    std::string operand = op.top();
    op.pop();
    if (get_casting_prior(func_tid.parametr[i][j]) <= 0) {
      if (get_casting_prior(operand) !=
          get_casting_prior(func_tid.parametr[i][j])) {
        throw wrong_parameters_error(func_tid.parametr[i][j], operand);
      }
    }
  }
}

void check_op() {
  int one = get_casting_prior(op.top());
  std::string first = op.top();
  op.pop();
  std::string oper = op.top();
  op.pop();
  std::string second = op.top();
  int two = get_casting_prior(op.top());
  op.pop();

  if (oper == "*" || oper == "-" || oper == "/") {
    if (two > 0 && one > 0) {
      if (two > one) {
        op.push(second);
        return;
      } else {
        op.push(second);
        return;
      }
    } else {
      throw type_incompatibility_error(first, second, oper);
      return;
    }
  }

  if (oper == ">" || oper == ">=" || oper == "<" || oper == "<=" ||
      oper == "==") {
    if (two > 0 && one > 0) {
      op.push("int");
      return;
    } else {
      throw type_incompatibility_error(first, second, oper);
      return;
    }
  }

  if (oper == "||" || oper == "&&") {
    if (two > 3 && one > 3) {
      op.push("int");
      return;
    } else {
      throw type_incompatibility_error(first, second, oper);
      return;
    }
  }

  if (oper == "%") {
    if (two > 3 && one > 3) {
      op.push("int");
      return;
    } else {
      throw type_incompatibility_error(first, second, oper);
      return;
    }
  }

  if (oper == "=") {
    if (one > 0 && two > 0) {
      op.push(first);
      return;
    } else {
      if (one == two) {
        op.push(first);
        return;
      } else {
        throw type_incompatibility_error(first, second, oper);
        return;
      }
    }
  }

  if (oper == "+") {
    if (two > 0 && one > 0) {
      if (two > one) {
        op.push(second);
        return;
      } else {
        op.push(second);
        return;
      }
    } else {
      if (one != -3) {
        throw type_incompatibility_error(first, second, oper);
        return;
      } else {
        if (one == two) {
          op.push("string");
          return;
        } else {
          throw type_incompatibility_error(first, second, oper);
          return;
        }
      }
    }
  }
  if (oper == "[]") {
    if (one >= 4) {
      if (two == -3) {
        op.push(second.substr(0, second.length() - 6));
        return;
      }
      throw type_incompatibility_error(first, second, oper);
    }
    throw type_incompatibility_error(first, second, oper);
  }
}

void check_unop() {
  std::string operator1 = op.top();
  int one = get_casting_prior(op.top());
  op.pop();
  std::string oper = op.top();
  op.pop();
  if (oper == "!") {
    if (one > 3) {
      op.push("int");
      return;
    } else {
      throw type_incompatibility_error(operator1, oper);
      return;
    }
  }
  if (oper == "++" || oper == "--") {
    if (one > 3 || (one == -3 && oper == "--")) {
      op.push(operator1);
      return;
    } else {
      throw type_incompatibility_error(operator1, oper);
      return;
    }
  }
  if (oper == "*") {
    if (one == -1) {
      operator1.pop_back();
      op.push(operator1);
      return;
    } else {
      throw type_incompatibility_error(operator1, oper);
      return;
    }
  }
  if (oper == "&") {
    if (one == -2) {
      operator1.pop_back();
      op.push(operator1);
      return;
    } else {
      throw type_incompatibility_error(operator1, oper);
      return;
    }
  }
}

void check_bool() {
  if (!(op.top() == "int" || op.top() == "int16" || op.top() == "int64")) {
    throw type_incompatibility_error(op.top(), layer2);
    std::cout << "expected bool";
    exit(0);
  } else {
    op.pop();
  }
}

std::string check_id(std::string name) {
  tid* a = alltids;
  while (a != nullptr) {
    for (int i = 0; i < a->name.size(); ++i) {
      if (a->name[i] == name) {
        return a->type[i];
      }
    }
    a = a->parent;
  }
  throw unknown_id_error(name);
  std::cout << "unknown name " << name;
  exit(0);
}

void next_lexeme() {
  if (current_index == 28 || current_index == 29) {
    int c = 0;
  }
  if (current_index == lexemes.size() - 1) throw out_of_bounds_forth_error();
  ++current_index;
  cur = lexemes[current_index];
}

void prev_lexeme() {
  if (current_index == 0) throw out_of_bounds_back_error();
  --current_index;
  cur = lexemes[current_index];
}

void expect(const std::string&
                expected_string) {  // checks if current lexeme is the one that
                                    // is expected else throws an exception
  if (expected_string != cur.lex)
    throw unexpected_lexeme_error(expected_string);
}

//  i tried to write the simplest tokens first
// name = id;

void exp_token();

void name() {
  layer3 = "name; ";
  if (cur.type != id) throw unexpected_lexeme_type_error("id");

  next_lexeme();
}

void num_const() {
  layer3 = "num const; ";
  if (cur.type != num) throw unexpected_lexeme_type_error("num");
  next_lexeme();
}

void binOp() {
  layer3 = "binary operator; ";
  std::ifstream binOps;
  binOps.open("binOp.txt");
  std::string current_oprator;
  while (!binOps.eof()) {
    binOps >> current_oprator;
    if (cur == current_oprator) {
      op.push(cur.lex);
      if (cur.lex == "=") {
        pol.push_back(poliz_unit(" ref", 1));
      }
      dump_with_priority(opers, pol, cur.lex);
      opers.push(poliz_unit(cur.lex, 1));
      next_lexeme();
      return;
    }
  }
  throw unexpected_lexeme_error("binary operator");
}

void unOp() {
  layer3 = "unanry operator; ";
  std::ifstream unOps;
  unOps.open("unOp.txt");
  std::string current_oprator;
  while (!unOps.eof()) {
    unOps >> current_oprator;
    if (cur == current_oprator) {
      op.push(cur.lex);
      // check_unop();
      dump_with_priority(opers, pol, "__?unary?operator__DOES_NOTHING__");
      opers.push(poliz_unit(cur.lex, 1));
      next_lexeme();
      return;
    }
  }
  throw unexpected_lexeme_error("unary operator");
}

//  types block begin

void type();

bool is_const = false;

void non_array_type() {
  layer3 = "non-array type; ";
  std::ifstream types;
  types.open("types.txt");
  std::string current_type;
  while (!types.eof()) {
    types >> current_type;
    if (cur == current_type) {
      if (is_const) {
        alltids->type.push_back("const" + cur.lex);
        is_const = false;
      } else {
        alltids->type.push_back(cur.lex);
      }
      next_lexeme();
      return;
    }
  }
  throw unexpected_lexeme_error("non-array type");
}

void array_type() {
  layer3 = "array type; ";
  if (cur != "array") throw unexpected_lexeme_error("array");
  next_lexeme();
  expect("<");
  next_lexeme();
  type();
  alltids->type[alltids->type.size() - 1] += " array";
  layer3 = "array type; ";
  expect(">");
  next_lexeme();
  expect("[");
  next_lexeme();
  opers.push(poliz_unit("(", 0));
  exp_token();
  dump_until(opers, pol, "(");
  opers.pop();
  layer3 = "";
  expect("]");
  next_lexeme();
}

void darray_type() {
  layer3 = "darray type; ";
  if (cur != "darray") throw unexpected_lexeme_error("darray");
  next_lexeme();
  expect("<");
  next_lexeme();
  type();
  alltids->type[alltids->type.size() - 1] += " darray";
  layer3 = "darray type; ";
  expect(">");
  next_lexeme();
}

void string_type() {
  layer3 = "string type; ";
  if (cur != "string") throw unexpected_lexeme_error("string");
  alltids->type.push_back("string");
  next_lexeme();
  if (cur != "[") return;
  next_lexeme();
  opers.push(poliz_unit("(", 0));
  exp_token();
  dump_until(opers, pol, "(");
  opers.pop();
  layer3 = "string type; ";
  expect("]");
  next_lexeme();
}

void type() {
  layer3 = "type; ";
  if (cur.type != reserved) throw unexpected_lexeme_error("type");
  if (cur == "string") {
    string_type();
    layer3 = "type; ";
    if (cur == "*") {
      alltids->type[alltids->type.size() - 1] += "*";
      next_lexeme();
    }
    if (cur == "&") {
      alltids->type[alltids->type.size() - 1] += "&";
      next_lexeme();
    }

  } else if (cur == "array") {
    array_type();
    layer3 = "type; ";
    if (cur == "*") next_lexeme();
    if (cur == "&") next_lexeme();
  } else if (cur == "darray") {
    darray_type();
    layer3 = "type; ";
    if (cur == "*") next_lexeme();
    if (cur == "&") next_lexeme();
  } else if (cur == "const") {
    next_lexeme();

    if (cur == "const") throw not_allowed_error("double const");
    type();
    if (cur == "*") next_lexeme();
    if (cur == "&") next_lexeme();
  } else {
    non_array_type();
    layer3 = "type; ";
    if (cur == "*") next_lexeme();
    if (cur == "&") next_lexeme();
  }
}

//  types block end

bool map_created_from_call = 0;

void exp_token() {
  map_created_from_call = 0;
  // layer2 = "expression token; ";

  if (cur.type == string) {
    op.push("string");
    std::string* tmp_pointer = new std::string;
    *tmp_pointer = cur.lex;
    tmp_pointer->erase(tmp_pointer->begin());
    tmp_pointer->erase(tmp_pointer->end() - 1);
    consts.push_back(tmp_pointer);
    pol.push_back(poliz_unit("string", 3, consts.back()));
    next_lexeme();
    if (cur.lex == "[") {
      next_lexeme();
      opers.push((poliz_unit("(", 0)));
      exp_token();
      dump_until(opers, pol, "(");
      opers.pop();
      pol.push_back(poliz_unit("[]", 1));
      expect("]");
      next_lexeme();
    }
    if (cur.type == operat) {
      binOp();
      exp_token();
      check_op();
      return;
    }
    return;
  }

  if (cur.type == num) {
    // ARTEM! what if its a double here
    if (cur.lex.find('.') != -1) {
      double* tmp_pointer = new double;
      *tmp_pointer = std::stod(cur.lex);
      consts.push_back(tmp_pointer);
      op.push("float64");
      pol.push_back(poliz_unit("float64", 3, consts.back()));
    } else {
      int64_t* tmp_pointer = new int64_t;
      *tmp_pointer = std::stoi(cur.lex);
      consts.push_back(tmp_pointer);
      op.push("int64");
      pol.push_back(poliz_unit("int64", 3, consts.back()));
    }

    next_lexeme();
    if (cur.type == operat) {
      binOp();
      exp_token();
      check_op();
      return;
    }
    return;
  }
  if (cur.type == id) {
    std::string funcname = cur.lex;
    next_lexeme();
    if (cur.type == operat) {
      pol.push_back(poliz_unit(funcname, 0));
      op.push(check_id(funcname));
      binOp();
      exp_token();
      check_op();
      return;
    }
    if (cur == "(") {
      check_func_id(funcname);
      opers.push(poliz_unit(funcname, 1));
      next_lexeme();
      pol.push_back(poliz_unit(" pushmap", 1));
      pol.push_back(poliz_unit(" return_value", 4));
      pol.push_back(
          poliz_unit(func_tid.returning_type[check_func_id(funcname)], 1));
      map_created_from_call = 1;
      int count = 0;
      while (cur != ")") {
        pol.push_back(poliz_unit(
            func_tid.parameter_name[check_func_id(funcname)][count], 4));
        pol.push_back(
            poliz_unit(func_tid.parametr[check_func_id(funcname)][count], 1));
        pol.push_back(poliz_unit(" dcb", 1));  // dcb - deep check begin
        exp_token();
        dump_until(opers, pol, funcname);
        pol.push_back(poliz_unit(" dce", 1));  // dce - deep check end
        pol.push_back(poliz_unit(" ch=", 5));
        count++;
        if (cur != ")")
          expect(",");
        else
          break;
        next_lexeme();
      }
      pol.push_back(poliz_unit(" returnto", 0));
      int64_t* tmp_pa = new int64_t;
      *tmp_pa = pol.size() + 4;
      consts.push_back(tmp_pa);
      pol.push_back(
          poliz_unit(" pa", 2, tmp_pa));  // p a stands for poliz adress
      pol.push_back(poliz_unit("=", 1));
      tmp_pa = new int64_t;
      *tmp_pa = find_func_place(funcname);
      consts.push_back(tmp_pa);
      pol.push_back(poliz_unit(" pa", 2, tmp_pa));
      pol.push_back(poliz_unit(" !j", 1));
      opers.pop();
      if (count != func_tid.parametr[check_func_id(funcname)].size()) {
        throw not_enough_operands_error(funcname);
      }
      next_lexeme();
      check_parametrs(funcname);
      return;
    }
    if (cur == ")") {
      pol.push_back(poliz_unit(funcname, 0));
      op.push(check_id(funcname));
      return;
    }
    if (cur == "[") {  // Artem, please, fix dat branch
      // dat branch is SOOOO fucked up;
      pol.push_back(poliz_unit(funcname, 0));
      while (cur == "[") {
        next_lexeme();
        op.push(check_id(funcname));
        op.push("[]");
        exp_token();
        check_op();
        expect("]");
        next_lexeme();
        pol.push_back(poliz_unit("[]", 1));
      }
      if (cur.type == operat) {  // Artem, please, fix dat branch;
        dump_with_priority(opers, pol, cur.lex);
        op.push(check_id(funcname));
        op.push(cur.lex);
        opers.push(poliz_unit(cur.lex, 1));
        next_lexeme();
        exp_token();
        check_op();
        return;
      }
      return;
    }
    op.push(check_id(funcname));
    pol.push_back(poliz_unit(funcname, 0));
    return;
  }
  if (cur == "(") {
    next_lexeme();
    opers.push(poliz_unit("(", 0));
    exp_token();
    expect(")");
    dump_until(opers, pol, "(");
    opers.pop();
    next_lexeme();
    if (cur.type == operat) {
      // opers.push(poliz_unit(cur.lex, 1));
      binOp();
      exp_token();
      check_op();
      return;
    }
    return;
  }
  if (cur.type == operat) {
    unOp();
    if (cur == "(") {
      opers.push(poliz_unit("(", 0));
      next_lexeme();
      exp_token();
      expect(")");
      dump_until(opers, pol, "(");
      opers.pop();

      next_lexeme();
      if (cur.type == operat) {  // ARTEM! Check this branch please, semantix
                                 // has smth to be added
        dump_with_priority(opers, pol, cur.lex);
        opers.push(poliz_unit(cur.lex, 1));
        next_lexeme();
        exp_token();
      }
      check_unop();
      return;
    }
    if (cur.type == id || cur.type == num || cur.type == string) {
      if (cur.type == num) {
        op.push("int64");
        int64_t* tmp_p = new int64_t;
        *tmp_p = std::stoi(cur.lex);
        consts.push_back(tmp_p);
        pol.push_back(poliz_unit("int64", 2, tmp_p));
      }
      if (cur.type == string) {
        op.push("string");
        std::string* tmp_p = new std::string;
        tmp_p->erase(tmp_p->begin());
        tmp_p->pop_back();
        consts.push_back(tmp_p);
        pol.push_back(poliz_unit("string", 2, tmp_p));
      }
      if (cur.type == id) {
        op.push(check_id(cur.lex));
        pol.push_back(poliz_unit(cur.lex, 0));
      }

      // pol.push_back(poliz_unit(cur.lex, 0));
      next_lexeme();
      check_unop();
      if (cur.type == operat) {
        binOp();
        // next_lexeme();
        exp_token();
        return;
      } else if (cur.type != punct)
        throw unexpected_lexeme_type_error("punctuation mark");
      return;
    }
    if (cur.type == operat) {
      exp_token();
      return;
    }
    return;
  }
  throw unexpected_lexeme_error("expression");
}

// goto & children block begin

void if_operator() {
  layer2 = "if operator; ";
  layer3 = "";
  expect("if");
  int to_replace_pa;
  // layer2 = "if operator :";
  next_lexeme();
  expect("(");
  next_lexeme();
  opers.push(poliz_unit("(", 0));
  exp_token();
  dump_until(opers, pol, "(");
  opers.pop();
  to_replace_pa = pol.size();
  pol.push_back(poliz_unit(" epa", 0));  // epa - empty poliz adress
  pol.push_back(poliz_unit(" !?j", 1));
  check_bool();
  layer3 = "";
  expect(")");
  next_lexeme();
  block();
  pol.push_back(poliz_unit(" epa", 0));
  pol.push_back(poliz_unit(" !j", 1));
  int64_t* tmp_pa = new int64_t;
  *tmp_pa = pol.size();
  consts.push_back(tmp_pa);
  pol[to_replace_pa].val = " pa";
  pol[to_replace_pa].type = 2;
  pol[to_replace_pa].const_val = tmp_pa;
  to_replace_pa = pol.size() - 2;
  if (cur == "else") {
    next_lexeme();
    block();
  }
  tmp_pa = new int64_t;
  *tmp_pa = pol.size();
  consts.push_back(tmp_pa);
  pol[to_replace_pa].val = " pa";
  pol[to_replace_pa].type = 2;
  pol[to_replace_pa].const_val = tmp_pa;
}

std::vector<std::pair<std::string, int>> labels_places;

int find_label_poliz_adress(const std::string& name_p) {
  for (int co = 0; co < labels_places.size(); ++co) {
    if (labels_places[co].first == name_p) return labels_places[co].second;
  }
  throw unknown_id_error(name_p, "label");
}

void goto_operator() {
  layer3 = "goto; ";
  expect("goto");
  next_lexeme();
  pol.push_back(
      poliz_unit(std::to_string(find_label_poliz_adress(cur.lex)), 0));
  pol.push_back(poliz_unit(" !j", 1));
  if (cur.type != id) throw unexpected_lexeme_type_error("label name");
  next_lexeme();
}

void label_declaration() {
  layer3 = "label; ";
  expect("label");
  next_lexeme();
  if (cur.type != id) throw unexpected_lexeme_type_error("id");
  labels_places.push_back(std::make_pair(cur.lex, pol.size()));
  next_lexeme();
}

void while_op() {
  layer2 = "while operator; ";
  layer2 = "";
  expect("while");
  int to_replace_pa, to_return_while;
  // layer1 = "while operator: ";
  next_lexeme();
  expect("(");
  next_lexeme();
  to_return_while = pol.size();
  pol.push_back(poliz_unit(" pushmap", 1));
  opers.push(poliz_unit("(", 0));
  exp_token();
  to_replace_pa = pol.size();
  dump_until(opers, pol, "(");
  opers.pop();
  pol.push_back(poliz_unit(" epa", 0));
  pol.push_back(poliz_unit(" !?j", 1));
  check_bool();
  layer3 = "";
  expect(")");
  next_lexeme();
  block();
  int64_t* tmp_pa = new int64_t;
  *tmp_pa = to_return_while;
  consts.push_back(tmp_pa);
  pol.push_back(poliz_unit(" pa", 2, tmp_pa));
  pol.push_back(poliz_unit(" !j", 1));
  tmp_pa = new int64_t;
  consts.push_back(tmp_pa);
  pol[to_replace_pa].val = " pa";
  pol[to_replace_pa].type = 2;
  pol[to_replace_pa].const_val = tmp_pa;
}

void for_op() {
  //  for(e1; e2; e3) {e4;}
  layer2 = "for operator; ";
  layer3 = "";
  expect("for");
  next_lexeme();
  expect("(");
  next_lexeme();
  if (cur.type == reserved)
    type_name(0);
  else {
    opers.push(poliz_unit("(", 0));
    exp_token();
    dump_until(opers, pol, "(");
    opers.pop();
  }
  pol.push_back(poliz_unit(" pushmap", 1));
  int e2 = pol.size();
  layer3 = "";
  expect(";");
  next_lexeme();
  opers.push(poliz_unit("(", 0));
  exp_token();
  check_bool();
  dump_until(opers, pol, "(");
  opers.pop();
  int to_replace_pa = pol.size();
  pol.push_back(poliz_unit(" epa", 0));
  pol.push_back(poliz_unit(" !?j", 1));
  int e4_jump = pol.size();
  pol.push_back(poliz_unit(" epa", 0));
  pol.push_back(poliz_unit(" !j", 1));
  layer3 = "";
  expect(";");
  int e3 = pol.size();
  next_lexeme();
  opers.push(poliz_unit("(", 0));
  exp_token();
  dump_until(opers, pol, "(");
  opers.pop();
  layer3 = "";
  int64_t* tmp_pa = new int64_t;
  *tmp_pa = e2;
  consts.push_back(tmp_pa);
  pol.push_back(poliz_unit(" pa", 2, tmp_pa));
  pol.push_back(poliz_unit(" !j", 1));
  expect(")");
  next_lexeme();
  tmp_pa = new int64_t;
  consts.push_back(tmp_pa);
  *tmp_pa = pol.size();
  pol[e4_jump].val = " pa";
  pol[e4_jump].type = 2;
  pol[e4_jump].const_val = tmp_pa;
  block(0, 0);
  tmp_pa = new int64_t;
  *tmp_pa = e3;
  consts.push_back(tmp_pa);
  pol.push_back(poliz_unit(" pa", 2, tmp_pa));
  pol.push_back(poliz_unit(" !j", 1));
  tmp_pa = new int64_t;
  consts.push_back(tmp_pa);
  *tmp_pa = pol.size();
  pol[to_replace_pa].val = " pa";
  pol[to_replace_pa].const_val = tmp_pa;
  pol[to_replace_pa].type = 2;
  layer2 = "";
}

void do_while_operator() {
  layer2 = "do while operator; ";
  layer3 = "";
  expect("do");
  next_lexeme();
  pol.push_back(poliz_unit(" pushmap", 1));
  int block_begin = pol.size();
  block(0, 0);
  layer3 = "";
  expect("while");
  next_lexeme();
  expect("(");
  next_lexeme();
  opers.push(poliz_unit("(", 0));
  exp_token();
  dump_until(opers, pol, "(");
  opers.pop();
  int to_replace = pol.size();
  pol.push_back(poliz_unit(" epa", 0));
  pol.push_back(poliz_unit(" !?j", 1));
  int64_t* tmp_pa = new int64_t;
  consts.push_back(tmp_pa);
  *tmp_pa = block_begin;
  pol.push_back(poliz_unit(" pa", 2, tmp_pa));
  pol.push_back(poliz_unit(" !j", 1));
  tmp_pa = new int64_t;
  *tmp_pa = pol.size();
  consts.push_back(tmp_pa);
  pol[to_replace].val = " pa";
  pol[to_replace].type = 2;
  pol[to_replace].const_val = tmp_pa;
  check_bool();
  layer3 = "";
  expect(")");
  next_lexeme();
}

void return_operator() {
  layer2 = "return operator";
  layer3 = "";
  expect("return");
  next_lexeme();

  pol.push_back(poliz_unit(" return_value", 0));
  pol.push_back(poliz_unit(" ref", 1));
  if (cur == ";") {
    pol.push_back(poliz_unit(" returnto", 0));
    pol.push_back(poliz_unit(" !j", 1));
    if (returning_type_now != "void") {
      throw wrong_returning_type_error(returning_type_now, "void");
    }
    return;
  }
  opers.push(poliz_unit("(", 0));
  exp_token();
  dump_until(opers, pol, "(");
  opers.pop();
  pol.push_back(poliz_unit("=", 1));
  pol.push_back(poliz_unit(" popmap", 1));
  pol.push_back(poliz_unit(" returnto", 0));
  pol.push_back(poliz_unit(" !j", 1));
  std::string s = returning_type_now;
  std::string w = op.top();
  if (op.top() == "int" || op.top() == "int16" || op.top() == "int64" ||
      op.top() == "constint" || op.top() == "constint16" ||
      op.top() == "constint64") {
    if (!(returning_type_now == "int" || returning_type_now == "int16" ||
          returning_type_now == "int64" || returning_type_now == "constint" ||
          returning_type_now == "constint16" ||
          returning_type_now == "constint64")) {
      throw wrong_returning_type_error(returning_type_now, op.top());
    }
  } else if (op.top() == "float" || op.top() == "float16" ||
             op.top() == "float64" || op.top() == "constfloat" ||
             op.top() == "constfloat16" || op.top() == "constfloat64") {
    if (!(returning_type_now == "int" || returning_type_now == "int16" ||
          returning_type_now == "int64" || returning_type_now == "float" ||
          returning_type_now == "float16" || returning_type_now == "float64" ||
          returning_type_now == "constint" ||
          returning_type_now == "constint16" ||
          returning_type_now == "constint64" ||
          returning_type_now == "constfloat" ||
          returning_type_now == "constfloat16" ||
          returning_type_now == "constfloat64")) {
      throw wrong_returning_type_error(returning_type_now, op.top());
    }
  } else if (op.top() != returning_type_now) {
    throw wrong_returning_type_error(returning_type_now, op.top());
  }
}

// goto & children end

class service_error : public error {
 public:
  service_error(const std::string& addition_p) { addition = addition_p; }
};

void goto_and_children() {
  layer3 = "";
  if (cur == "return")
    return_operator();
  else if (cur == "while")
    while_op();
  else if (cur == "do")
    do_while_operator();
  else if (cur == "for")
    for_op();
  else if (cur == "goto")
    goto_operator();
  else if (cur == "label")
    label_declaration();
  else if (cur == "if")
    if_operator();
  else {
    layer2 = "goto and children; ";
    throw service_error("goto or a child");
  }
  // next_lexeme();
}

void st_in() {
  layer2 = "stin: ";
  layer3 = "";
  expect("stin");
  next_lexeme();
  expect("(");
  next_lexeme();
  opers.push(poliz_unit("(", 0));
  /*exp_token();
  dump_until(opers, pol, "(");
  pol.push_back(poliz_unit(" format in", 1));*/
  // opers.pop();
  layer3 = "";
  // expect(",");
  exp_token();
  dump_until(opers, pol, "(");
  int counter = 1;
  while (cur == ",") {
    next_lexeme();
    // if (cur.type != id) throw unexpected_lexeme_type_error("id");
    exp_token();
    dump_until(opers, pol, "(");
    ++counter;
  }
  opers.pop();
  int64_t* tmp_p = new int64_t;
  *tmp_p = counter;
  consts.push_back(tmp_p);
  pol.push_back(poliz_unit("int64", 2, tmp_p));
  pol.push_back(poliz_unit(" stin", 1));
  expect(")");
  next_lexeme();
}

void st_out() {
  layer2 = "stout";
  layer3 = "";
  expect("stout");
  next_lexeme();
  expect("(");
  next_lexeme();
  opers.push(poliz_unit("(", 0));
  exp_token();
  dump_until(opers, pol, "(");
  pol.push_back(poliz_unit(" format out", 1));
  layer3 = "";
  expect(",");
  while (cur == ",") {
    next_lexeme();
    exp_token();
    dump_until(opers, pol, "(");
    layer3 = "";
  }
  opers.pop();
  expect(")");
  pol.push_back(poliz_unit("stout", 1));
  next_lexeme();
}

//  declarations, definitions etc. block begin

void const_init() {
  layer2 = "const initialization; ";
  expect("=");
  next_lexeme();
  exp_token();
  layer2 = "const initialization; ";
  if (cur == ",") {
    next_lexeme();
    if (cur.type != id) throw unexpected_lexeme_type_error("id");
    pol.push_back(poliz_unit(cur.lex, 4));
    pol.push_back(poliz_unit(alltids->type.back(), 1));
    next_lexeme();
    const_init();
  }
}

void non_const_init() {
  layer3 = "non const initialization; ";
  if (cur == "=") {
    op.push("=");
    opers.push(poliz_unit("=", 1));
    next_lexeme();
    std::stack<std::string> we = op;
    exp_token();
    we = op;
    check_op();
    layer3 = "non const initialization; ";
    dump_all(opers, pol);
    non_const_init();
  } else if (cur == ",") {
    dump_all(opers, pol);
    next_lexeme();
    if (cur.type != id) throw unexpected_lexeme_type_error("id");
    alltids->name.push_back(cur.lex);
    alltids->type.push_back(alltids->type.back());
    pol.push_back(poliz_unit(cur.lex, 4));
    pol.push_back(poliz_unit(alltids->type.back(), 1));
    next_lexeme();
    non_const_init();
  } else
    expect(";");
}

bool in_func = false;

void parameters() {
  layer2 = "parameters";
  layer3 = "";
  type();
  layer3 = "";
  if (cur.type != id) throw unexpected_lexeme_type_error("id");
  if (func_tid.parametr.size() < func_tid.name.size())
    func_tid.parametr.push_back(std::vector<std::string>()),
        func_tid.parameter_name.push_back(std::vector<std::string>());
  func_tid.parametr[func_tid.name.size() - 1].push_back(
      alltids->type[alltids->type.size() - 1]);
  func_tid.parameter_name[func_tid.name.size() - 1].push_back(cur.lex);
  alltids->name.push_back(cur.lex);
  next_lexeme();
  if (cur == ",") {
    next_lexeme();
    parameters();
  }
}

bool function_block = 0;

void function_description() {
  was_in_function = function_block = 1;
  in_func = true;
  prev_lexeme();
  layer1 = "function (" + cur.lex + ")";
  if (check_func_id(cur.lex) + 1)
    funcs.push_back(func_place(cur.lex, current_pol_index));
  alltids->child.push_back(new tid(alltids));
  alltids = alltids->child[alltids->child.size() - 1];
  next_lexeme();
  next_lexeme();
  if (cur != ")") {
    parameters();
  } else {
    std::vector<std::string> empty_parameters(0);
    func_tid.parametr.push_back(empty_parameters);
  }
  expect(")");
  next_lexeme();

  block();
  pol.push_back(poliz_unit(" returnto", 0));
  pol.push_back(poliz_unit(" !j", 1));
  in_func = false;
}

void type_name(bool func_allowed) {  // type_name checks if name and type are
                                     // correct and proceeds further
  // layer1 = "global; ";
  // was_in_function = 0;
  std::string tmp;
  if (cur == "const") {
    next_lexeme();
    if (cur == "const") throw not_allowed_error("double const");
    type();
    if (cur.type != id) throw unexpected_lexeme_type_error("id");
    opers.push(poliz_unit(alltids->type.back(), 1));
    // tmp = cur.lex;
    check_var_conflict(cur.lex);
    alltids->name.push_back(cur.lex);
    pol.push_back(poliz_unit(cur.lex, 4));
    next_lexeme();
    pol.push_back(opers.top());
    opers.pop();
    const_init();
    return;
  }
  type();
  if (cur.type != id) throw unexpected_lexeme_type_error("id");
  tmp = cur.lex;
  check_var_conflict(cur.lex);
  alltids->name.push_back(tmp);
  next_lexeme();
  if (cur == "(")
    if (func_allowed) {
      func_tid.name.push_back(alltids->name[alltids->name.size() - 1]);
      alltids->name.pop_back();
      func_tid.returning_type.push_back(
          alltids->type.back() /*alltids->name[alltids->type.size() - 1]*/);
      alltids->type.pop_back();
      function_description();
    } else
      throw not_allowed_error("function definition");
  else {
    pol.push_back(poliz_unit(alltids->name.back(), 4));
    pol.push_back(poliz_unit(alltids->type.back(), 1));
    if (cur == "," || cur == "=") {
      prev_lexeme();
      prev_lexeme();
      op.push(alltids->type[alltids->type.size() - 1]);
      next_lexeme();
      next_lexeme();
      non_const_init();
    }
  }
}

// declarations, definitons etc. block end

void operator_or_exp() {
  // expect("{");
  // next_lexeme();
  if (cur == "return") {
    return_operator();  // yeah yeah, i know that's ugly, but i just want to be
    return;             // sure that the correct function will be called
  }
  if (cur.type == id) {
    exp_token();
    return;
  }
  if (cur.type == reserved) {
    if (cur == "stin") {
      st_in();
      return;
    }
    if (cur == "stout") {
      st_out();
      return;
    }
    try {
      goto_and_children();
    } catch (service_error err) {
      if (err.addition != "goto or a child") throw err;
      type_name(0);
    }
  } else if (cur.type == operat || cur.type == id || cur.type == num ||
             cur.type == string)
    exp_token();
  else
    throw unexpected_lexeme_error("operator or experession");
}

bool is_reserved_operator(
    const std::string& str) {  // check block() for more information
  std::ifstream resOp;
  resOp.open("reserved_operators.txt");
  std::string current_operator;
  while (!resOp.eof()) {
    resOp >> current_operator;
    if (str == current_operator) return 1;
  }
  return 0;
}

void block(bool in_main, bool to_push_map) {
  bool in_f_block = function_block;  // just to be sure
  if (!function_block && to_push_map)
    pol.push_back(poliz_unit(" pushmap", 1)), function_block = 0;
  layer3 = "";
  expect("{");
  if (!in_func) {
    alltids->child.push_back(new tid(alltids));
    alltids = alltids->child.back();
  }
  next_lexeme();
  std::string cur_check;  // explanation below
  while (cur != "}") {
    cur_check = cur.lex;
    if (!is_reserved_operator(cur_check)) {
      opers.push(poliz_unit("(", 0));
    }
    operator_or_exp();
    layer3 = "";
    // expect(;);
    // well, i kinda fucked up here
    // i should have placed while, for, if etc. in a separate file for such
    // opers i didnt though that causes problems like checks for ';' after them
    // so, they are in a separate file, but 9.3.2021 19:37
    // this file is only used here to check wheher ';' is needeed
    // file's name is reserved_operators.txt
    if (!is_reserved_operator(cur_check)) {
      expect(";");
      dump_until(opers, pol, "(");
      next_lexeme();
    }
  }
  alltids = alltids->parent;
  tid t = alltids;
  if (!in_f_block && to_push_map) {
    pol.push_back(poliz_unit(" popmap", 1));
  }
  if (in_main) return;
  next_lexeme();
}

void program() {
  layer1 = "global";
  layer2 = layer3 = "";
  pol.push_back(poliz_unit(" pushmap", 1));
  if (cur == "int") {
    next_lexeme();
    if (cur == "main") {
      layer1 = "int main; ";
      returning_type_now = "int";
      layer2 = layer3 = "";
      next_lexeme();
      expect("(");
      next_lexeme();
      expect(")");
      next_lexeme();

      pol.push_back(poliz_unit(" pushmap", 1));
      start_of_the_main = pol.size() - 1;
      block(1, 0);
      if (current_index != lexemes.size() - 1)
        throw not_allowed_error("anything after int main()");
    } else {
      was_in_function = 0;
      prev_lexeme();
      returning_type_now = cur.lex;
      type_name();
      if (!was_in_function) expect(";"), next_lexeme();
      program();
    }
  } else {
    was_in_function = 0;
    returning_type_now = cur.lex;
    type_name();
    if (!was_in_function) {
      expect(";");
      next_lexeme();
    }
    program();
  }
}

class typep {
 public:
  std::string type_name;
  std::string name;
  void* val;
  int size;
  typep(std::string var_type, std::string var_name, void* value) {
    type_name = var_type;
    name = var_name;
    val = value;
  }
  typep() { val = NULL; }
};

std::string formating_string;
int j = -1;
std::vector<std::vector<std::pair<std::string, typep>>> memory;
std::stack<std::pair<void*, std::string>> values;

typep key_search(std::string key, int c) {
  for (int i = 0; i < memory[c].size(); ++i) {
    if (memory[c][i].first == key) {
      return memory[c][i].second;
    }
  }
}
bool check_key(std::string key, int c) {
  for (int i = 0; i < memory[c].size(); ++i) {
    if (memory[c][i].first == key) {
      return true;
    }
  }
  return false;
}

#include <algorithm>
void get_var(int i) {
  int c = j;
  while (c > 0 && !check_key(pol[i].val, c)) {
    c--;
  }
  values.push(std::pair<void*, std::string>(
      key_search(pol[i].val, c).val, key_search(pol[i].val, c).type_name));
}

void running_program() {
  int i = start_of_the_main;

  std::vector<poliz_unit> pol1 = pol;

  while (i < pol.size()) {
    if (pol[i].type == 1) {
      if (pol[i].val == "+") {
        std::pair<void*, std::string> one = values.top();
        values.pop();
        std::pair<void*, std::string> two = values.top();
        values.pop();
        if (one.second == "string") {
          std::string* tmp = new std::string;
          *tmp = *static_cast<std::string*>(one.first) +
                 *static_cast<std::string*>(two.first);
          void* a = tmp;
          values.push(std::pair<void*, std::string>(a, "string"));
        }
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            *tmp = *static_cast<long long int*>(one.first) +
                   *static_cast<long long int*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            *tmp = *static_cast<long long int*>(one.first) +
                   *static_cast<double*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "float64"));
          }
        }
        if (one.second == "float64" || one.second == "float") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            double* tmp = new double;
            *tmp = *static_cast<double*>(one.first) +
                   *static_cast<long long int*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "float64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            *tmp = *static_cast<double*>(one.first) +
                   *static_cast<double*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "float64"));
          }
        }

        ++i;
        continue;
      }
      if (pol[i].val == "*") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            *tmp = *static_cast<long long int*>(one.first) *
                   *static_cast<long long int*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            *tmp = *static_cast<long long int*>(one.first) *
                   *static_cast<double*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "float64"));
          }
        }
        if (one.second == "float64" || one.second == "float") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            double* tmp = new double;
            *tmp = *static_cast<double*>(one.first) *
                   *static_cast<long long int*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "float64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            *tmp = *static_cast<double*>(one.first) *
                   *static_cast<double*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "float64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == "/") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            *tmp = *static_cast<long long int*>(one.first) /
                   *static_cast<long long int*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            *tmp = *static_cast<long long int*>(one.first) /
                   *static_cast<double*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "float64"));
          }
        }
        if (one.second == "float64" || one.second == "float") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            double* tmp = new double;
            *tmp = *static_cast<double*>(one.first) /
                   *static_cast<long long int*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "float64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            *tmp = *static_cast<double*>(one.first) /
                   *static_cast<double*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "float64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == "%") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            *tmp = *static_cast<long long int*>(one.first) %
                   *static_cast<long long int*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == "&&") {
        std::pair<void*, std::string> one = values.top();
        values.pop();
        std::pair<void*, std::string> two = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            if (*static_cast<long long int*>(one.first) > 0 &&
                *static_cast<long long int*>(two.first) > 0) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == " ref") {
        ++i;
        continue;
      }
      if (pol[i].val == "||") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            if (*static_cast<long long int*>(one.first) > 0 ||
                *static_cast<long long int*>(two.first) > 0) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        ++i;
        continue;
      }

      if (pol[i].val == ">") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            if (*static_cast<long long int*>(one.first) >
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<long long int*>(one.first) >
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        if (one.second == "float64" || one.second == "float") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) >
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) >
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == ">=") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            if (*static_cast<long long int*>(one.first) >=
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<long long int*>(one.first) >=
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        if (one.second == "float64" || one.second == "float") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) >=
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) >=
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == "<") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            if (*static_cast<long long int*>(one.first) <
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<long long int*>(one.first) <
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        if (one.second == "float64" || one.second == "float") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) <
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) <
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == "<=") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            if (*static_cast<long long int*>(one.first) <=
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<long long int*>(one.first) <=
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        if (one.second == "float64" || one.second == "float") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) <=
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) <=
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == "==") {
        std::pair<void*, std::string> one = values.top();
        values.pop();
        std::pair<void*, std::string> two = values.top();
        values.pop();
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            if (*static_cast<long long int*>(one.first) ==
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<long long int*>(one.first) ==
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        if (one.second == "float64" || one.second == "float") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) ==
                *static_cast<long long int*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) ==
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == "=") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "string" && two.second == "string") {
          std::string* tmp = new std::string;
          *tmp = *static_cast<std::string*>(two.first);
          *static_cast<std::string*>(one.first) =
              *static_cast<std::string*>(two.first);
          void* a = tmp;
          values.push(std::pair<void*, std::string>(a, "string"));
        }
        if (one.second == "adress" && two.second == "adress") {
          *static_cast<int*>(one.first) = *static_cast<int*>(two.first);
        }
        if (one.second == "int64" || one.second == "int" ||
            one.second == "int16") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            long long int* tmp = new long long int;
            *tmp = *static_cast<long long int*>(two.first);
            *static_cast<long long int*>(one.first) =
                *static_cast<long long int*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            *tmp = *static_cast<long long int*>(two.first);
            *static_cast<long long int*>(one.first) =
                *static_cast<double*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        if (one.second == "float64" || one.second == "float") {
          if (two.second == "int64" || two.second == "int" ||
              two.second == "int16") {
            double* tmp = new double;
            *tmp = *static_cast<long double*>(two.first);
            *static_cast<long long int*>(one.first) =
                *static_cast<long long int*>(two.first);
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
          if (two.second == "float" || two.second == "float64") {
            double* tmp = new double;
            if (*static_cast<double*>(one.first) <=
                *static_cast<double*>(two.first)) {
              *tmp = 1;
            } else {
              *tmp = 0;
            }
            void* a = tmp;
            values.push(std::pair<void*, std::string>(a, "int64"));
          }
        }
        ++i;
        continue;
      }
      if (pol[i].val == "++") {
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int" || one.second == "int16" ||
            one.second == "int64") {
          (*static_cast<long long int*>(one.first))++;
          values.push(std::pair<void*, std::string>(one.first, "int64"));
        }
        ++i;
        continue;
      }
      if (pol[i].val == "--") {
        std::pair<void*, std::string> one = values.top();
        values.pop();
        if (one.second == "int" || one.second == "int16" ||
            one.second == "int64") {
          (*static_cast<long long int*>(one.first))++;
          values.push(std::pair<void*, std::string>(one.first, "int64"));
        }
        if (one.second == "string") {
          (*static_cast<std::string*>(one.first)).pop_back();
          values.push(std::pair<void*, std::string>(one.first, "string"));
        }
      }

      if (pol[i].val == "[]") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        std::string* tmp = new std::string;
        *tmp = (*static_cast<std::string*>(
            one.first))[*static_cast<long long int*>(two.first)];
        void* a = tmp;
        values.push(std::pair<void*, std::string>(a, "string"));
        ++i;
        continue;
      }
      if (pol[i].val == " !j") {
        std::pair<void*, std::string> one = values.top();
        i = *static_cast<int*>(one.first);
        values.pop();
        continue;
      }
      if (pol[i].val == " !?j") {
        std::pair<void*, std::string> two = values.top();
        values.pop();
        std::pair<void*, std::string> one = values.top();
        values.pop();
        ++i;
        if (*static_cast<int64_t*>(one.first) <= 0) {
          i = *static_cast<int*>(two.first);
        }
        continue;
      }
      if (pol[i].val == " pushmap") {
        memory.push_back(std::vector<std::pair<std::string, typep>>());
        j++;
        memory[j].push_back(std::pair<std::string, typep>(
            " returnto", typep("adress", " returnto", new int)));
        ++i;
        continue;
      }
      if (pol[i].val == " popmap") {
        j--;
        memory.pop_back();
        ++i;
        continue;
      }

      if (pol[i].val == " format out") {
        std::pair<void*, std::string> one = values.top();
        values.pop();
        formating_string = *static_cast<std::string*>(one.first);
        ++i;
        continue;
      }
      if (pol[i].val == "stout") {
        std::string str = "";
        for (int j = formating_string.size() - 1; j >= 0; --j) {
          if (formating_string[j] == '\\') {
            --j;
            if (formating_string[j] == '%') {
              str = '%' + str;
            } else {
              str = '\\' + str;
            }
          } else if (formating_string[j] == '%') {
            --j;
            if (formating_string[j] == 'd') {
              std::pair<void*, std::string> two = values.top();
              values.pop();
              if (two.second == "int64" || two.second == "int" ||
                  two.second == "int16" || two.second == "float" ||
                  two.second == "float64") {
                str = std::to_string(*static_cast<long long int*>(two.first)) +
                      str;
              } else {
                // throw error;
              }
            } else if (formating_string[j] == 'f') {
              std::pair<void*, std::string> two = values.top();
              values.pop();
              if (two.second == "int64" || two.second == "int" ||
                  two.second == "int16" || two.second == "float" ||
                  two.second == "float64") {
                str = std::to_string(*static_cast<double*>(two.first)) + str;
              } else {
                // throw error;
              }
            } else if (formating_string[j] == 's') {
              std::pair<void*, std::string> two = values.top();
              values.pop();
              if (two.second == "string") {
                str = *static_cast<std::string*>(two.first) + str;
              } else {
                // throw error;
              }
            } else {
              str = '%' + str;
            }

          } else {
            str = formating_string[j] + str;
          }
        }
        std::cout << str;
        ++i;
        continue;
      }
      if (pol[i].val == "int") {
        memory[j].push_back(std::pair<std::string, typep>(
            values.top().second, typep("int", values.top().second, new int)));
        values.pop();
        values.push(std::pair<void*, std::string>(
            memory[j][memory[j].size() - 1].second.val,
            memory[j][memory[j].size() - 1].second.type_name));
        ++i;
        continue;
      }
      if (pol[i].val == "int64") {
        memory[j].push_back(std::pair<std::string, typep>(
            values.top().second,
            typep("int64", values.top().second, new long long int)));
        values.pop();
        values.push(std::pair<void*, std::string>(
            memory[j][memory[j].size() - 1].second.val,
            memory[j][memory[j].size() - 1].second.type_name));
        ++i;
        continue;
      }
      if (pol[i].val == "int16") {
        memory[j].push_back(std::pair<std::string, typep>(
            values.top().second,
            typep("int16", values.top().second, new short int)));
        values.pop();
        values.push(std::pair<void*, std::string>(
            memory[j][memory[j].size() - 1].second.val,
            memory[j][memory[j].size() - 1].second.type_name));
        ++i;
        continue;
      }
      if (pol[i].val == "float") {
        memory[j].push_back(std::pair<std::string, typep>(
            values.top().second,
            typep("float", values.top().second, new float)));
        values.pop();
        values.push(std::pair<void*, std::string>(
            memory[j][memory[j].size() - 1].second.val,
            memory[j][memory[j].size() - 1].second.type_name));
        ++i;
        continue;
      }
      if (pol[i].val == "float64") {
        memory[j].push_back(std::pair<std::string, typep>(
            values.top().second,
            typep("float64", values.top().second, new double)));
        values.pop();
        values.push(std::pair<void*, std::string>(
            memory[j][memory[j].size() - 1].second.val,
            memory[j][memory[j].size() - 1].second.type_name));
        ++i;
        continue;
      }
      if (pol[i].val == "string") {
        memory[j].push_back(std::pair<std::string, typep>(
            values.top().second,
            typep("string", values.top().second, new std::string)));
        values.pop();
        values.push(std::pair<void*, std::string>(
            memory[j][memory[j].size() - 1].second.val,
            memory[j][memory[j].size() - 1].second.type_name));
        ++i;
        continue;
      }
    }
    if (pol[i].type == 3 || pol[i].type == 2) {
      values.push(std::pair<void*, std::string>(pol[i].const_val, pol[i].val));
      ++i;
      continue;
    }
    if (pol[i].type == 0) {
      get_var(i);
      ++i;
      continue;
    }
    if (pol[i].type == 4) {
      values.push(std::pair<void*, std::string>(NULL, pol[i].val));
      ++i;
      continue;
    }
  }
  // 0 - var
  // 1 - operator
  // 2 - const
}

void break_down() {
  std::ifstream cde;
  cde.open("cde.txt");
  std::ofstream out;
  out.open("fortex.txt");
  std::string cur;
  std::string type, num_par, tmp;
  std::ifstream types;
  // types.open("types_txt");
  int co = 0;
  int cycl = -1;
  char cr;
  while (!cde.eof()) {
    type = "";
    ++cycl;
    std::cout << cycl;
    num_par = "";
    cur = "";
    co = 0;
    cr = ' ';
    while (cr != '\n') {
      cde.get(cr);
      cur.push_back(cr);
    }
    while (co < cur.size() && cur[co] != ' ') type += cur[co++];
    types.open("types.txt");
    if (co == cur.size()) continue;
    bool ok = 0;
    if (type == "struct" || type == "class") ok = 1;
    if (!ok) continue;
    while (co < cur.size() && cur[co] != '{') num_par += cur[co++];
    if (co == cur.size() && num_par.back() != '(') continue;
    num_par.back() = '\\';
    num_par.push_back('{');
    out << "\\item { \n";
    out << type << " " << num_par << '\n';
    while (cur != "};") {
      cde >> cur;
      if (cur != "};") {
        out << cur;
        std::getline(cde, cur);
        out << ' ' << cur << '\n';
      }
    }
    out << '}';
    out << "\n";
  }
}

/*
\itemint  find_func_place(const std::string& func_name_p)
\itemvoid  dump_all(std::stack<poliz_unit>& opers, std::vector<poliz_unit>& pol)
\itemint  check_func_id(std::string name)
\itemvoid  check_func_conflict(const std::string& name)
\itemvoid  check_func_op(std::string name)
\itemvoid  check_var_conflict(std::string name)
\itemint  get_casting_prior(std::string type)
\itemvoid  check_parametrs(std::string name)
\itemvoid  check_op()
\itemvoid  check_unop()
\itemvoid  check_bool()
\itemvoid  next_lexeme()
\itemvoid  prev_lexeme()
\itemvoid  name()
\itemvoid  num_const()
\itemvoid  binOp()
\itemvoid  unOp()
\itemvoid  non_array_type()
\itemvoid  array_type()
\itemvoid  darray_type()
\itemvoid  string_type()
\itemvoid  type()
\itemvoid  exp_token()
\itemvoid  if_operator()
\itemint  find_label_poliz_adress(const std::string& name_p)
\itemvoid  goto_operator()
\itemvoid  label_declaration()
\itemvoid  while_op()
\itemvoid  for_op()
\itemvoid  do_while_operator()
\itemvoid  return_operator()
\itemvoid  goto_and_children()
\itemvoid  st_in()
\itemvoid  st_out()
\itemvoid  const_init()
\itemvoid  non_const_init()
\itemvoid  parameters()
\itemvoid  function_description()
\itemvoid  type_name(bool func_allowed)
\itemvoid  operator_or_exp()
\itemvoid  block(bool in_main, bool to_push_map)
\itemvoid  program()
\itembool  check_key(std::string key, int c)
\itemvoid  get_var(int i)
\itemvoid  running_program()
\itemvoid  break_down()
\itemint  main()
\itemint  main()

*/

int main() {
  break_down();
  exit(0);
  program_stream.open("code.txt");
  std::getline(program_stream, program_text, '\0');
  std::vector<std::pair<std::string, states_num>> lex_res;
  lex_res = devide_by_leksems(program_text);
  lexemes.resize(lex_res.size());
  for (int co = 0; co < lex_res.size(); ++co) {
    lexemes[co].lex = lex_res[co].first;
    lexemes[co].type = lex_res[co].second;
  }
  cur = lexemes[0];
  // working with lexemes as a struct is way more convenient than working with a
  // pair because with pair you never know what is first and what is second

  // syntax analysis begin

  // this piece of code runs sytax analysis and tells the results
  bool debug = 0;            // if debug == 0 all exceptions are caught
                             //  else they fly away
  std::ifstream debug_file;  // is used to quickly change the debug variable
                             // in future debug system could be elaborated
                             // and this file may come in handy in the case
  debug_file.open("debug_file.txt");
  debug_file >> debug;
  alltids = new tid(NULL);
  int* ap;
  int a;
  ap = &a;
  *ap = 5;
  if (debug)
    program();
  else {
    error* err_later =
        nullptr;  // is used to cout all the layers and the problematic lexeme;
    try {
      program();
    } catch (unexpected_lexeme_error err) {
      err_later = new unexpected_lexeme_error;
      *err_later = err;
      std::cout << err.addition + " expected instead of " + err.problem.lex;
      // return 0;
    } catch (unexpected_lexeme_type_error err) {
      err_later = new unexpected_lexeme_type_error;
      *err_later = err;
      std::cout << err.addition + " lexeme expected instead of ";
      if (err.problem.type == id)
        std::cout << "id";
      else if (err.problem.type == num)
        std::cout << "number const";
      else if (err.problem.type == string)
        std::cout << "string const";
      else if (err.problem.type == reserved)
        std::cout << "reserved word";
      else if (err.problem.type == operat)
        std::cout << "operator";
      else if (err.problem.type == punct)
        std::cout << "punctuaton mark";
      // return 0;
    } catch (out_of_bounds_back_error) {
      std::cout << "that's weird, write telegram @spydragonfork to clarify";
    } catch (out_of_bounds_forth_error) {
      std::cout << "not enough lexemes";
    } catch (type_incompatibility_error err) {
      err_later = new type_incompatibility_error;
      *err_later = err;
      if (err.second_addition == "") {
        std::cout << err.problematic_operator << " incompatible with "
                  << err.addition;
      } else {
        std::cout << err.problematic_operator << " can't be used with "
                  << err.addition << " and " << err.second_addition;
      }
    } catch (prohbited_operation_error err) {
      // nothing here, it is used
    } catch (unknown_id_error err) {
      err_later = new unknown_id_error;
      *err_later = err;
      std::cout << err.addition << " is an unknown id ";
    } catch (not_enough_operands_error err) {
      err_later = new not_enough_operands_error;
      *err_later = err;
      std::cout << "not enough operands to call " << err.addition;
    } catch (wrong_operand_type_error err) {
      std::cout << "wrong operand type, expected " << err.addition;
      err_later = new not_enough_operands_error;
      *err_later = err;
    } catch (wrong_parameters_error err) {
      err_later = new wrong_parameters_error;
      *err_later = err;
      std::cout << "can't call " << err.addition << " with given parameters";
      if (err.second_addition != "") {
        std::cout << "; problem addition: " << err.second_addition;
      }
    } catch (not_allowed_error err) {
      err_later = new not_allowed_error;
      *err_later = err;
      std::cout << err.addition << " is not allowed";
    };
    if (err_later == nullptr) {
      for (int co = 0; co < pol.size(); ++co) {
        if (pol[co].val == " pa") {
          std::cout << co << '(' << *static_cast<int*>(pol[co].const_val)
                    << ") ";
          continue;
        }
        std::cout << co << '(' << pol[co].val << ") ";
      }
      running_program();

      std::cout << "\nresult: ok";
      return 0;
    }
    /*
int main() {
    for (int th = 0; th < 10; ++th) {
        for(int hu = 0; hu < 10; ++hu) {
            for (int dec = 0; dec < 10; ++dec) {
                for (int n = 0; n < 10; ++n) {
                    if ((th + hu + dec + n) % 2) {
                        stout("d%d%d%d%", th, hu, dec, n)
                    }
                }
            }
        }
    }
}
    */
    std::cout << '\n'
              << err_later->layers[0] << " " << err_later->layers[1] << " "
              << err_later->layers[2] << '\n'
              << "problematic lexeme: " << err_later->problem.lex;
    return 0;
  }
  // syntax analysis end
}
