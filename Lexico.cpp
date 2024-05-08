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
//BIBLIOTECA PARA TRABALHAR COM CARACTERES LARGOS (NECESSÁRIO NA REMOÇÃO DOS ACENTOS)
#include <cwchar>

using namespace std;

// TOKENS COM NOME DA PALAVRA | CLASSIFICAÇÃO | PALAVRA SEM ACENTO (PARA CONSULTA WEB) | PERMITE SINÔNIMO
struct Token {
  string lex;
  string classificacao;
  string lexSemAcento;
  bool permiteSinonimo = true;
};

class Lexico {

  public:
    // VARIÁVEIS PARA ARMAZENAR E CLASSIFICAR EM UM TOKEN CADA PALAVRA ENCONTRADA NA FRASE
    Token tokens[100];
    int nToken = 0;
    string palavra = "";
    // VARIÁVEL UTILIZADA QUANDO FOR ENCONTRADO UM NÚMERO NA FRASE E CONCATENAR TODOS OS PRÓXIMOS NÚMEROS
    bool numeral = false;

    // ARRAY DE STRINGS COM AS EXCEÇÕES QUE NÃO SÃO ENCONTRADAS OU NÃO SÃO CLASSIFICADAS CORRETAMENTE NA WEB
    
    // ARRAY COM TODOS OS ARTIGOS, POIS A PÁGINA WEB UTILIZADA NÃO RETORNA A CLASSIFICAÇÃO CORRETA
    string artigos[12]{
      "o","a","os","as","um","algum","uma","alguma","uns","alguns","umas","algumas"
    };
      
    // ARRAY COM ALGUMAS CONJUNÇÕES ESPECÍFICAS QUE A PÁGINA WEB NÃO RETORNA A CLASSIFICAÇÃO CORRETA
    string conjuncao[7]{
      "e","ou","mas","que","como","porque","embora"
    };

    // ARRAY COM ALGUNS VERBOS FLEXIONADOS ESPECÍFICOS QUE A PÁGINA WEB NÃO RETORNA COMO VERBO
    string verbo_flexionado[8]{
      "é","está", "foi", "sera", "será", "tem", "correu", "sugere"
    };
      
    // ARRAY COM ALGUMAS PREPOSIÇÕES ESPECÍFICAS QUE A PÁGINA WEB NÃO RETORNA COMO PREPOSIÇÃO
    string preposicao[15]{
      "ao","à","aos","às","do","da","dos","das","no","na","nos","nas", "para","pela", "pelo"
    };

