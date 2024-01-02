#ifndef SERVIDOR_H
#define SERVIDOR_H

#include <vector>
#include <map>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include "socket.h"

struct Requisicao {
  std::string tipoConteudo;
  std::string mensagem;
};

class Servidor {
private:
  Socket socket;
  std::map<std::pair<std::string, std::string>, Requisicao> requisicoes;
  std::vector<std::string> arquivosHTML;
  std::string mensagemInicial, respostaAnterior;
  bool bloqueioAutomatico{false};
  void testarSocket();
  void comunicacao(std::string mensagem);
  std::pair<std::string, std::string> formarIndice(std::string str);
  std::string lerArquivo(std::string nomeArquivo);
  std::string tipoMIME(std::string arquivo);

public:
  Servidor(int porta);
  void definirOrigemCruzada(std::string alvo);
  void bloquearRotasAutomaticas();
  void adicionarRequisicao(std::string metodo, std::string rota, std::string tipoConteudo, std::string mensagem);
  void adicionarRequisicaoParaArquivo(std::string metodo, std::string rota, std::string tipoConteudo, std::string nomeArquivo);
  void adicionarPastaRaiz(std::string pasta);
  void adicionarRequisicaoParaArquivoLido(std::string metodo, std::string rota, std::string nomeArquivo);
  void iniciar();
};

#endif
