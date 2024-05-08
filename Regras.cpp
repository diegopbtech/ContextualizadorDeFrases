//BIBLIOTECAS GERAIS PARA USO
#include <iostream>
#include <string>
#include <sstream>
//BIBLIOTECA PARA FAZER FORMATAÇÃO NA TABELA
#include <iomanip>
//BIBLIOTECA PARA SOLICITAR O CONTEÚDO NA WEB
#include <curl/curl.h>
//BIBLIOTECA PARA EXPRESSÕES REGULARES
#include <regex>

using namespace std;

struct Token4 {
  string lex;
  string classificacao;
  string lexSemAcento;
  bool permiteSinonimo = true;
};

struct fraseAlterada {
  string lex;
  string classificacao;
  string lexSemAcento;
  bool permiteSinonimo = true;
};

class Regras {
  public:
    Token4 tokens[100];
    fraseAlterada fraseReorganizada[100];
    int nToken = 0;
    int nFraseAlterada = 0;
    bool virgula = false;
    int posVirgula = nToken;

  void reorganizar(string claimOriginal){
    posVirgula = nToken;
    reorganizarPosVirgula();
    reorganizarAntesDaVirgula();
    substituirSinonimos();
    cout << "\n\n===================== Concluído com Sucesso =====================\n" << endl;

    fraseReorganizada[0].lex[0] = toupper(fraseReorganizada[0].lex[0]);
    
    cout << "Claim Original: " << claimOriginal << endl;
    cout << "\nClaim Modificado: ";
    for(int j=0;j<=nFraseAlterada;j++){
      if(fraseReorganizada[j+1].lex != "," && fraseReorganizada[j+1].classificacao != "Pontuação final"){
        cout <<  fraseReorganizada[j].lex << " ";
      }else{
        cout <<  fraseReorganizada[j].lex;
      }
    }
  }

  void reorganizarPosVirgula(){
    int i=0;
    bool jaEncontrei = false;
    while(i<nToken){
      //LOCALIZANDO VIRGULA E APLICANDO A REGRA INVERTENDO A POSIÇÃO DAS ORAÇÕES
      if(tokens[i].classificacao == "Pontuação intermediária"){
        virgula = true;
        posVirgula = i;
        int j = i + 1;
        while(j<nToken){
          if(tokens[j].classificacao == "substantivo" || tokens[j].classificacao == "artigo" || tokens[j].classificacao == "pronome"){
            jaEncontrei = true;
              j++;
              while(tokens[j].classificacao == "substantivo"){
                j++;
              }
              if(tokens[j].classificacao == "verbo" || tokens[j].classificacao == "adverbio" || tokens[j].classificacao == "preposição" || tokens[j].classificacao == "conjunção"){
                //MOVER PARA O INÍCIO
                fraseReorganizada[nFraseAlterada].lex = tokens[j].lex;
                fraseReorganizada[nFraseAlterada].classificacao = tokens[j].classificacao;
                fraseReorganizada[nFraseAlterada].lexSemAcento = tokens[j].lexSemAcento;
                fraseReorganizada[nFraseAlterada].permiteSinonimo = tokens[j].permiteSinonimo;
                nFraseAlterada++;
                //MOVER OS SUBSTANTIVOS, ARTIGOS OU PRONOME DO INÍCIO PARA APÓS O VERBO, ADVÉRBIO, PREPOSIÇÃO OU CONJUNÇÃO
                for(int l = posVirgula + 1; l < j; l++){
                  fraseReorganizada[nFraseAlterada].lex = tokens[l].lex;
                  fraseReorganizada[nFraseAlterada].classificacao = tokens[l].classificacao;
                  fraseReorganizada[nFraseAlterada].lexSemAcento = tokens[l].lexSemAcento;
                  fraseReorganizada[nFraseAlterada].permiteSinonimo = tokens[l].permiteSinonimo;
                  nFraseAlterada++;
                }
                j++;
                // REORGANIZAR O RESTANTE NORMALMENTE
                for(int restante = j; restante < nToken; restante++){
                  if(tokens[restante].classificacao != "Pontuação final" && tokens[restante].classificacao != "Pontuação intermediária"){
                    fraseReorganizada[nFraseAlterada].lex = tokens[restante].lex;
                    fraseReorganizada[nFraseAlterada].classificacao = tokens[restante].classificacao;
                    fraseReorganizada[nFraseAlterada].lexSemAcento = tokens[restante].lexSemAcento;
                    fraseReorganizada[nFraseAlterada].permiteSinonimo = tokens[restante].permiteSinonimo;
                    nFraseAlterada++;
                  }
                }
              }else{
                // REORGANIZAR SEM A NECESSIDADE DE MOVER AS PALAVRAS
                for(int l = posVirgula + 1; l < nToken; l++){
                  if(tokens[l].classificacao != "Pontuação final" && tokens[l].classificacao != "Pontuação intermediária"){
                    fraseReorganizada[nFraseAlterada].lex = tokens[l].lex;
                    fraseReorganizada[nFraseAlterada].classificacao = tokens[l].classificacao;
                    fraseReorganizada[nFraseAlterada].lexSemAcento = tokens[l].lexSemAcento;
                    fraseReorganizada[nFraseAlterada].permiteSinonimo = tokens[l].permiteSinonimo;
                    nFraseAlterada++;
                  }
                }
              }
          }else{
            // REORGANIZAR SEM A NECESSIDADE DE MOVER AS PALAVRAS
            for(int l = posVirgula; l < nToken; l++){
              if(tokens[l].classificacao != "Pontuação final" && tokens[l].classificacao != "Pontuação intermediária" && !jaEncontrei){
                fraseReorganizada[nFraseAlterada].lex = tokens[l].lex;
                fraseReorganizada[nFraseAlterada].classificacao = tokens[l].classificacao;
                fraseReorganizada[nFraseAlterada].lexSemAcento = tokens[l].lexSemAcento;
                fraseReorganizada[nFraseAlterada].permiteSinonimo = tokens[l].permiteSinonimo;
                nFraseAlterada++;
                break;
              }
            }
            break;
          }
        }
        break;
      }
      i++;
    }
    if(virgula){
      fraseReorganizada[nFraseAlterada].lex = ",";
      fraseReorganizada[nFraseAlterada].lexSemAcento = ",";
      nFraseAlterada++;
    }
  }