    // ANALISADOR SINTÁTICO RESPONSÁVEL POR ANALISAR CADA CARACTER ENCONTRADO NA FRASE E SEPARAR EM TOKENS DE ACORDO COM SUA CLASSIFICAÇÃO
    int AnaliseLexica(string claim) {
      string texto;

      // TRANSFORMANDO A STRING EM UM DADO DE ENTRADA PARA FAZER A ANÁLISE CARACTER POR CARACTER
      istringstream frase(claim);

      // LENDO CARACTER POR CARACTER DA STRING DE ENTRADA
      while (getline(frase, texto)) {
        for (int i = 0; i < texto.length(); ++i) {
          char c = texto[i];
          
          // SE O CARACTER FOR UM ESPAÇO EM BRANCO E A VARIÁVEL NUMERAL FOR FALSA, SIGNIFICA QUE ALGUMA PALAVRA FOI ENCONTRADA, NÃO É UM NUMERAL E PRECISA SER CLASSIFICADA
          if (c == ' ' && !numeral) {
            //ADICIONAR A TABELA
            classificar(palavra);
            palavra = "";
          }
          // SE O CARACTER FOR UM '!', '?' OU '.' E A VARIÁVEL NUMERAL FOR FALSA, SIGNIFICA QUE UMA PONTUAÇÃO FINAL FOI ENCONTRADA, NÃO É UM NUMERAL E PRECISA SER CLASSIFICADA
          else if((c == '!'||c == '?'||c == '.') && !numeral){
            //ADICIONAR A TABELA
            classificar(palavra);
            palavra = "";
            tokens[nToken].lex = c;
            tokens[nToken].classificacao = "Pontuação final";
            nToken++;
          }
          // SE O CARACTER FOR UMA ',' E A VARIÁVEL NUMERAL FOR FALSA, SIGNIFICA QUE UMA PONTUAÇÃO INTERMEDIÁRIA FOI ENCONTRADA, NÃO É UM NUMERAL E PRECISA SER CLASSIFICADA
          else if(c == ',' && !numeral){
            //ADICIONAR A TABELA
            classificar(palavra);
            palavra = "";
            tokens[nToken].lex = c;
            tokens[nToken].classificacao = "Pontuação intermediária";
            nToken++;
          }
          // SE O CARACTER FOR UM NÚMERO, CONCATENE A STRING REFERENTE A PALAVRA ENCONTRADA E DECLARE NUMERAL COMO VERDADEIRA, POIS UM NÚMERO FOI ENCONTRADO
          else if((c >= '0' && c <= '9') || c == ','){
            if(c == ',' && (texto[i+1] >= '0' && texto[i+1] <= '9')){
              palavra += c;
              numeral = true;
            }else if(c >= '0' && c <= '9'){
              palavra += c;
              numeral = true;
              if(texto.length()==(i+1)){
                tokens[nToken].lex = palavra;
                tokens[nToken].classificacao = "numeral";
                nToken++;
                numeral = false;
                palavra = "";
              }
            }else{
              i--;
              numeral = false;
            }
          }
          // SE NUMERAL É VERDADEIRO, SIGNIFICA QUE O CARACTER ANTERIOR FOI UM NÚMERO E O CARACTER ATUAL É ALGO QUE FINALIZA UMA SEQUÊNCIA NUMÉRICA PERMITINDO A SUA CLASSIICAÇÃO
          else if(numeral){
            tokens[nToken].lex = palavra;
            tokens[nToken].classificacao = "numeral";
            nToken++;
            numeral = false;
            if(c!=' '){
              i--;
            }
            palavra = "";
          }
          // SE NENHUMA DAS CONDIÇÕES FOR SATISFEITA, SIGNIFICA QUE SE TRATA DE UMA LETRA, LOGO BASTA CONCATENAR NA STRING REFERENTE A PALAVRA ENCONTRADA
          else{
            palavra += c;
          }
          // SE O CARACTER ATUAL FOR O ÚLTIMO DA STRING, SIGNIFICA QUE A PALAVRA FOI CONCLUÍDA, LOGO PRECISA SER CLASSIFICADA.
          if(texto.length()==i+1){
            //ADICIONAR A TABELA
            classificar(palavra);
          }
        }
      }
      
        cout << "\n\n===================== Analisador Lexico ====================="
             << endl;
        cout << setw(20) << "     Token       |" << setw(20)
             << "   Classe Gramatical  " << endl;
        cout << "-----------------------------------------------------------"
             << endl;
        for (int i = 0; i < nToken; i++) {
          cout << setw(17) << tokens[i].lex << "  |" << setw(16);
          cout << tokens[i].classificacao << endl;
          cout << "-----------------------------------------------------------"
               << endl;
        }
  
      return 0;
    }

    // FUNÇÃO PARA REMOVER OS CARACTERES ACENTUADAS DENTRO DA STRING PARA FAZER A PESQUISA NO SITE WEB
    wstring RemoverAcento(wchar_t string[]){
        wchar_t comAcento[] = L"áéèíóúâêôãõàç";
        wchar_t semAcento[] = L"aeeiouaeoaoac";
        int tamanho = wcslen(string), tam = wcslen(comAcento);
  
        for (int i=0; i < tamanho; i++){
            for (int j=0; j < tam; j++){
                if (string[i] == comAcento[j]){
                    string[i] = semAcento[j];
                }
            }
        }
  
      return string;
    }

