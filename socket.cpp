#include "socket.h"

/*
* Construtor com as definições de operação do socket
*/
Socket::Socket(int porta, Tipo tipo, std::string host) {
  this->porta = porta;
  this->tipo = tipo; // Determina se o socket opera como cliente ou servidor
  this->host = host;
}

/*
* Inicializa a conexao do socket, podendo iniciar como servidor ou como cliente
*/
void Socket::iniciar() {
  if (tipo == Tipo::SERVIDOR) Socket::iniciarServidor();
  else Socket::iniciarCliente();
}

/*
* Inicializa o servidor
*/
void Socket::iniciarServidor() {
  try {
    int opt = 1;

    // Preparando o socket
    if ((def.servico = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      throw std::string("Falha ao iniciar o socket.");

    // Atribuindo propriedades ao socket
    if (setsockopt(def.servico, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
      throw std::string("Falha na atribuição de propriedades do socket.");

    // Conectando o socket a porta
    def.endereco.sin_family = AF_INET;
    def.endereco.sin_addr.s_addr = INADDR_ANY;
    def.endereco.sin_port = htons(porta);

    // Iniciando a ligação
    if (bind(def.servico, (struct sockaddr*)&def.endereco, sizeof(def.endereco)) < 0)
      throw std::string("Falha na ligação do socket.");

    // Preparando a escuta da porta
    if (listen(def.servico, 3) < 0)
      throw std::string("Falha na escuta do socket.");
  } catch(std::string erro) {
    std::cout << erro << std::endl;
  }
}

/*
* Inicializa o cliente
*/
void Socket::iniciarCliente() {
  try {
    // Preparando o socket
    if ((def.servico = socket(AF_INET, SOCK_STREAM, 0)) < 0)
      throw std::string("Erro na criação do socket.");

    // Conectando o socket a porta
    def.endereco.sin_family = AF_INET;
    def.endereco.sin_port = htons(porta);

    // Atribuindo o host
    if (inet_pton(AF_INET, host.c_str(), &def.endereco.sin_addr) <= 0)
      throw std::string("O endereço de host " + host + " é invalido ou não supportado.");

    // Conectando o socket
    if ((def.novoSocket = connect(def.servico, (struct sockaddr*)&def.endereco, sizeof(def.endereco))) < 0)
      throw std::string("Falha na conexão.");
  } catch(std::string erro) {
    std::cout << erro << std::endl;
  }
}

/*
* Realiza a aceitação da conexão do socket
*/
void Socket::aceitar() {
  try {
    // Finalizando o processo de inicialização e aceita a conexão do socket
    if (tipo == Tipo::SERVIDOR) {
      socklen_t tamanhoEndereco = sizeof(def.endereco);
      if ((def.novoSocket = accept(def.servico, (struct sockaddr*)&def.endereco, &tamanhoEndereco)) < 0)
        throw std::string("Falha na aceitação do socket.");
    }
  } catch(std::string erro) {
    std::cout << erro << std::endl;
  }
}

/*
* Encerra a aceitação atribuída (Apenas para o servidor)
*/
void Socket::concluir() {
  // Encerra a conexão aceita com o socket
  if (tipo == Tipo::SERVIDOR) {
    close(def.novoSocket);
  }
}

/*
* Encerra a conexão com o socket
*/
void Socket::encerrar() {
  close(def.servico);
}

/*
* Efetua a leitura de uma mensagem enviada por um cliente
*/
std::string Socket::lerMensagem() {
  char buffer[1024] = {0};
  if (tipo == Tipo::SERVIDOR) read(def.novoSocket, buffer, 1023);
  else read(def.servico, buffer, 1023);
  def.mensagemLida = std::string(buffer);
  return def.mensagemLida;
}

/*
* Envia uma mensagem para um cliente
*/
void Socket::enviarMensagem(std::string mensagem) {
  if (tipo == Tipo::SERVIDOR) send(def.novoSocket, mensagem.c_str(), mensagem.size(), 0);
  else send(def.servico, mensagem.c_str(), mensagem.size(), 0);
}
