//BIBLIOTECAS GERAIS PARA USO
#include <iostream>
#include <string>
#include <sstream>
//BIBLIOTECA PARA FAZER FORMATAÇÃO NA TABELA
#include <iomanip>

using namespace std;

struct Token3 {
  string lex;
  string classificacao;
  string lexSemAcento;
};

class Sintatico {
public:
  Token3 tokens[100];
  int nToken = 0;
  int next = 0;
  bool failure = false;

  int AnaliseSintatica(void) {
    cout << "\n\n=====================Analisador Sintático=====================\n" << endl;
      if (sentenca()) {
        if(tokens[next].classificacao == "Pontuação final"){
          next++;
          sentenca();
        }
        if(next != nToken){
          failure = true;
        }
        if(!failure){
          cout << "Análise sintática bem sucedida e aceita pela gramática!" << endl;
        }else{
          cout << "Erro na sintaxe ou sintaxe não aceita" << endl;
          failure = true;
        }
      }else{
        cout << "Erro na sintaxe ou sintaxe não aceita" << endl;
        failure = true;
      }

    return 0;
  }

  bool sentenca(){
    bool eSentenca = false;
    // ANALISANDO FRASE COM NOMINAL E VERBO
    if(Sintagma_nominal()){
      if(Sintagma_verbal() && (tokens[next].classificacao == "Pontuação final" || tokens[next].classificacao == "Pontuação intermediária" || nToken == next)){
        if(tokens[next].classificacao == "Pontuação intermediária"){
          next++;
          sentenca();
        }
        eSentenca = true;
      }else{
        next -= 2;
      }
    }
    // ANALISANDO FRASE COM NOMINAL, VERBO E ADJETIVO
    if(Sintagma_nominal() && !eSentenca){
      if(Sintagma_verbal()){
        if(Sintagma_adjetival() && (tokens[next].classificacao == "Pontuação final" || tokens[next].classificacao == "Pontuação intermediária" || nToken == next)){
          if(tokens[next].classificacao == "Pontuação intermediária"){
            next++;
            sentenca();
          }
          eSentenca = true;
        }else{
          next -= 2;
        }
      }
    }
    // ANALISANDO FRASE COM NOMINAL, VERBO E ADVÉRBIO
    if(Sintagma_nominal() && !eSentenca){
      if(Sintagma_verbal()){
        if(Sintagma_adverbial()){
          if(tokens[next].classificacao == "Pontuação intermediária"){
            next++;
            sentenca();
          }
          eSentenca = true;
        }else{
          next -= 3;
        }
      }else{
        next -= 2;
      }
    }
    // ANALISANDO FRASE COM PREPOSIÇÃO E SUBSTANTIVO
    if(Sintagma_preposicional() && !eSentenca){
      if(tokens[next].classificacao == "Pontuação intermediária"){
        next++;
        if(sentenca()){
            eSentenca = true;
        }
      }
    }
    return eSentenca;
  }

  //Substantivo | Artigo Substantivo | Pronome | Numeral
  bool Sintagma_nominal(){
    bool eNominal = false;
    if(tokens[next].classificacao == "substantivo" || tokens[next].classificacao == "substantivo próprio"){
      next++;
      Sintagma_preposicional();
      if(tokens[next].classificacao == "substantivo" || tokens[next].classificacao == "substantivo próprio"){
        Sintagma_conjuntivo();
        Sintagma_nominal();
        Sintagma_preposicional();
      }else if (tokens[next].classificacao == "numeral" || tokens[next].classificacao == "pronome" || tokens[next].classificacao == "preposição"){
        Sintagma_conjuntivo();
        Sintagma_nominal();
        Sintagma_preposicional();
      }
      eNominal = true;
    }else if (tokens[next].classificacao == "artigo"){
      next++;
      if(tokens[next].classificacao == "substantivo" || tokens[next].classificacao == "substantivo próprio"){
        next++;
        Sintagma_nominal();
        Sintagma_preposicional();
      }
      eNominal = true;
    }else if (tokens[next].classificacao == "pronome"){
      next++;
      Sintagma_nominal();
      Sintagma_preposicional();
      eNominal = true;
    }else if (tokens[next].classificacao == "numeral"){
      next++;
      Sintagma_nominal();
      Sintagma_preposicional();
      eNominal = true;
    }
    return eNominal;
  }

  // Verbo | Verbo Sintagma_verbal
  bool Sintagma_verbal(){
    bool eVerbo = false;
    if(tokens[next].classificacao == "verbo"){
      next++;
      Sintagma_nominal();
      Sintagma_preposicional();
      Sintagma_verbal();
      eVerbo = true;
    }
    return eVerbo;
  }
  // Adjetivo | Adjetivo Sintagma_adjetival
  bool Sintagma_adjetival(){
    bool eAdjetivo = false;
    if(tokens[next].classificacao == "adjetivo" || tokens[next].classificacao == "adjetivo,"){
      next++;
      Sintagma_adjetival();
      Sintagma_preposicional();
      eAdjetivo = true;
    }
    return eAdjetivo;
  }
  // Adverbio | Adverbio Sintagma_adjetival
  bool Sintagma_adverbial(){
    bool eAdverbio = false;
    if(tokens[next].classificacao == "advérbio"){
      next++;
      Sintagma_adverbial();
      eAdverbio = true;
    }
    return eAdverbio;
  }

  bool Sintagma_preposicional(){
    bool ePreposicao = false;
    if(tokens[next].classificacao == "preposição"){
      next++;
      if(Sintagma_nominal()){
        Sintagma_preposicional();
        ePreposicao = true;
      }
      if(Sintagma_verbal() && !ePreposicao){
        Sintagma_preposicional();
        ePreposicao = true;
      }
    }
    return ePreposicao;
  }

  bool Sintagma_conjuntivo(){
    bool eConjuncao = false;
    if(tokens[next].classificacao == "conjunção"){
      next++;
      eConjuncao = true;
    }
    return eConjuncao;
  }

};