  void reorganizarAntesDaVirgula(){
    for(int i=0;i < posVirgula;i++){
      fraseReorganizada[nFraseAlterada].lex = tokens[i].lex;
      fraseReorganizada[nFraseAlterada].classificacao = tokens[i].classificacao;
      fraseReorganizada[nFraseAlterada].lexSemAcento = tokens[i].lexSemAcento;
      fraseReorganizada[nFraseAlterada].permiteSinonimo = tokens[i].permiteSinonimo;
      nFraseAlterada++;
    }
  }

  void substituirSinonimos(){
    for(int i=0;i<nFraseAlterada;i++){
      if(fraseReorganizada[i].lex != ","){
        buscarSinonimos(i);
      }
    }
  }

  // Função de escrita usada por cURL
  static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *stream) {
      size_t total_size = size * nmemb;
      stream->append((char*)contents, total_size);
      return total_size;
  }

  // FUNÇÃO PARA CONSULTA OS SINÔNIMOS DA PALAVRA
  int buscarSinonimos(int pos){
      CURL *curl;
      CURLcode res;
      std::string webpage_content;

      curl = curl_easy_init();
      if (curl) {
          std::string url = "http://www.sinonimos.com.br/";
          url += fraseReorganizada[pos].lexSemAcento;

          curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
          curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
          curl_easy_setopt(curl, CURLOPT_WRITEDATA, &webpage_content);

          // Configurar o cURL para seguir redirecionamentos automaticamente
          curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

          res = curl_easy_perform(curl);
          curl_easy_cleanup(curl);

          if (res != CURLE_OK) {
              std::cerr << "Falha para fazer o download da página HTML: " << curl_easy_strerror(res) << std::endl;
              return 1;
          }

          // Aqui você tem o conteúdo da página da web em webpage_content
          // Vamos usar expressões regulares para encontrar as ocorrências de sinônimos
          regex pattern("<p class=\"syn-list\">(.*?)</p>");
          smatch match;
          string::const_iterator search_start(webpage_content.cbegin());

          while (regex_search(search_start, webpage_content.cend(), match, pattern)) {
              string sinonimoWeb = match[1];

              // Limpando tags HTML restantes dentro da ocorrência e os comandos não necessários
              sinonimoWeb = regex_replace(sinonimoWeb, std::regex("<.*?>|\\d+|\\..*"), "");
              sinonimoWeb = regex_replace(sinonimoWeb, std::regex("(.*?),.*"), "$1");

              if(fraseReorganizada[pos].lex != "de" && fraseReorganizada[pos].lex != "mais" && fraseReorganizada[pos].lex != "com" && fraseReorganizada[pos].permiteSinonimo && fraseReorganizada[pos].classificacao != "artigo"){
                fraseReorganizada[pos].lex = sinonimoWeb.substr(1);
              }else{
                if(fraseReorganizada[pos-1].classificacao == "substantivo" && fraseReorganizada[pos].lex == "com" && fraseReorganizada[pos+1].classificacao == "artigo"){
                  fraseReorganizada[pos].lex = "que possui";
                }
              }
              // Movendo para a próxima ocorrência
              search_start = match.suffix().first;
              break;
          }
      } else {
          std::cerr << "Falha para iniciar o cURL." << std::endl;
          return 1;
      }
      return 0;
  }

};