    // FUNÇÃO PARA CLASSIFICAR AS PALAVRAS DE ACORDO COM SUA CLASSE GRAMATICAL
    void classificar(string palavra){
      bool encontrei = false;
      string palavraComAcentoMinuscula = palavra;

      // TRANSFORMAR TODOS OS CARACTERES EM MINÚSCULO PARA FAZER A COMPARAÇÃO E A PESQUISA NA WEB
      for (char& c : palavraComAcentoMinuscula) {
          c = tolower(c);
      }
      
      // ANTES DE FAZER A PESQUISA NA WEB, VERIFICAR ANTES SE A PALAVRA ESTÁ EM UM DOS CASOS ESPECÍFICOSS MENCIONADOS ANTERIORMENTE
      if(palavra!=""){
        // SE TÁ NOS ARTIGOS ESPECÍFICOS
        for(int i=0;i<12;i++){
          if(palavraComAcentoMinuscula==artigos[i]){
            tokens[nToken].lex = palavra;
            tokens[nToken].classificacao = "artigo";
            encontrei = true;
            nToken++;
            break;
          }
        }
        // SE TÁ NAS CONJUNÇÕES ESPECÍFICAS
        for(int i=0;i<6 && !encontrei;i++){
          if(palavraComAcentoMinuscula==conjuncao[i]){
            tokens[nToken].lex = palavra;
            tokens[nToken].classificacao = "conjunção";
            encontrei = true;
            nToken++;
            break;
          }
        }
        // SE TÁ NAS PREPOSIÇÕES ESPECÍFICAS
        for(int i=0;i<15;i++){
          if(palavraComAcentoMinuscula==preposicao[i]){
            tokens[nToken].lex = palavra;
            tokens[nToken].classificacao = "preposição";
            encontrei = true;
            nToken++;
            break;
          }
        }
        // SE TÁ NOS VERBOS ESPECÍFICOS
        for(int i=0;i<8 && !encontrei;i++){
          if(palavraComAcentoMinuscula==verbo_flexionado[i]){
            tokens[nToken].lex = palavra;
            tokens[nToken].classificacao = "verbo";
            encontrei = true;
            nToken++;
            break;
          }
        }
        // CASO NÃO ESTEJA EM NENHUMA DAS REGRAS ESPECÍFICAS, SERÁ NECESSÁRIO FAZER A SOLICITAÇÃO DESSA CLASSIFICAÇÃO NA WEB
        if(!encontrei){
          // FOI NECESSÁRIO CONVERTER A PALAVRA EM UM ARRAY DO TIPO WCHAR, UM TIPO ESPECÍFICO DO CHAR QUE PERMITE LER CARACTER ACENTUADO
          size_t tamanho = mbstowcs(nullptr, palavra.c_str(), 0) + 1;
          wchar_t charAcentos[tamanho];
          mbstowcs(charAcentos, palavraComAcentoMinuscula.c_str(), tamanho);

          // APÓS A CONVERSÃO, ENVIA ESSA VARIÁVEL CONVERTIDA PARA A FUNÇÃO REMOVER ACENTO E A RETORNA EM OUTRO TIPO ESPECÍFICO DO STRING, O WSTRING
          wstring claimSemAcento = RemoverAcento(charAcentos);
          // APÓS REMOVER OS ACENTOS, CONVERTE NOVAMENTE PARA O TIPO STRING
          string palavraSemAcento(claimSemAcento.begin(), claimSemAcento.end());
          // POR FIM ENVIA PARA A FUNÇÃO QUE FARÁ A BUSCA NA WEB
          classeGramaticalGeral(palavraSemAcento);

          // APÓS CONSULTAR, PREENCHER NA TABELA E CLASSIFICAR DE ACORDO COM SUA CLASSE GRAMATICAL, LIMPAR A VARIÁVEL DA PALAVRA PARA NÃO CONFLICTAR COM A PRÓXIMA PALAVRA DA FRASE
          palavra = "";
          
        }
      }
    }
  
    // FUNÇÃO DE ESCRITA USADA POR CURL
    static size_t WriteCallback_1(void *contents, size_t size, size_t nmemb, std::string *stream) {
        size_t total_size = size * nmemb;
        stream->append((char*)contents, total_size);
        return total_size;
    }
  
