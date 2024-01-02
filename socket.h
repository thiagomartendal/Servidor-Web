#ifndef SOCKET_H
#define SOCKET_H

#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

enum Tipo {
  SERVIDOR,
  CLIENTE
};

struct Definicao {
  int servico, novoSocket;
  struct sockaddr_in endereco;
  std::string mensagemLida;
};

class Socket {
private:
  int porta;
  std::string host;
  Tipo tipo;
  Definicao def;
  void iniciarServidor();
  void iniciarCliente();

public:
  Socket() = default;
  Socket(int porta, Tipo tipo, std::string host = "127.0.0.1");
  void iniciar();
  void aceitar();
  void concluir();
  void encerrar();
  void enviarMensagem(std::string mensagem);
  std::string lerMensagem();
};

#endif
