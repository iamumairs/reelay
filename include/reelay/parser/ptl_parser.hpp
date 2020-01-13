/*
 * Copyright (c) 2019 Dogan Ulus
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <iostream>
#include <string>

#include "reelay/third_party/peglib.h"

#include "reelay/networks.hpp"
#include "reelay/settings.hpp"

namespace reelay {

template <class Setting> struct ptl_parser {

  using time_t = typename Setting::time_t;
  using input_t = typename Setting::input_t;
  using output_t = typename Setting::output_t;
  using function_t = typename Setting::function_t;

  using node_t = typename Setting::node_t;
  using state_t = typename Setting::state_t;
  using network_t = typename Setting::network_t;

  using node_ptr_t = typename Setting::node_ptr_t;
  using state_ptr_t = typename Setting::state_ptr_t;

  static constexpr auto grammar = R"(
    Expression  <- ExistsExpr / ForallExpr / Implicative
    ExistsExpr  <- EXISTS '[' NonEmptyVarList ']' '.' Implicative 
    ForallExpr  <- FORALL '[' NonEmptyVarList ']' '.' Implicative
    Implicative <- Disjunctive (LIMPLIES Disjunctive)?
    Disjunctive <- Conjunctive (LOR Conjunctive)*
    Conjunctive <- SinceExpr (LAND SinceExpr)*
    SinceExpr <- Unary (SINCE Bound? Unary)?
    Unary <- NotExpr / PrevExpr / TimedOnceExpr / OnceExpr / TimedHistExpr / HistExpr / Atom / '(' Expression ')'
    NotExpr  <- LNOT Atom / LNOT '(' Expression ')'
    PrevExpr <- PREV Atom / PREV '(' Expression ')'
    OnceExpr <- ONCE Atom / ONCE '(' Expression ')'
    HistExpr <- HIST Atom / HIST '(' Expression ')'
    TimedOnceExpr <- ONCE Bound Atom / ONCE Bound '(' Expression ')'
    TimedHistExpr <- HIST Bound Atom / HIST Bound '(' Expression ')'
    Atom <- CustomPredicate / BasicPredicateLE / BasicPredicateLT / BasicPredicateGE / BasicPredicateGT / ListProposition / RecordProposition / Proposition
            
    Proposition <- Name
    BasicPredicateLE <- Name "<=" Number
    BasicPredicateLT <- Name "<" Number
    BasicPredicateGE <- Name ">=" Number
    BasicPredicateGT <- Name ">" Number
    CustomPredicate <- '$' Name

    ListProposition <- LBRACE NonEmptyValueList RBRACE
    RecordProposition <- LCURLY NonEmptyKeyValuePairs RCURLY

    NonEmptyVarList <- Name (COMMA Name)*
    NonEmptyValueList <- FieldValue (COMMA FieldValue)*
    NonEmptyKeyValuePairs <- KeyValuePair (COMMA KeyValuePair)*

    FieldProp <- Name
    FieldKey <- Name (DOT Name)*
    FieldValue <- VariableRef / UnnamedRef / FieldProp
    KeyValuePair <- FieldKey ':' FieldValue

    UnnamedRef <- STAR
    VariableRef <- STAR Name
    VariableDecl <- AMSAND Name
  
    Bound <- FullBound / LowerBound / UpperBound
    FullBound <-  "[" Number ":" Number "]"
    LowerBound <- "[" Number ":" 'inf'? "]"
    UpperBound <- "["        ":" Number "]"
        
    ~PREV   <- < 'Y' / 'pre' >
    ~HIST  <- < 'H' / 'historically' >
    ~ONCE  <- < 'P' / 'once' >
    ~SINCE <- < 'S' / 'since' >
            
    ~LOR      <- < "||" / 'or' >
    ~LAND     <- < "&&" / 'and' >
    ~LNOT     <- < "!"  / 'not' >
    ~LIMPLIES <- < "->" / 'implies' >
    ~EXISTS <- < 'E' / 'exists' >
    ~FORALL <- < 'A' / 'forall' >
    ~COMMA <- < ',' >
    ~LPARAM <- < '(' >
    ~RPARAM <- < ')' >
    ~LBRACE <- < '[' >
    ~RBRACE <- < ']' >
    ~LCURLY <- < '{' >
    ~RCURLY <- < '}' >
    
    ~DOT <- < '.' >
    ~STAR <- < '*' >
    ~AMSAND <- < '&' >
    ~DOLLAR <- < '$' >
    ~SQUARE <- < '#' >
    
    Name   <- <[_a-zA-Z][_a-zA-Z0-9]*>
    Number <- <'-'? [0-9]+ ('.' [0-9]+)?>
            
    %whitespace <- [  \t\r\n]*
    )";

  peg::parser parser;
  reelay::kwargs meta;

  std::vector<state_ptr_t> states = std::vector<state_ptr_t>();

  explicit ptl_parser(const reelay::kwargs &mm = reelay::kwargs())
      : meta(mm) {

    parser = peg::parser(grammar);
    parser.log = [](size_t line, size_t col, const std::string &msg) {
      std::cerr << line << ":" << col << ": " << msg << std::endl;
    };

    parser["Proposition"] = [&](const peg::SemanticValues &sv) {
      auto name = sv[0].get<std::string>();

      reelay::kwargs kw = {{"name", name}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_state("proposition", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["ListProposition"] = [&](const peg::SemanticValues &sv) {
      auto manager = std::any_cast<data_mgr_t>(meta.at("manager"));
      auto fields =
          sv[0].get<std::vector<std::pair<std::string, std::string>>>();
      reelay::kwargs kw = {{"fields", fields}};
      
      for(const auto& field : fields){
        if(field.first == "variable_ref"){
          manager->add_variable(field.second);
        }
      }

      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_state("listing", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["NonEmptyVarList"] = [&](const peg::SemanticValues &sv) {
      auto vlist = std::vector<std::string>();

      for (std::size_t i = 0; i < sv.size(); i++) {
        auto child = sv[i].get<std::string>();
        vlist.push_back(child);
      }

      return vlist;
    };

    parser["NonEmptyValueList"] = [&](const peg::SemanticValues &sv) {
      auto vlist = std::vector<std::pair<std::string, std::string>>();

      for (std::size_t i = 0; i < sv.size(); i++) {
        auto child = sv[i].get<std::pair<std::string, std::string>>();
        vlist.push_back(child);
      }

      return vlist;
    };

    parser["FieldProp"] = [&](const peg::SemanticValues &sv) {
      return std::pair<std::string, std::string>("proposition",
                                                 sv[0].get<std::string>());
    };

    parser["VariableRef"] = [&](const peg::SemanticValues &sv) {
      return std::pair<std::string, std::string>("variable_ref", sv[0].get<std::string>());
    };

    parser["UnnamedRef"] = [&](const peg::SemanticValues &sv) {
      return std::pair<std::string, std::string>("ignore_field","");
    };

    parser["BasicPredicateLT"] = [&](const peg::SemanticValues &sv) {
      auto name = sv[0].get<std::string>();
      auto constant = std::stof(sv[1].get<std::string>());

      reelay::kwargs kw = {{"name", name}, {"constant", constant}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_state("lt", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["BasicPredicateLE"] = [&](const peg::SemanticValues &sv) {
      auto name = sv[0].get<std::string>();
      auto constant = std::stof(sv[1].get<std::string>());

      reelay::kwargs kw = {{"name", name}, {"constant", constant}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_state("le", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["BasicPredicateGT"] = [&](const peg::SemanticValues &sv) {
      auto name = sv[0].get<std::string>();
      auto constant = std::stof(sv[1].get<std::string>());

      reelay::kwargs kw = {{"name", name}, {"constant", constant}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_state("gt", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["BasicPredicateGE"] = [&](const peg::SemanticValues &sv) {
      auto name = sv[0].get<std::string>();
      auto constant = std::stof(sv[1].get<std::string>());

      reelay::kwargs kw = {{"name", name}, {"constant", constant}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_state("ge", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["CustomPredicate"] = [&](const peg::SemanticValues &sv) {
      auto name = sv[0].get<std::string>();
      auto func = meta[name];

      reelay::kwargs kw = {{"function", func}};
      kw.insert(meta.begin(), meta.end()); 
      auto expr = Setting::make_state("predicate", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["ExistsExpr"] = [&](const peg::SemanticValues &sv) {
      auto vars = sv[0].get<std::vector<std::string>>();
      
      auto child = sv[1].get<node_ptr_t>();
      auto args = std::vector<node_ptr_t>({child});

      reelay::kwargs kw = {{"args", args}, {"vars", vars}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_node("exists", kw);

      return std::static_pointer_cast<node_t>(expr);
    };

    parser["ForallExpr"] = [&](const peg::SemanticValues &sv) {
      auto vars = sv[0].get<std::vector<std::string>>();

      auto child = sv[1].get<node_ptr_t>();
      auto args = std::vector<node_ptr_t>({child});

      reelay::kwargs kw = {{"args", args}, {"vars", vars}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_node("forall", kw);

      return std::static_pointer_cast<node_t>(expr);
    };

    parser["NotExpr"] = [&](const peg::SemanticValues &sv) {
      // Rule: NotExpr  <- LNOT Expression
      node_ptr_t child = sv[0].get<node_ptr_t>();
      auto args = std::vector<node_ptr_t>({child});

      reelay::kwargs kw = {{"args", args}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_node("negation", kw);

      return std::static_pointer_cast<node_t>(expr);
    };

    parser["Implicative"] = [&](const peg::SemanticValues &sv) {
      // Rule:
      if (sv.size() > 1) {
        std::vector<node_ptr_t> args;
        for (size_t i = 0; i < sv.size(); i++) {
          node_ptr_t child = sv[i].get<node_ptr_t>();
          args.push_back(child);
        }

        reelay::kwargs kw = {{"args", args}};
        kw.insert(meta.begin(), meta.end());
        auto expr = Setting::make_node("implication", kw);

        return std::static_pointer_cast<node_t>(expr);
      } else {
        node_ptr_t child = sv[0].get<node_ptr_t>();
        return child;
      }
    };

    parser["Disjunctive"] = [&](const peg::SemanticValues &sv) {
      // Rule:
      if (sv.size() > 1) {
        std::vector<node_ptr_t> args;
        for (size_t i = 0; i < sv.size(); i++) {
          node_ptr_t child = sv[i].get<node_ptr_t>();
          args.push_back(child);
        }

        reelay::kwargs kw = {{"args", args}};
        kw.insert(meta.begin(), meta.end());
        auto expr = Setting::make_node("disjunction", kw);

        return std::static_pointer_cast<node_t>(expr);
      } else {
        node_ptr_t child = sv[0].get<node_ptr_t>();
        return child;
      }
    };

    parser["Conjunctive"] = [&](const peg::SemanticValues &sv) {
      // Rule:
      if (sv.size() > 1) {
        std::vector<node_ptr_t> args;
        for (size_t i = 0; i < sv.size(); i++) {
          node_ptr_t child = sv[i].get<node_ptr_t>();
          args.push_back(child);
        }

        reelay::kwargs kw = {{"args", args}};
        kw.insert(meta.begin(), meta.end());
        auto expr = Setting::make_node("conjunction", kw);

        return std::static_pointer_cast<node_t>(expr);
      } else {
        node_ptr_t child = sv[0].get<node_ptr_t>();
        return child;
      }
    };

    parser["PrevExpr"] = [&](const peg::SemanticValues &sv) {
      // Rule:
      node_ptr_t child = sv[0].get<node_ptr_t>();
      auto args = std::vector<node_ptr_t>({child});

      reelay::kwargs kw = {{"args", args}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_state("previous", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["OnceExpr"] = [&](const peg::SemanticValues &sv) {
      // Rule:
      node_ptr_t child = sv[0].get<node_ptr_t>();
      auto args = std::vector<node_ptr_t>({child});

      reelay::kwargs kw = {{"args", args}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_state("past_sometime", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["TimedOnceExpr"] = [&](const peg::SemanticValues &sv) {
      // Rule:
      std::pair<float, float> bound = sv[0].get<std::pair<float, float>>();
      node_ptr_t child = sv[1].get<node_ptr_t>();

      time_t lbound = std::get<0>(bound);
      time_t ubound = std::get<1>(bound);
      auto args = std::vector<node_ptr_t>({child});

      reelay::kwargs kw = {{"args", args},
                           {"lbound", lbound},
                           {"ubound", ubound}};
      kw.insert(meta.begin(), meta.end());
      state_ptr_t expr;
      if (ubound > 0) {
        expr = Setting::make_state("past_sometime_bounded", kw);
      } else {
        expr = Setting::make_state("past_sometime_bounded_half", kw);
      }

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["HistExpr"] = [&](const peg::SemanticValues &sv) {
      // Rule:
      node_ptr_t child = sv[0].get<node_ptr_t>();
      auto args = std::vector<node_ptr_t>({child});

      reelay::kwargs kw = {{"args", args}};
      kw.insert(meta.begin(), meta.end());
      auto expr = Setting::make_state("past_always", kw);

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["TimedHistExpr"] = [&](const peg::SemanticValues &sv) {
      // Rule:
      std::pair<float, float> bound = sv[0].get<std::pair<float, float>>();
      node_ptr_t child = sv[1].get<node_ptr_t>();

      time_t lbound = std::get<0>(bound);
      time_t ubound = std::get<1>(bound);
      auto args = std::vector<node_ptr_t>({child});

      reelay::kwargs kw = {{"args", args},
                           {"lbound", lbound},
                           {"ubound", ubound}};
      kw.insert(meta.begin(), meta.end());
      state_ptr_t expr;
      if (ubound > 0) {
        expr = Setting::make_state("past_always_bounded", kw);
      } else {
        expr = Setting::make_state("past_always_bounded_half", kw);
      }

      this->states.push_back(expr);
      return std::static_pointer_cast<node_t>(expr);
    };

    parser["SinceExpr"] = [&](const peg::SemanticValues &sv) {
      
      if (sv.size() == 3) {
        node_ptr_t left = sv[0].get<node_ptr_t>();
        std::pair<float, float> bound = sv[1].get<std::pair<float, float>>();
        node_ptr_t right = sv[2].get<node_ptr_t>();

        time_t lbound = std::get<0>(bound);
        time_t ubound = std::get<1>(bound);
        auto args = std::vector<node_ptr_t>({left, right});

        reelay::kwargs kw = {{"args", args},
                             {"lbound", lbound},
                             {"ubound", ubound}};
        kw.insert(meta.begin(), meta.end());

        state_ptr_t expr;
        if (ubound > 0){
          expr = Setting::make_state("since_bounded", kw);
        } else {
          expr = Setting::make_state("since_bounded_half", kw);
        }

        this->states.push_back(expr);
        return std::static_pointer_cast<node_t>(expr);

      } else if (sv.size() == 2) {
        node_ptr_t left = sv[0].get<node_ptr_t>();
        node_ptr_t right = sv[1].get<node_ptr_t>();
        auto args = std::vector<node_ptr_t>({left, right});

        reelay::kwargs kw = {{"args", args}};
        kw.insert(meta.begin(), meta.end());
        auto expr = Setting::make_state("since", kw);

        this->states.push_back(expr);
        return std::static_pointer_cast<node_t>(expr);

      } else {
        node_ptr_t child = sv[0].get<node_ptr_t>();
        return child;
      }
    };

    parser["FullBound"] = [](const peg::SemanticValues &sv) {
      float l = std::stof(sv[0].get<std::string>());
      float u = std::stof(sv[1].get<std::string>());
      return std::make_pair(l, u);
    };

    parser["LowerBound"] = [](const peg::SemanticValues &sv) {
      float l = std::stof(sv[0].get<std::string>());
      return std::make_pair(l, 0.0f);
    };

    parser["UpperBound"] = [](const peg::SemanticValues &sv) {
      float u = std::stof(sv[0].get<std::string>());
      return std::make_pair(0.0f, u);
    };

    parser["Name"] = [](const peg::SemanticValues &sv) { return sv.token(); };
    parser["Number"] = [](const peg::SemanticValues &sv) { return sv.token(); };

    parser.enable_packrat_parsing(); // Enable packrat parsing.
  }

  std::shared_ptr<network_t> parse(const std::string &pattern) {
    node_ptr_t output_func;
    parser.parse(pattern.c_str(), output_func);

    auto network = std::make_shared<network_t>(output_func, states);
    return network;
  }
};

} // namespace reelay