    // FUNÇÃO PARA CLASSIFICAR O TIPO DA PALAVRA NA WEB ATRAVÉS DO SITE DICIO.COM.BR
    void classeGramaticalGeral(string consulta){
      CURL *link;
      CURLcode res;
      string conteudo_web;
      bool encontrei = false;
      string classificacao;
  
      link = curl_easy_init();
      if (link) {
          string url = "https://dicio.com.br/";
          url += consulta;
          // URL PARA QUAL O CURL IRÁ FAZER A SOLICITAÇÃO
          curl_easy_setopt(link, CURLOPT_URL, url.c_str());
          curl_easy_setopt(link, CURLOPT_WRITEFUNCTION, WriteCallback_1);
          // DOWNLOAD DA PÁGINA WEB
          curl_easy_setopt(link, CURLOPT_WRITEDATA, &conteudo_web);
          // CONFIGURAR O cURL PARA SEGUIR O REDIRECIONAMENTO AUTOMATICAMENTE
          curl_easy_setopt(link, CURLOPT_FOLLOWLOCATION, 1L);
        
          res = curl_easy_perform(link);
          curl_easy_cleanup(link);
  
          if (res != CURLE_OK) {
              cerr << "Falha para fazer o download da página HTML: " << curl_easy_strerror(res) << endl;
          }
  
          // EXPRESSÕES REGULARES PARA CAPTURAR O CONTEÚDO ESPECÍFICO REFERENTE A CLASSIFICAÇÃO NA PÁGINA WEB
          regex pattern("<span class=\"cl\">(.*?)</span>");
          smatch match;
          string::const_iterator search_start(conteudo_web.cbegin());
  
          while (regex_search(search_start, conteudo_web.cend(), match, pattern)) {
              string classe = match[1];
              // LIMPAR AS TAGS DESNECESSÁRIAS PARA FILTRAR APENAS OS SINÔNIMOS
              classificacao = regex_replace(classe, regex("<.*?|\\s.*"), "");
              if(classificacao.find("span")==std::string::npos && classificacao.find("prefixo")==std::string::npos){
                // O DICIO AO INVÉS DE RETORNAR O SUBSTANTIVO, RETORNA "NOME", ESSA CONDIÇÃO IRÁ RESOLVER ISSO!
                if(classificacao=="adjetivo"){
                  if(tokens[nToken-1].classificacao == "substantivo"){
                    tokens[nToken].lex = palavra;
                    tokens[nToken].lexSemAcento = consulta;
                    tokens[nToken].classificacao = "substantivo";
                    tokens[nToken].permiteSinonimo = false;
                    nToken++;
                  }else{
                    tokens[nToken].lex = palavra;
                    tokens[nToken].lexSemAcento = consulta;
                    tokens[nToken].classificacao = classificacao;
                    nToken++;
                  }
                }else{
                  if(classificacao == "substantivo" && tokens[nToken-1].classificacao == "adjetivo"){
                    tokens[nToken-1].classificacao = "substantivo";
                    tokens[nToken-1].permiteSinonimo = false;
                  }
                  tokens[nToken].lex = palavra;
                  tokens[nToken].lexSemAcento = consulta;
                  tokens[nToken].classificacao = classificacao;
                  nToken++;
                }
                  encontrei = true;
                  break;
              }
  
              // ENCAMINHANDO PARA A PRÓXIMA OCORRÊNCIA
              search_start = match.suffix().first;
          }
          if(!encontrei){
            // O DICIO.COM.BR NÃO FAZ A CLASSIFICAÇÃO DE VERBOS FLEXIONADOS, POR ISSO FOI NECESSÁRIO USAR OUTRO SITE WEB. O "dicionario.priberam.org", QUE IRÁ RETORNAR CORRETAMENTO O NOME DO VERBO
            classeGramaticalVerbosFlexionados(palavra);
          }
        } else {
            cerr << "Falha para iniciar o cURL." << endl;
        }
      }
  
      
    // FUNÇÃO PARA CLASSIFICAR O TIPO DA PALAVRA NA WEB ATRAVÉS DO SITE "dicionario.priberam.org"
    void classeGramaticalVerbosFlexionados(string consultaVerbo){
      CURL *link;
      CURLcode res;
      string conteudo_web;
      bool encontrei = false;
      string classificacao;
  
      link = curl_easy_init();
      if (link) {
          string url = "https://dicionario.priberam.org/";
          url += palavra;
          // URL PARA QUAL O CURL IRÁ FAZER A SOLICITAÇÃO
          curl_easy_setopt(link, CURLOPT_URL, url.c_str());
          curl_easy_setopt(link, CURLOPT_WRITEFUNCTION, WriteCallback_1);
          // DOWNLOAD DA PÁGINA WEB
          curl_easy_setopt(link, CURLOPT_WRITEDATA, &conteudo_web);
  
          // CONFIGURAR O cURL PARA SEGUIR O REDIRECIONAMENTO AUTOMATICAMENTE
          curl_easy_setopt(link, CURLOPT_FOLLOWLOCATION, 1L);
  
          res = curl_easy_perform(link);
          curl_easy_cleanup(link);
  
          if (res != CURLE_OK) {
              cerr << "Falha para fazer o download da página HTML: " << curl_easy_strerror(res) << endl;
          }
  
          // EXPRESSÕES REGULARES PARA CAPTURAR O CONTEÚDO DA PÁGINA WEB
          regex pattern("<span class=\"\">(.*?)</span>");
          smatch match;
          string::const_iterator search_start(conteudo_web.cbegin());
  
          while (regex_search(search_start, conteudo_web.cend(), match, pattern)) {
              string classe = match[1];
              // LIMPAR AS TAGS DESNECESSÁRIAS PARA FILTRAR APENAS OS SINÔNIMOS
              classificacao = regex_replace(classe, regex("<.*?|\\s.*"), "");
  
              if(classificacao.find("span")==std::string::npos && classificacao.find("prefixo")==std::string::npos){
                if(classificacao=="nome"){
                  tokens[nToken].lex = palavra;
                  tokens[nToken].lexSemAcento = consultaVerbo;
                  tokens[nToken].classificacao = "substantivo";
                  if(tokens[nToken].classificacao == "substantivo" && tokens[nToken-1].classificacao == "adjetivo"){
                    tokens[nToken-1].classificacao = "substantivo";
                    tokens[nToken-1].permiteSinonimo = false;
                  }
                  nToken++;
                }else{
                  if(tokens[nToken].classificacao == "substantivo" && tokens[nToken-1].classificacao == "adjetivo"){
                    tokens[nToken-1].classificacao = "substantivo";
                    tokens[nToken-1].permiteSinonimo = false;
                  }
                  tokens[nToken].lex = palavra;
                  tokens[nToken].lexSemAcento = consultaVerbo;
                  tokens[nToken].classificacao = classificacao;
                  nToken++;
                }
                encontrei = true;
                break;
              }
  
              // ENCAMINHANDO PARA A PRÓXIMA OCORRÊNCIA
              search_start = match.suffix().first;
          }
        // CASO NÃO ENCONTRE NENHUMA OCORRÊNCIA NO SITE, SIGNIFICA QUE ESSA PALAVRA NÃO ESTÁ NOS DICIONÁRIOS OU NÃO HÁ UMA CLASSIFICAÇÃO CORRETA. NESSE CASO, A PALAVRA É CLASSIFICADA COMO "SUBSTANTIVO PRÓPRIO", DANDO A ENTENDER QUE SE TRATE DE UMA PALAVRA ÚNICA COMO UM NOME POR EXEMPLO.
        if(!encontrei){
          tokens[nToken].lex = consultaVerbo;
          tokens[nToken].lexSemAcento = consultaVerbo;
          tokens[nToken].classificacao = "substantivo próprio";
          tokens[nToken].permiteSinonimo = false;
          nToken++;
        }
      } else {
          cerr << "Falha para iniciar o cURL." << endl;
      }
    }
  
};