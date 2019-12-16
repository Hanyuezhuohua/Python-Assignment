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
        //cout << "File_input" << endl;
        oop = 0;
        int t = context->stmt().size();
        for(int i = 0 ;i <t ;i++){
            visit(context->stmt()[i]);
        }
        return nullptr;
    };

    antlrcpp::Any visitFuncdef(Python3Parser::FuncdefContext *context) override{
        //cout << "Funcdef" << endl;
        ++oop;
        Function[context->NAME()->getText()] = oop;
        visit(context->parameters());
        fun_suite[oop]=context->suite();
        return nullptr;
    };

    antlrcpp::Any visitParameters(Python3Parser::ParametersContext *context) override{
        //cout << "Parameters" << endl;
        if(context->typedargslist() != nullptr) visit(context->typedargslist());
        return nullptr;
    };

    antlrcpp::Any visitTypedargslist(Python3Parser::TypedargslistContext *context) override{
        //cout << "Typedargslist" << endl;
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
        //cout << "Tfpdef" << endl;
        return context->NAME()->getText();
    };

    antlrcpp::Any visitStmt(Python3Parser::StmtContext *context) override{
        //cout << "Stmt" << endl;
        if(context->simple_stmt() != nullptr){
            antlrcpp::Any Ret =visit(context->simple_stmt());
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            else if(Ret.is<vector<BFINS>>()){ 
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                //ret[0].print();
                //cout << endl;
                return ret;
            }
        }
        else if(context->compound_stmt() != nullptr){
            return visit(context->compound_stmt());
        }
    };

    antlrcpp::Any visitSimple_stmt(Python3Parser::Simple_stmtContext *context) override{
        //cout << "Simple_stmt" << endl;
        return visit(context->small_stmt());
    };

    antlrcpp::Any visitSmall_stmt(Python3Parser::Small_stmtContext *context) override{
        //cout << "Small_stmt" << endl;
        if(context->expr_stmt() != nullptr){
            return visit(context->expr_stmt());
        }
        else if(context->flow_stmt() != nullptr){
            return visit(context->flow_stmt());
        }
    };

    antlrcpp::Any visitExpr_stmt(Python3Parser::Expr_stmtContext *context) override{
        //cout << "Expr_stmt" << endl;
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
//            string s1 =context->testlist(context->testlist().size()-1)->getText();
//            cout << "s1:" << s1 << endl;
            vector<BFINS>length = visit(context->testlist(context->testlist().size()-1));
//            cout << length.size() << "length" << endl;
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
        //cout << "Augassign" << endl;
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
        //cout << "Flow_stmt" << endl;
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
        //cout << "Break_stmt" << endl;
        flowcontrol_A.turnto(Broken);
        BFINS ret(0);
        return ret; 
    };

    antlrcpp::Any visitContinue_stmt(Python3Parser::Continue_stmtContext *context) override{
        //cout << "Continue_stmt" << endl;
        flowcontrol_A.turnto(Continued);
        BFINS ret(0);
        return ret;
    };

    antlrcpp::Any visitReturn_stmt(Python3Parser::Return_stmtContext *context) override{
        //cout << "Return_stmt" << endl;
        flowcontrol_B.turnto(Returned);
        if(context->testlist() != nullptr) {
            vector<BFINS>v = visit(context->testlist()).as<vector<BFINS>>();
//            for(int i = 0 ; i < v.size() ; i++){
//                v[i].print();
//                cout << " " << i << "i" << endl;
//            }
            return v;
        }
        else{
            vector<BFINS>v;
            return v;
        }
    };

    antlrcpp::Any visitCompound_stmt(Python3Parser::Compound_stmtContext *context) override{
        //cout << "Compound_stmt" << endl;
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
        //cout << "If_stmt" << endl;
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
            if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                if(ret[0].Bool().b){
                    return visit(context->suite()[i]);
                }
            }
        }
        if(length_suite > length_test) return visit(context->suite()[length_test]);
        return BFINS(0);
    };

    antlrcpp::Any visitWhile_stmt(Python3Parser::While_stmtContext *context) override{
        //cout << "While_stmt" << endl;
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
        if(Ret.is<vector<BFINS>>()){
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
    };

    antlrcpp::Any visitSuite(Python3Parser::SuiteContext *context) override{
        //cout << "Suite" << endl;
        if(context->simple_stmt() != nullptr) return visit(context->simple_stmt());
        else{
            int size = context->stmt().size();
            //cout << size <<"SIZE" << endl;
            for(int i = 0; i<size; i++){
                if(flowcontrol_A.TOP() != Running) break;
                antlrcpp::Any Ret = visit(context->stmt()[i]);
                //cout << "i:" << i << endl;
                if(Ret.is<BFINS>()){
                    BFINS ret = Ret.as<BFINS>();
                    if(flowcontrol_B.TOP() == Returned) return ret;
                }
                if(Ret.is<vector<BFINS>>()){
                    vector<BFINS> ret = Ret.as<vector<BFINS>>();
                    //cout << "WSL" << endl;
                    if(flowcontrol_B.TOP() == Returned){
                        //ret[0].print();
                        //cout << "WYSL" << endl;
                        return ret;
                    }
                }
            }
        }
        return BFINS(0);
    };

    antlrcpp::Any visitTest(Python3Parser::TestContext *context) override{
        //cout << "Test" << endl;
        if(context->or_test() != nullptr){
            antlrcpp::Any Ret = visit(context->or_test());
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
            //    ret.print();
            //    cout << "test" << endl;
                return ret;
            }
            if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
        }
        BFINS ret(0);
        return ret;
    };

    antlrcpp::Any visitOr_test(Python3Parser::Or_testContext *context) override{
        //cout << "Or_test" << endl;
        if(context->and_test().size() == 1){
            antlrcpp::Any Ret = visit(context->and_test()[0]);
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
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
                if(Ret.is<vector<BFINS>>()){
                    ret = Ret.as<vector<BFINS>>()[0];
                }
                if(ret.b == 1){
                    flag = 1;
                }
            }
            return BFINS(flag);
        }
    };

    antlrcpp::Any visitAnd_test(Python3Parser::And_testContext *context) override{
        //cout << "And_test" << endl;
        if(context->not_test().size() == 1){
            antlrcpp::Any Ret = visit(context->not_test()[0]);
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
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
                if(Ret.is<vector<BFINS>>()){
                    //cout << "PLPLP" << i <<endl;
                    ret = Ret.as<vector<BFINS>>()[0];
                    //ret.print();
                    //cout << "QWQQ" << endl;
                }
                if(ret.b == 0){
                    flag = 0;
                }
            }
            return BFINS(flag);
        }
    };

    antlrcpp::Any visitNot_test(Python3Parser::Not_testContext *context) override{
        //cout << "Not_test" << endl;
        if(context->comparison() != nullptr){
            antlrcpp::Any Ret = visit(context->comparison());
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                //if(ret.size() >=1) ret[0].print();
                //cout << "CHECK" << endl;
                return ret;
            }
        }
        else if(context->not_test() != nullptr){
            bool flag;
            antlrcpp::Any Ret = visit(context->not_test());
            BFINS ret;
            if(Ret.is<BFINS>()){
                ret = Ret.as<BFINS>();
            }
            if(Ret.is<vector<BFINS>>()){
                ret = Ret.as<vector<BFINS>>()[0];
            }
            if(ret.b) flag = 0;
            else flag=1;
            return BFINS(flag);
        }
    };
    antlrcpp::Any visitComparison(Python3Parser::ComparisonContext *context) override{
        //cout << "Comparison" << endl;
        if(context->arith_expr().size() == 1){
            antlrcpp::Any Ret = visit(context->arith_expr()[0]);
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                return ret;
            }
            if(Ret.is<vector<BFINS>>()){
                vector<BFINS>ret = Ret.as<vector<BFINS>>();
                return ret;
            }
        }
        else if(context->arith_expr().size() > 1){
            bool flag = 1;
            antlrcpp::Any RetA = visit(context->arith_expr(0));
            BFINS retA;
            if(RetA.is<BFINS>()){
                retA = RetA.as<BFINS>();
            }
            if(RetA.is<vector<BFINS>>()){
                retA = RetA.as<vector<BFINS>>()[0];
            }
            for(int i = 1;i < context->arith_expr().size();i++){
                if(!flag) break;
                string s = context->comp_op()[i-1]->getText();
                antlrcpp::Any RetB = visit(context->arith_expr(i));
                BFINS retB;
                if(RetB.is<BFINS>()){
                    retB = RetB.as<BFINS>();
                }
                if(RetB.is<vector<BFINS>>()){
                    retB = RetB.as<vector<BFINS>>()[0];
                }
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
                retA = retB;
            }
            return BFINS(flag);
        }
    };

    antlrcpp::Any visitComp_op(Python3Parser::Comp_opContext *context) override{
        //cout << "Comp_op" << endl;
        if(context->LESS_THAN() != nullptr){return context->getText();}
        else if(context->GREATER_THAN() != nullptr){return context->getText();}
        else if(context->EQUALS() != nullptr){return context->getText();}
        else if(context->GT_EQ() != nullptr){return context->getText();}
        else if(context->LT_EQ() != nullptr){return context->getText();}
        else if(context->NOT_EQ_2() != nullptr){return context->getText();}
    };

    antlrcpp::Any visitArith_expr(Python3Parser::Arith_exprContext *context) override{
        //cout << "Arith_expr" << endl;
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
                if(s[0]=='+'){
                    ret = ret + tmp;
                }
                else{
                    ret = ret - tmp;
                }
                s.erase(0,1+context->term()[i]->getText().length());
            }
            return ret;
        }
        if(Ret.is<vector<BFINS>>()){
            vector<BFINS>ret = Ret.as<vector<BFINS>>();
            //if(ret.size() >= 1) ret[0].print();
            //cout << "POUYT" << endl;
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
                if(s[0]=='+'){
                    ret[0] = ret[0] + tmp;
                }
                else{
                    ret[0] = ret[0] - tmp;
                }
                s.erase(0,1+context->term()[i]->getText().length());
            }
            return ret[0];
        }   
    };

    antlrcpp::Any visitTerm(Python3Parser::TermContext *context) override{
        //cout << "Term" << endl;
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
            }
            return ret;
        }
        if(Ret.is<vector<BFINS>>()){
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
            }
            return ret[0];
        }
    };

    antlrcpp::Any visitFactor(Python3Parser::FactorContext *context) override{
        //cout << "Factor" << endl;
        if(context->atom_expr() != nullptr){
            antlrcpp::Any qwq = visit(context->atom_expr());
            if(qwq.is<vector<BFINS>>()){
                vector<BFINS>tmp = qwq.as<vector<BFINS>>();
                //if(tmp.size() >= 1)tmp[0].print();
                //cout <<"WWWW" <<endl;
                return tmp;
            }
            else if(qwq.is<BFINS>()){
                BFINS ret = qwq.as<BFINS>();
                return ret;
            }
        }
        else{
            antlrcpp::Any Ret = visit(context->factor());
            if(Ret.is<BFINS>()){
                BFINS ret = Ret.as<BFINS>();
                int flag = context->ADD() != nullptr ? 1 : -1;
                    return ret * BFINS(Bigfloat(flag));
            }
            if(Ret.is<vector<BFINS>>()){
                BFINS ret = Ret.as<vector<BFINS>>()[0];
                int flag = context->ADD() != nullptr ? 1 : -1;
                    return ret * BFINS(Bigfloat(flag));
            }
        }
    };

    antlrcpp::Any visitAtom_expr(Python3Parser::Atom_exprContext *context) override{
       // cout << "Atom_expr" << endl;
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
                //cout << "WYYYSL" << endl;
                if(QQQ.is<BFINS>()){
                 //   cout << "www" << endl;
                    BFINS qqq = QQQ.as<BFINS>();
                    return qqq;
                }
                if(QQQ.is<vector<BFINS>>()){
                    vector<BFINS>qqq = QQQ.as<vector<BFINS>>();
                    //qqq[0].print();
                    //cout << "WWW" << endl;
                    return qqq;
                }
            }
        }
    };

    antlrcpp::Any visitTrailer(Python3Parser::TrailerContext *context) override{
        //cout << "Trailer" << endl;
        if(context->arglist() != nullptr){
            vector<BFINS>v = visit(context->arglist()).as<vector<BFINS>>();
            //if(v.size() >0) v[0].print();
            //cout <<"WWWWWPP" << endl;
            return v;
        }
        vector<BFINS>v;
        return v;
    };

    antlrcpp::Any visitAtom(Python3Parser::AtomContext *context) override{
        //cout << "Atom" << endl;
        if(context->NONE() != nullptr) return BFINS(4);
        else if(context->TRUE() != nullptr){
            //cout << "TRUE" << endl;
            bool flag = true;
            return BFINS(flag);
        }
        else if(context->FALSE() != nullptr){
            //cout << "FALSE" << endl;
            bool flag = false;
            return BFINS(flag);
        }
        else if(context->NAME() != nullptr){
            //cout << "NAME" << endl;
            string s = context->NAME()->getText();
            //cout << s << endl;
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
            //cout << "NUMBER" << endl;
            bool flag = true;
            string s = context->NUMBER()->getText();
            if(s.find('.') < s.length()) {
                flag = false;
            }
            if(flag){
                //cout << "s:" << s << endl;
                Bigfloat res(s);
                //res.print();
                //cout << "WWWW" << endl;
                BFINS ret(res);
                //ret.print();
                //cout << "QWW" << endl;
                return ret;
            }
            else{
                double tmp = std::stod(s);
                return BFINS(tmp);
            }
        }
        else if(context->STRING().size() != 0){
            //cout << "STRING" << endl;
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
        //cout << "Testlist" << endl;
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
        if(Ret.is<vector<BFINS>>()){
            vector<BFINS>ret = Ret.as<vector<BFINS>>();
            return ret;
        }
    };

    antlrcpp::Any visitArglist(Python3Parser::ArglistContext *context) override{
        //cout << "Arglist" << endl;
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
        //cout << "Argument" << endl;
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
            //ret.print();
            //cout << func << endl;            
            }
            return ret;
        }
        if(Ret.is<vector<BFINS>>()){
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
    }; 
};


#endif //PYTHON_INTERPRETER_EVALVISITOR_H

