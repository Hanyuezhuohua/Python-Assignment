#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3BaseVisitor.h"
#include "bfins.h"
#include "flowcontrolcenter.h"

int depth = 0;
int oop = 0;
map<string , BFINS>bfins[2010];
map<string , int> Function;
map<string , BFINS>BFins[100000];
map<int , Python3Parser::SuiteContext*>fun_suite;
vector<string> vec[100000];
vector<int> ve;
int CHECK;
flowcontrolcenter flowcontrol_A,flowcontrol_B;

class EvalVisitor: public Python3BaseVisitor {

//todo:override all methods of Python3BaseVisitor

    antlrcpp::Any visitFile_input(Python3Parser::File_inputContext *context) override{
        oop = 0;
        int t = context->stmt().size();
        for(int i = 0 ;i <t ;i++){
            visit(context->stmt()[i]);
        }
        return nullptr;
    };

    antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *context) override{
        ++oop;
        Function[context->NAME()->getText()] = oop;
        visit(context->parameters());
        fun_suite[oop]=context->suite();
        return nullptr;
    };

    antlrcpp::Any visitParameters(Python3Parser::ParametersContext *context) override{
        if(context->typedargslist() != nullptr) visit(context->typedargslist());
        return nullptr;
    };

    antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *context) override{
        int length = context->tfpdef().size();
        int Length = context->test().size();
        for(int i = 0 ;  i <length-Length ; i++) {
                string s = context->tfpdef()[i]->getText();
                BFINS tmp(0);
                BFins[oop][s] = tmp;
                vec[oop].emplace_back(s);
        }
        for(int i = length-Length ; i<length ; i++){
                string s = context->tfpdef()[i]->getText();
                BFINS tmp = visit(context->test()[i]).as<BFINS>();
                BFins[oop][s] = tmp;
                vec[oop].emplace_back(s); 
        }
        BFINS ret(0);
        return ret;   
    };

    antlrcpp::Any visitTfpdef(Python3Parser::TfpdefContext *context) override{
        return context->NAME()->getText();
    };

    antlrcpp::Any visitStmt(Python3Parser::StmtContext *context) override{
        if(context->simple_stmt() != nullptr){
            BFINS ret =visit(context->simple_stmt()).as<BFINS>();
            return ret;
        }
        else if(context->compound_stmt() != nullptr){
            return visit(context->compound_stmt());
        }
    };

    antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *context) override{
        return visit(context->small_stmt());
    };

    antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *context) override{
        if(context->expr_stmt() != nullptr){
            return visit(context->expr_stmt());
        }
        else if(context->flow_stmt() != nullptr){
            return visit(context->flow_stmt());
        }
    };

    antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *context) override{
        if(context->testlist().size() == 1){
            visit(context->testlist()[0]);
            BFINS ret(4);
            return ret;
        }
        else if(context->augassign() != nullptr){
            vector<BFINS>retA = visit(context->testlist()[context->testlist().size()-1]).as<vector<BFINS>>();
            string s = visit(context->augassign());
            for(int i = 0 ; i < retA.size() ; i++){
                string ss = context->testlist()[0]->test()[i]->getText();
                if(s == "+="){
                    bfins[depth][ss]=bfins[depth][ss]+retA[i];   
                }
                if(s == "-="){
                    bfins[depth][ss]=bfins[depth][ss]-retA[i];   
                }
                if(s == "*="){
                    bfins[depth][ss]=bfins[depth][ss]*retA[i];   
                }
                if(s == "/="){
                    bfins[depth][ss]=bfins[depth][ss]/retA[i];   
                }
                if(s == "//="){
                    bfins[depth][ss]=bfins[depth][ss]^retA[i];   
                }
                if(s == "%="){
                    bfins[depth][ss]=bfins[depth][ss]%retA[i];   
                }
            }
        }
        else{
            vector<BFINS>length = visit(context->testlist(context->testlist().size()-1));
            for(int i = 0 ; i < context->testlist().size()-1 ; i++){
                for(int j = 0 ;j < length.size() ; j++){
                    string s = context->testlist()[i]->test()[j]->getText();
                    bfins[depth][s] = length[j];
                    visit(context->testlist()[i]->test()[j]);
                }
            }
        }  
        return BFINS(0);  
    };

    antlrcpp::Any visitAugassign(Python3Parser::AugassignContext *context) override{
        if(context->ADD_ASSIGN() != nullptr){
            return context->ADD_ASSIGN()->getText();
        }
        else if(context->SUB_ASSIGN() != nullptr){
            return context->SUB_ASSIGN()->getText();
        }
        else if(context->MULT_ASSIGN() != nullptr){
            return context->MULT_ASSIGN()->getText();
        }
        else if(context->DIV_ASSIGN() != nullptr){
            return context->DIV_ASSIGN()->getText();
        }
        else if(context->IDIV_ASSIGN() != nullptr){
            return context->IDIV_ASSIGN()->getText();
        }
        else if(context->MOD_ASSIGN() != nullptr){
            return context->MOD_ASSIGN()->getText();
        }
    };

    antlrcpp::Any visitFlow_stmt(Python3Parser::Flow_stmtContext *context) override{
        if(context->break_stmt() != nullptr){
            return visit(context->break_stmt());
        }
        else if(context->continue_stmt() != nullptr){
            return visit(context->continue_stmt());
        }
        else if(context->return_stmt() != nullptr){
            return visit(context->return_stmt());
        }
    };

    antlrcpp::Any visitBreak_stmt(Python3Parser::Break_stmtContext *context) override{
        flowcontrol_A.turnto(Broken);
        BFINS ret(0);
        return ret; 
    };

    antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *context) override{
        flowcontrol_A.turnto(Continued);
        BFINS ret(0);
        return ret;
    };

    antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *context) override{
        flowcontrol_B.turnto(Returned);
        if(context->testlist() != nullptr) return visit(context->testlist());
        else{
            BFINS ret(0);
            return ret;
        }
    };

    antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *context) override{
        if(context->if_stmt() != nullptr){
            return visit(context->if_stmt());
        }
        else if(context->while_stmt() != nullptr){
            return visit(context->while_stmt());
        }
        else if(context->funcdef() != nullptr){
            return visit(context->funcdef());
        }
    };

    antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *context) override{
        int length_test = context->test().size();
        int length_suite = context->suite().size();
        for(int i = 0; i< length_test ; i++){
            BFINS ret = visit(context->test()[i]).as<BFINS>();
            if(ret.Bool().b){  
                return visit(context->suite()[i]);
            }
        }
        if(length_suite > length_test) return visit(context->suite()[length_test]);
    };

    antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *context) override{
        flowcontrol_A.push(Running);
        while (visit(context->test()).as<BFINS>().Bool().b) 
        {
            BFINS ret = visit(context->suite()).as<BFINS>();
            if(flowcontrol_B.TOP() == Returned){
                flowcontrol_A.pop();
                return ret;
            }
            if(flowcontrol_A.TOP() == Broken) break;
            flowcontrol_A.turnto(Running);
        }
        flowcontrol_A.pop();
        return BFINS(0);
    };

    antlrcpp::Any visitSuite(Python3Parser::SuiteContext *context) override{
        if(context->simple_stmt() != nullptr) return visit(context->simple_stmt());
        else{
            int size = context->stmt().size();
            for(int i = 0; i<size; i++){
                if(flowcontrol_A.TOP() != Running) break;
                auto ret = visit(context->stmt()[i]);
                if(flowcontrol_B.TOP() == Returned) return ret;
            }
        }
        return BFINS(0);
    };

    antlrcpp::Any visitTest(Python3Parser::TestContext *context) override{
        if(context->or_test() != nullptr){
            BFINS ret = visit(context->or_test()).as<BFINS>();
            return ret;
        }
        BFINS ret(0);
        return ret;
    };

    antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *context) override{
        if(context->and_test().size() == 1){
            BFINS ret = visit(context->and_test()[0]).as<BFINS>();
            return ret;
        }
        else if(context->and_test().size() > 1){
            bool flag = 0;
            for(int i = 0;i<context->and_test().size();i++){
                if(flag) break;
                BFINS ret = visit(context->and_test()[i]).as<BFINS>();
                if(ret.b == 1){
                    flag = 1;
                }
            }
            return BFINS(flag);
        }
    };

    antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *context) override{
        if(context->not_test().size() == 1){
            BFINS ret = visit(context->not_test()[0]).as<BFINS>();
            return ret;
        }
        else if(context->not_test().size() > 1){
            bool flag = 1;
            for(int i = 0;i<context->not_test().size();i++){
                if(!flag) break;
                BFINS ret = visit(context->not_test()[i]).as<BFINS>();
                if(ret.b == 0){
                    flag = 0;
                }
            }
            return BFINS(flag);
        }
    };

    antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *context) override{
        if(context->comparison() != nullptr) {
            BFINS ret = visit(context->comparison()).as<BFINS>();
            return ret;
        }
        else if(context->not_test() != nullptr){
            bool flag;
            BFINS ret = visit(context->not_test()).as<BFINS>();
            if(ret.b) flag = 0;
            else flag=1;
            return BFINS(flag);
        }
    };

    antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *context) override{
        if(context->arith_expr().size() == 1){
            BFINS ret = visit(context->arith_expr()[0]).as<BFINS>();
            return ret;
        }
        else if(context->arith_expr().size() > 1){
            bool flag = 1;
            for(int i = 1;i < context->arith_expr().size();i++){
                if(!flag) break;
                string s = context->comp_op()[i-1]->getText();
                BFINS retA = visit(context->arith_expr(i-1)).as<BFINS>();
                BFINS retB = visit(context->arith_expr(i)).as<BFINS>();
                if(s == "<"){
                    if(retA >= retB)
                    flag = 0;
                }
                else if(s == ">"){
                    if(retA <= retB)
                    flag = 0;
                }
                else if(s == "=="){
                    if(retA != retB)
                    flag = 0;
                }
                else if(s == ">="){
                    if(retA < retB)
                    flag = 0;
                }
                else if(s == "<="){
                    if(retA > retB)
                    flag = 0;
                }
                else if(s == "!="){
                    if(retA == retB)
                    flag = 0;
                }
            }
            return BFINS(flag);
        }
    };

    antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *context) override{
        if(context->LESS_THAN() != nullptr){return context->getText();}
        else if(context->GREATER_THAN() != nullptr){return context->getText();}
        else if(context->EQUALS() != nullptr){return context->getText();}
        else if(context->GT_EQ() != nullptr){return context->getText();}
        else if(context->LT_EQ() != nullptr){return context->getText();}
        else if(context->NOT_EQ_2() != nullptr){return context->getText();}
    };

    antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *context) override{
        BFINS ret = visit(context->term()[0]).as<BFINS>();
        string s =context->getText();
        s.erase(0,context->term(0)->getText().length());
        for(int i = 1 ; i<context->term().size();i++){
            BFINS tmp = visit(context->term()[i]).as<BFINS>();
            if(s[0]=='+'){
                ret = ret + tmp;
            }
            else{
                ret = ret - tmp;
            }
            s.erase(0,1+context->term()[i]->getText().length());
        }
        return ret;   
    };

    antlrcpp::Any visitTerm(Python3Parser::TermContext *context) override{
        BFINS ret = visit(context->factor()[0]).as<BFINS>();
        string s=context->getText();
        s.erase(0,context->factor()[0]->getText().length());
        for(int i = 1;i < context->factor().size();i++){
            BFINS retA = visit(context->factor()[i]).as<BFINS>();
            if(s[0]=='*'){
                ret = ret*retA;
                s.erase(0,1+context->factor()[i]->getText().length());
            }
            else if(s[0]=='/' && s[1]!='/'){
                ret = ret/retA;
                s.erase(0,1+context->factor()[i]->getText().length());
            }
            else if(s[0]=='/'){
                ret = ret^retA;
                s.erase(0,2+context->factor()[i]->getText().length());
            }
            if(s[0]=='%'){
                ret = ret%retA;
                s.erase(0,1+context->factor()[i]->getText().length());
            }
        }
        return ret;
    };

    antlrcpp::Any visitFactor(Python3Parser::FactorContext *context) override{
        if(context->atom_expr() != nullptr){
            antlrcpp::Any qwq = visit(context->atom_expr());
            if(qwq.is<vector<BFINS>>()){
                vector<BFINS>tmp = qwq.as<vector<BFINS>>();
                BFINS ret;
                if(tmp.size() != 0) ret = tmp[0];
                return ret;
            }
            else if(qwq.is<BFINS>()){
                BFINS ret = qwq.as<BFINS>();
                return ret;
            }
        }
        else{
            BFINS ret = visit(context->factor()).as<BFINS>();
            int flag = context->ADD() != nullptr ? 1 : -1;
                return ret * BFINS(Bigfloat(flag));
        }
    };

    antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *context) override{
        if(context->trailer() == nullptr) {
            BFINS ret  = visit(context->atom()).as<BFINS>();
            return ret;
        }
        else{
            string s = context->atom()->getText();
            if(s == "print"){
                CHECK = 1;
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                for(int i = 0 ; i < tmp.size()-1 ;i++){
                    tmp[i].print();
                    cout << " ";
                }
                tmp[tmp.size()-1].print();
                cout << endl;
                CHECK = 0;
                vector<BFINS>v;
                return v;
            }
            else if(s == "int"){
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                vector<BFINS>v;
                v.clear();
                for(int i = 0 ; i<tmp.size() ; i++){
                    v.push_back(tmp[i].Int()); 
                }
                return v;
            }
            else if(s == "float"){
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                vector<BFINS>v;
                v.clear();
                for(int i =0 ; i<tmp.size() ; i++){
                    v.push_back(tmp[i].Double()); 
                }
                return v;
            }
            else if(s == "bool"){
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                vector<BFINS>v;
                v.clear();
                for(int i =0 ; i<tmp.size() ; i++){
                    v.push_back(tmp[i].Bool()); 
                }
                return v;
            }
            else if(s == "str"){
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                vector<BFINS>v;
                v.clear();
                for(int i =0 ; i<tmp.size() ; i++){
                    v.push_back(tmp[i].String()); 
                }
                return v;
            }
            else{
                int tmp = Function[context->atom()->NAME()->getText()];
                depth++;
                for(int i = 0; i<vec[tmp].size() ; i++){
                    string s = vec[tmp][i];
                    bfins[depth][s] = BFins[tmp][s];
                }
                visit(context->trailer());
                visit(fun_suite[tmp]);
                depth--;
                vector<BFINS>v;
                return v;
            }
        }
    };

    antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *context) override{
        if(context->arglist() != nullptr) return visit(context->arglist());
        vector<BFINS>v;
        return v;
    };

    antlrcpp::Any visitAtom(Python3Parser::AtomContext *context) override{
        if(context->NONE() != nullptr) return BFINS(4);
        else if(context->TRUE() != nullptr){
            bool flag = true;
            return BFINS(flag);
        }
        else if(context->FALSE() != nullptr){
            bool flag = false;
            return BFINS(flag);
        }
        else if(context->NAME() != nullptr){
            for(int i = depth ; i>=0 ; i--){
                if(bfins[i].count(context->NAME()->getText())) {
                    return bfins[i][context->NAME()->getText()];
                }
            }
            BFINS ret(0);
            bfins[depth][context->NAME()->getText()] = ret; //need to check
            return bfins[depth][context->NAME()->getText()];
        }
        else if(context->NUMBER() != nullptr){
            bool flag = true;
            string s = context->NUMBER()->getText();
            if(s.find('.') < s.length()) {
                flag = false;
            }
            if(flag){
                Bigfloat res(s);
                BFINS ret(res);
                return ret;
            }
            else{
                double tmp = std::stod(s);
                return BFINS(tmp);
            }
        }
        else if(context->STRING().size() != 0){
            string s;
            for(int i = 0 ; i<context->STRING().size() ; i++){
                string Stmp = context->STRING(i)->getText();
                s += Stmp.substr(1,Stmp.length()-2);
            }
            return BFINS(s);
        }
        else if(context->test() != nullptr){
            return visit(context->test());
        } 
        return BFINS(0);
    };

    antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *context) override{
        vector<BFINS>v;
        v.clear();
        for(int i = 0 ; i < context->test().size() ; i++){
            v.push_back(visit(context->test()[i]).as<BFINS>());
        }
        return v;
    };

    antlrcpp::Any visitArglist(Python3Parser::ArglistContext *context) override{
        vector<BFINS>v;
        v.clear();
        for(int i = 0 ; i < context->argument().size() ; i++){
            BFINS ret = visit(context->argument()[i]).as<BFINS>();
            if(context->argument()[i]->NAME() == nullptr && CHECK != 1) {
                string s = vec[depth][i];
                bfins[depth][s] = ret;
            }
            v.push_back(ret);
        } 
        return v;
    }; 

    antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *context) override{
        BFINS ret;
        if(context->NAME() != nullptr){
            bfins[depth][context->NAME()->getText()] = visit(context->test()).as<BFINS>();
            ret = bfins[depth][context->NAME()->getText()];
        }
        else {
            ret = visit(context->test()).as<BFINS>();            
        }
        return ret;
    }; 
};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H
