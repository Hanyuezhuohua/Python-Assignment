#ifndef PYTHON_INTERPRETER_EVALVISITOR_H
#define PYTHON_INTERPRETER_EVALVISITOR_H


#include "Python3BaseVisitor.h"
#include "bfins.h"
#include "flowcontrolcenter.h"

int depth = 0;
int oop = 0;
int func = 0;
int CHECK[100000];
map<string , BFINS>bfins[2010];
map<string , int> Function;
map<string , BFINS>BFins[100000];
map<int , Python3Parser::SuiteContext*>fun_suite;
vector<string> vec[100000];
vector<int> ve;
map<string , bool>zrh[100000];
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
                zrh[oop][s] = 0;
                bfins[0][s] = tmp;
                vec[oop].emplace_back(s);
        }
        for(int i = length-Length ; i<length ; i++){
                string s = context->tfpdef()[i]->getText();
                BFINS tmp = visit(context->test()[i-length+Length]).as<BFINS>();
                BFins[oop][s] = tmp;
                zrh[oop][s] = 0;
                bfins[0][s] = tmp;
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
            antlrcpp::Any Ret =visit(context->simple_stmt());
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            else if(Ret.is<vector<BFINS>>()){ 
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
            else cerr << "Wrong type from visitSimple_stmt" << endl;
        }
        else if(context->compound_stmt() != nullptr){
            return visit(context->compound_stmt());
        }
        else cerr << "Wrong type in visitStmt" << endl;
    };

    antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *context) override{
        if(context->small_stmt() != nullptr) return visit(context->small_stmt());
        else cerr << "Wrong ty[e in visitSimple_stmt" << endl;
    };

    antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *context) override{
        if(context->expr_stmt() != nullptr){
            return visit(context->expr_stmt());
        }
        else if(context->flow_stmt() != nullptr){
            return visit(context->flow_stmt());
        }
        else cerr << "Wrong type in visitSmall_stmt" << endl;
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
        else cerr << "Wrong type in visitAugassign" << endl;
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
        else cerr << "Wrong type in visitFlow_stmt" << endl;
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
        if(context->testlist() != nullptr) {
            vector<BFINS>v = visit(context->testlist()).as<vector<BFINS>>();
            return v;
        }
        else{
            vector<BFINS>v;
            return v;
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
        else cerr << "Wrong type in Compound_stmt" << endl;
    };

    antlrcpp::Any visitIf_stmt(Python3Parser::If_stmtContext *context) override{
        int length_test = context->test().size();
        int length_suite = context->suite().size();
        for(int i = 0; i< length_test ; i++){
            antlrcpp::Any Ret = visit(context->test()[i]);
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                if(ret.Bool().b){  
                    return visit(context->suite()[i]);
                }
            }
            else if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                if(ret[0].Bool().b){
                    return visit(context->suite()[i]);
                }
            }
            else cerr << "Wrong type from visitSuite" << endl;
        }
        if(length_suite > length_test) return visit(context->suite()[length_test]);
        return BFINS(0);
    };

    antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *context) override{
        flowcontrol_A.push(Running);
        antlrcpp::Any Ret = visit(context->test());
        if(Ret.is<BFINS>()){
            if(Ret.as<BFINS>().Bool().b){
                auto ret = visit(context->suite());
                if(flowcontrol_B.TOP() == Returned){
                    flowcontrol_A.pop();
                    return ret;
                }
                if(flowcontrol_A.TOP() == Broken){
                    flowcontrol_A.pop();
                    return BFINS(0);
                }
            }
            while (visit(context->test()).as<BFINS>().Bool().b){
                auto ret = visit(context->suite());
                if(flowcontrol_B.TOP() == Returned){
                    flowcontrol_A.pop();
                    return ret;
                }
                if(flowcontrol_A.TOP() == Broken) break;
                flowcontrol_A.turnto(Running);
            }
            flowcontrol_A.pop();
            return BFINS(0);
        }
        else if(Ret.is<vector<BFINS>>()){
            if(Ret.as<vector<BFINS>>()[0].Bool().b){
                auto ret = visit(context->suite());
                if(flowcontrol_B.TOP() == Returned){
                    flowcontrol_A.pop();
                    return ret;
                }
                if(flowcontrol_A.TOP() == Broken){
                    flowcontrol_A.pop();
                    return BFINS(0);
                }
            }
            while (visit(context->test()).as<vector<BFINS>>()[0].Bool().b){
                auto ret = visit(context->suite());
                if(flowcontrol_B.TOP() == Returned){
                    flowcontrol_A.pop();
                    return ret;
                }
                if(flowcontrol_A.TOP() == Broken) break;
                flowcontrol_A.turnto(Running);
            }
            flowcontrol_A.pop();
            return BFINS(0);
        }
        else cerr << "Wrong type in visitWhile_stmt" << endl;
    };

    antlrcpp::Any visitSuite(Python3Parser::SuiteContext *context) override{
        if(context->simple_stmt() != nullptr) return visit(context->simple_stmt());
        else{
            int size = context->stmt().size();
            for(int i = 0; i<size; i++){
                if(flowcontrol_A.TOP() != Running) break;
                antlrcpp::Any Ret = visit(context->stmt()[i]);
                if(Ret.is<BFINS>()){
                    BFINS ret = Ret.as<BFINS>();
                    if(flowcontrol_B.TOP() == Returned) return ret;
                }
                else if(Ret.is<vector<BFINS>>()){
                    vector<BFINS> ret = Ret.as<vector<BFINS>>();
                    if(flowcontrol_B.TOP() == Returned){
                        return ret;
                    }
                }
                else cerr << "Wrong type from visitStmt" << endl;
            }
        }
        return BFINS(0);
    };

    antlrcpp::Any visitTest(Python3Parser::TestContext *context) override{
        if(context->or_test() != nullptr){
            antlrcpp::Any Ret = visit(context->or_test());
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            else if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
            else cerr << "Wrong type from visitOr_test" << endl;
        }
        else cerr << "Wrong type in visitTest" << endl;
        BFINS ret(0);
        return ret;
    };

    antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *context) override{
        if(context->and_test().size() == 1){
            antlrcpp::Any Ret = visit(context->and_test()[0]);
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            else if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
            else cerr << "Wrong type from visitAnd_test" << endl;
        }
        else if(context->and_test().size() > 1){
            bool flag = 0;
            for(int i = 0;i<context->and_test().size();i++){
                if(flag) break;
                antlrcpp::Any Ret = visit(context->and_test()[i]);
                BFINS ret;
                if(Ret.is<BFINS>()){
                    ret = Ret.as<BFINS>();
                }
                else if(Ret.is<vector<BFINS>>()){
                    ret = Ret.as<vector<BFINS>>()[0];
                }
                else cerr << "Wrong type from visitAnd_test" << endl;
                if(ret.b == 1){
                    flag = 1;
                }
            }
            return BFINS(flag);
        }
        else cerr << "Wrong type in visitOr_test" << endl;
    };

    antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *context) override{
        if(context->not_test().size() == 1){
            antlrcpp::Any Ret = visit(context->not_test()[0]);
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            else if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
            else cerr << "Wrong type from visitNot_test" << endl;
        }
        else if(context->not_test().size() > 1){
            bool flag = 1;
            for(int i = 0;i<context->not_test().size();i++){
                if(!flag) break;
                antlrcpp::Any Ret = visit(context->not_test()[i]);
                BFINS ret;
                if(Ret.is<BFINS>()){
                    ret = Ret.as<BFINS>();
                }
                else if(Ret.is<vector<BFINS>>()){
                    ret = Ret.as<vector<BFINS>>()[0];
                }
                else cerr << "Wrong type from visitNot_test" << endl;
                if(ret.b == 0){
                    flag = 0;
                }
            }
            return BFINS(flag);
        }
        else cerr << "Wrong type in visitAnd_test" << endl;
    };

    antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *context) override{
        if(context->comparison() != nullptr){
            antlrcpp::Any Ret = visit(context->comparison());
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            else if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
            else cerr << "Wrong type from visitComparison" << endl;
        }
        else if(context->not_test() != nullptr){
            bool flag;
            antlrcpp::Any Ret = visit(context->not_test());
            BFINS ret;
            if(Ret.is<BFINS>()){
                ret = Ret.as<BFINS>();
            }
            else if(Ret.is<vector<BFINS>>()){
                ret = Ret.as<vector<BFINS>>()[0];
            }
            else cerr << "Wrong type from visitnot_test" << endl;
            if(ret.b) flag = 0;
            else flag=1;
            return BFINS(flag);
        }
        else cerr << "Wrong type in visitNot_test" << endl;
    };

    antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *context) override{
        if(context->arith_expr().size() == 1){
            antlrcpp::Any Ret = visit(context->arith_expr()[0]);
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            else if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
            else cerr << "Wrong type from visitArith_expr" << endl;
        }
        else if(context->arith_expr().size() > 1){
            bool flag = 1;
            antlrcpp::Any RetA = visit(context->arith_expr(0));
            BFINS retA;
            if(RetA.is<BFINS>()){
                retA = RetA.as<BFINS>();
            }
            else if(RetA.is<vector<BFINS>>()){
                retA = RetA.as<vector<BFINS>>()[0];
            }
            else cerr << "Wrong type from visitArith_expr" << endl;
            for(int i = 1;i < context->arith_expr().size();i++){
                if(!flag) break;
                string s = context->comp_op()[i-1]->getText();
                antlrcpp::Any RetB = visit(context->arith_expr(i));
                BFINS retB;
                if(RetB.is<BFINS>()){
                    retB = RetB.as<BFINS>();
                }
                else if(RetB.is<vector<BFINS>>()){
                    retB = RetB.as<vector<BFINS>>()[0];
                }
                else cerr << "Wrong type from visitArith_expr" << endl;
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
                else cerr << "Wrong type from visitcomp_OP" << endl;
                retA = retB;
            }
            return BFINS(flag);
        }
        else cerr << "Wrong type in visitComparison" << endl;
    };

    antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *context) override{
        if(context->LESS_THAN() != nullptr){return context->getText();}
        else if(context->GREATER_THAN() != nullptr){return context->getText();}
        else if(context->EQUALS() != nullptr){return context->getText();}
        else if(context->GT_EQ() != nullptr){return context->getText();}
        else if(context->LT_EQ() != nullptr){return context->getText();}
        else if(context->NOT_EQ_2() != nullptr){return context->getText();}
        else cerr << "Wrong type in viistComp_op" << endl;
    };

    antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *context) override{
        antlrcpp::Any Ret = visit(context->term()[0]);
        if(Ret.is<BFINS>()){
            BFINS ret = Ret.as<BFINS>();
            string s =context->getText();
            s.erase(0,context->term(0)->getText().length());
            for(int i = 1 ; i<context->term().size();i++){
                antlrcpp::Any TMP = visit(context->term()[i]);
                BFINS tmp;
                if(TMP.is<BFINS>()){
                    tmp = TMP.as<BFINS>();
                }
                else if(TMP.is<vector<BFINS>>()){
                    tmp = TMP.as<vector<BFINS>>()[0];
                }
                else cerr << "Wrong type from visitTerm" << endl;
                if(s[0]=='+'){
                    ret = ret + tmp;
                }
                else if(s[0]=='-'){
                    ret = ret - tmp;
                }
                else cerr << "Wrong type from addminus" << endl;
                s.erase(0,1+context->term()[i]->getText().length());
            }
            return ret;
        }
        else if(Ret.is<vector<BFINS>>()){
            vector<BFINS>ret = Ret.as<vector<BFINS>>();
            if(context->term().size() == 1) return ret;
            string s =context->getText();
            s.erase(0,context->term(0)->getText().length());
            for(int i = 1 ; i<context->term().size();i++){
                antlrcpp::Any TMP = visit(context->term()[i]);
                BFINS tmp;
                if(TMP.is<BFINS>()){
                    tmp = TMP.as<BFINS>();
                }
                else if(TMP.is<vector<BFINS>>()){
                    tmp = TMP.as<vector<BFINS>>()[0];
                }
                else cerr << "Wrong type from viistTerm" << endl;
                if(s[0]=='+'){
                    ret[0] = ret[0] + tmp;
                }
                else if(s[0]=='-'){
                    ret[0] = ret[0] - tmp;
                }
                else cerr << "Wrong type from addminus" << endl;
                s.erase(0,1+context->term()[i]->getText().length());
            }
            return ret[0];
        }   
        else cerr << "Wrong type in visitArith_expr" << endl;
    };

    antlrcpp::Any visitTerm(Python3Parser::TermContext *context) override{
        antlrcpp::Any Ret = visit(context->factor()[0]);
        if(Ret.is<BFINS>()){
            BFINS ret = Ret.as<BFINS>();
            if(context->factor().size() == 1) return ret;
            string s=context->getText();
            s.erase(0,context->factor()[0]->getText().length());
            for(int i = 1;i < context->factor().size();i++){
                antlrcpp::Any RetA = visit(context->factor()[i]);
                BFINS retA;
                if(RetA.is<BFINS>()){
                    retA = RetA.as<BFINS>();
                }
                else if(RetA.is<vector<BFINS>>()){
                    retA = RetA.as<vector<BFINS>>()[0];
                }
                else cerr << "Wrong type from visitFactor" << endl;
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
                else if(s[0]=='%'){
                    ret = ret%retA;
                    s.erase(0,1+context->factor()[i]->getText().length());
                }
                else cerr << "Wrong type in multidivide" << endl;
            }
            return ret;
        }
        else if(Ret.is<vector<BFINS>>()){
            vector<BFINS>ret = Ret.as<vector<BFINS>>();
            if(context->factor().size() == 1) return ret;
            string s=context->getText();
            s.erase(0,context->factor()[0]->getText().length());
            for(int i = 1;i < context->factor().size();i++){
                antlrcpp::Any RetA = visit(context->factor()[i]);
                BFINS retA;
                if(RetA.is<BFINS>()){
                    retA = RetA.as<BFINS>();
                }
                else if(RetA.is<vector<BFINS>>()){
                    retA = RetA.as<vector<BFINS>>()[0];
                }
                if(s[0]=='*'){
                    ret[0] = ret[0]*retA;
                    s.erase(0,1+context->factor()[i]->getText().length());
                }
                else if(s[0]=='/' && s[1]!='/'){
                    ret[0] = ret[0]/retA;
                    s.erase(0,1+context->factor()[i]->getText().length());
                }
                else if(s[0]=='/'){
                    ret[0] = ret[0]^retA;
                    s.erase(0,2+context->factor()[i]->getText().length());
                }
                else if(s[0]=='%'){
                    ret[0] = ret[0]%retA;
                    s.erase(0,1+context->factor()[i]->getText().length());
                }
                else cerr << "Wrong type in multidivide" << endl;
            }
            return ret[0];
        }
        else cerr << "Wrong type in visitTerm" << endl;
    };

    antlrcpp::Any visitFactor(Python3Parser::FactorContext *context) override{
        if(context->atom_expr() != nullptr){
            antlrcpp::Any qwq = visit(context->atom_expr());
            if(qwq.is<vector<BFINS>>()){
                vector<BFINS>tmp = qwq.as<vector<BFINS>>();
                return tmp;
            }
            else if(qwq.is<BFINS>()){
                BFINS ret = qwq.as<BFINS>();
                return ret;
            }
            else cerr << "Wrong type from visitAtom_expr" << endl;
        }
        else if(context->factor() != nullptr){
            antlrcpp::Any Ret = visit(context->factor());
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                int flag = context->ADD() != nullptr ? 1 : -1;
                    return ret * BFINS(Bigfloat(flag));
            }
            else if(Ret.is<vector<BFINS>>()){
                BFINS ret = Ret.as<vector<BFINS>>()[0];
                int flag = context->ADD() != nullptr ? 1 : -1;
                    return ret * BFINS(Bigfloat(flag));
            }
            else cerr << "Wrong typr from visitAtom_expr" << endl;
        }
        else cerr << "Wrong type in VistFactor" << endl;
    };

    antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *context) override{
        if(context->trailer() == nullptr) {
            BFINS ret  = visit(context->atom()).as<BFINS>();
            return ret;
        }
        else{
            string s = context->atom()->getText();
            if(s == "print"){
                int TMP = 0;
                if(func != 0){
                    TMP = func;
                    func = 0;
                }
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                func = TMP;
                if(tmp.size()>=1){
                    for(int i = 0 ; i < tmp.size()-1 ;i++){
                        tmp[i].print();
                        cout << " ";
                    }
                    tmp[tmp.size()-1].print();
                }
                cout << endl;
                vector<BFINS>v;
                return v;
            }
            else if(s == "int"){
                int TMP = 0;
                if(func != 0){
                    TMP = func;
                    func = 0;
                }
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                func = TMP;
                return tmp[0].Int();
            }
            else if(s == "float"){
                int TMP = 0;
                if(func != 0){
                    TMP = func;
                    func = 0;
                }
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                func = TMP;
                return tmp[0].Double();
            }
            else if(s == "bool"){
                int TMP = 0;
                if(func != 0){
                    TMP = func;
                    func = 0;
                }
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                func = TMP;
                return tmp[0].Bool();
            }
            else if(s == "str"){
                int TMP = 0;
                if(func != 0){
                    TMP = func;
                    func = 0;
                }
                vector<BFINS>tmp = visit(context->trailer()).as<vector<BFINS>>();
                func = TMP;
                return tmp[0].String();
            }
            else{
                int tmp = func;
                func = Function[context->atom()->NAME()->getText()];
                depth++;
                flowcontrol_A.push(Running);
                flowcontrol_B.push(Running);
                CHECK[depth] = 1; 
                visit(context->trailer());
                CHECK[depth] = 0;
                antlrcpp::Any QQQ = visit(fun_suite[func]);
                flowcontrol_A.pop();
                flowcontrol_B.pop();
                depth--;
                func = tmp;
                if(QQQ.is<BFINS>()){
                    BFINS qqq = QQQ.as<BFINS>();
                    return qqq;
                }
                if(QQQ.is<vector<BFINS>>()){
                    vector<BFINS>qqq = QQQ.as<vector<BFINS>>();
                    return qqq;
                }
            }
        }
    };

    antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *context) override{
        if(context->arglist() != nullptr){
            vector<BFINS>v = visit(context->arglist()).as<vector<BFINS>>();
            return v;
        }
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
            string s = context->NAME()->getText();
            if(CHECK[depth] == 1){
                for(int i = depth-1 ; i>=0 ; i--){
                    if(bfins[i].find(context->NAME()->getText()) != bfins[i].end()) {
                        return bfins[i][context->NAME()->getText()];
                    }
                }
            }
            if(CHECK[depth] == 0){
                for(int i = depth ; i>=0 ; i--){
                    if(bfins[i].find(context->NAME()->getText()) != bfins[i].end()) {
                        return bfins[i][context->NAME()->getText()];
                    }
                }   
            }
            BFINS ret(0);
            bfins[depth][context->NAME()->getText()] = ret; //need to check!!!
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
        else cerr << "Wrong type in visitAtom" << endl;
        return BFINS(0);
    };

    antlrcpp::Any visitTestlist(Python3Parser::TestlistContext *context) override{
        antlrcpp::Any Ret = visit(context->test()[0]);
        if(Ret.is<BFINS>()){
            vector<BFINS>v;
            v.clear();
            v.push_back(Ret.as<BFINS>());
            for(int i = 1 ; i < context->test().size() ; i++){
                v.push_back(visit(context->test()[i]).as<BFINS>());
            }
        return v;
        }
        else if(Ret.is<vector<BFINS>>()){
            vector<BFINS>ret = Ret.as<vector<BFINS>>();
            return ret;
        }
        else cerr << "Wrong type in visitTestlist" << endl;
    };

    antlrcpp::Any visitArglist(Python3Parser::ArglistContext *context) override{
        vector<BFINS>v;
        v.clear();
        for(int i = 0 ; i < context->argument().size() ; i++){
            BFINS ret = visit(context->argument()[i]).as<BFINS>();
            if(context->argument()[i]->NAME() == nullptr && func != 0) {
                string s = vec[func][i];
                bfins[depth][s] = ret;
                zrh[func][s] = 1;
            }
            v.push_back(ret);
        }
        if(func != 0){
            if(context->argument().size() < vec[func].size()){
                for(int i = 0 ; i<vec[func].size() ; i++){
                    if(zrh[func][vec[func][i]] == 0){
                    v.push_back(BFins[func][vec[func][i]]);
                    bfins[depth][vec[func][i]] = BFins[func][vec[func][i]];
                    }
                }
            } 
            for(int i = 0 ; i <vec[func].size() ; i++){
                zrh[func][vec[func][i]] = 0;
            }
        }
        return v;
    }; 

    antlrcpp::Any visitArgument(Python3Parser::ArgumentContext *context) override{
        antlrcpp::Any Ret = visit(context->test());
        if(Ret.is<BFINS>()){
            BFINS ret;
            if(context->NAME() != nullptr){
                bfins[depth][context->NAME()->getText()] = Ret.as<BFINS>();
                ret = bfins[depth][context->NAME()->getText()];
                zrh[func][context->NAME()->getText()] = 1;
            }
            else {
                ret = Ret.as<BFINS>();     
            }
            return ret;
        }
        else if(Ret.is<vector<BFINS>>()){
            BFINS ret;
            if(context->NAME() != nullptr){
                bfins[depth][context->NAME()->getText()] = Ret.as<vector<BFINS>>()[0];
                ret = bfins[depth][context->NAME()->getText()];
                zrh[func][context->NAME()->getText()] = 1;
            }
            else {
                ret = Ret.as<vector<BFINS>>()[0];       
            }
            return ret;
        }
        else cerr << "Wrong type in visitArgument" << endl;
    }; 
};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H

