#include <iostream>
#include <locale>
#include "Lexico.cpp"
#include "Sintatico.cpp"
#include "Regras.cpp"

using namespace std;

int main() {
    setlocale(LC_ALL, "");
    Lexico lexema;
    Sintatico sint;
    Regras regras;
    string claim;
    cout << "Insira uma declaração: ";
    getline(cin, claim);
  
    lexema.AnaliseLexica(claim);
  
    sint.nToken = lexema.nToken;
  
    for(int i = 0; i < lexema.nToken; i++){
      sint.tokens[i].lex = lexema.tokens[i].lex;
      sint.tokens[i].classificacao = lexema.tokens[i].classificacao;
      sint.tokens[i].lexSemAcento = lexema.tokens[i].lexSemAcento;
    }

    sint.AnaliseSintatica();

    regras.nToken = sint.nToken;
  
    if(!sint.failure){
      for(int i = 0; i < lexema.nToken; i++){
        regras.tokens[i].lex = lexema.tokens[i].lex;
        regras.tokens[i].classificacao = lexema.tokens[i].classificacao;
        regras.tokens[i].lexSemAcento = lexema.tokens[i].lexSemAcento;
        regras.tokens[i].permiteSinonimo = lexema.tokens[i].permiteSinonimo;
      }
      regras.reorganizar(claim);
    }
  
    return 0;
}