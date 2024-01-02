#include "servidor.h"

/*
* Construtor com as definições de inicialização do servidor
*/
Servidor::Servidor(int porta) {
  mensagemInicial =
  "HTTP/2 200 OK\n"
  "Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept\n"
  "Access-Control-Allow-Methods: CONNECT, DELETE, GET, HEAD, OPTIONS, PATCH, POST, PUT, TRACE\n"
  "Access-Control-Allow-Credentials: true\n";
  socket = Socket(porta, Tipo::SERVIDOR);
  socket.iniciar();
  Servidor::testarSocket();
}

/*
* Teste de funcionalidades do socket para preparar a troca de requisições
*/
void Servidor::testarSocket() {
  // Método necessário para se testar a conexão com o socket e habilitar as requisições entre cliente e servidor
  // O teste também faz com que a comunicação entre cliente e servidor ocorra ao se pressionar CTRL + F5 na página
  socket.aceitar();
  socket.concluir();
}

/*
* Realiza o envio de requisições HTTP com base nos pedidos do cliente
*/
void Servidor::comunicacao(std::string mensagem) {
  socket.aceitar();
  std::string resposta = socket.lerMensagem();
  if ((resposta == "" || resposta == " ") && (respostaAnterior != "")) {
    resposta = respostaAnterior;
  } else respostaAnterior = resposta;
  std::cout << resposta << std::endl;
  std::pair<std::string, std::string> indice = Servidor::formarIndice(resposta);
  if (requisicoes.find(indice) != requisicoes.end() || indice.first == "OPTIONS") {
    Requisicao req = requisicoes[indice];
    mensagem += "Content-Type: " + req.tipoConteudo + "; Charset=UTF-8\n\n";
    mensagem += req.mensagem;
    socket.enviarMensagem(mensagem);
    std::cout << "Requisição HTTP enviada para " << indice.first << " " << indice.second << "." << std::endl;
  } else std::cout << "A requisição HTTP não foi enviada pois a rota " << indice.second << " não está registrada para método " << indice.first << "." << std::endl;
  socket.concluir();
}

/*
* Forma um índice a partir da requisição do cliente
*/
std::pair<std::string, std::string> Servidor::formarIndice(std::string str) {
  std::string metodo = "", rota = "";
  while (!isspace(str[0])) {
    metodo += str[0];
    str.erase(0, 1);
  }
  str.erase(0, 1);
  int i = 0;
  while (!isspace(str[i])) {
    rota += str[i];
    i++;
  }
  return std::make_pair(metodo, rota);
}

/*
* Habilita as requisições do servidor a atenderem uma origem cruzada
*/
void Servidor::definirOrigemCruzada(std::string alvo) {
  mensagemInicial += "Access-Control-Allow-Origin: " + alvo + "\n";
}

/*
* Impede que rotas automáticas sejam geradas para arquivos HTML lidos no método adicionarPastaRaiz,
* assim é possível usar os arquivos lidos em rotas definidas manualmente
*/
void Servidor::bloquearRotasAutomaticas() {
  bloqueioAutomatico = true;
}

/*
* Adiciona uma requisição HTTP ao servidor
*/
void Servidor::adicionarRequisicao(std::string metodo, std::string rota, std::string tipoConteudo, std::string mensagem) {
  try {
    std::pair<std::string, std::string> indice = std::make_pair(metodo, rota);
    if (requisicoes.find(indice) != requisicoes.end())
      throw std::string("A requisição não pode ser cadastrada, pois o caminho " + metodo + " " + rota + " já está em uso.");
    requisicoes[indice] = {tipoConteudo, mensagem};
  } catch(std::string erro) {
    std::cout << erro << std::endl;
  }
}

/*
* Adiciona uma requisição HTTP que envia o conteúdo de um arquivo para o cliente
*/
void Servidor::adicionarRequisicaoParaArquivo(std::string metodo, std::string rota, std::string tipoConteudo, std::string nomeArquivo) {
  std::string conteudo = Servidor::lerArquivo(nomeArquivo);
  Servidor::adicionarRequisicao(metodo, rota, tipoConteudo, conteudo);
}

/*
* Lê o conteúdo de um arquivo solicitado
*/
std::string Servidor::lerArquivo(std::string nomeArquivo) {
  std::string conteudo = "";
  std::ifstream arquivo = std::ifstream(nomeArquivo, std::ifstream::in);
  while (arquivo.good()) {
    std::string linha;
    getline(arquivo, linha); 
    conteudo += linha + "\n";
  }
  return conteudo;
}

/*
* Adiciona o conteúdo de todos os arquivos de uma pasta ao servidor
*/
void Servidor::adicionarPastaRaiz(std::string pasta) {
  for (const auto &caminho : std::filesystem::directory_iterator(pasta)) {
    std::string arquivo = caminho.path();
    std::string extensao = caminho.path().extension();
    std::string tipoMIME = Servidor::tipoMIME(arquivo);
    if (bloqueioAutomatico && tipoMIME == "text/html") arquivosHTML.push_back(arquivo);
    else {
      if (pasta.find("/") == std::string::npos)
        pasta += '/';
      arquivo.replace(arquivo.begin(), arquivo.begin()+pasta.length(), "");
      if (arquivo[0] == '/')
        arquivo.erase(0, 1);
      std::string tipoConteudo = ((extensao == ".css") ? "text/css" : tipoMIME);
      Servidor::adicionarRequisicaoParaArquivo("GET", "/"+arquivo, tipoConteudo, caminho.path());
    }
  }
}

/*
* Retorna o tipo MIME de um arquivo, para ser utilizado no envio de requisições para o cliente
*/
std::string Servidor::tipoMIME(std::string arquivo) {
  std::string comando = "file --mime-type " + arquivo;
  std::shared_ptr<FILE> pipe(popen(comando.c_str(), "r"), pclose);
  try {
    if (!pipe)
      throw std::string("Erro na execução do comando: " + comando);
    char buffer[128];
    std::string resultado = "";
    while (!feof(pipe.get())) {
      if (fgets(buffer, 128, pipe.get()) != NULL)
        resultado += std::string(buffer);
    }
    while (resultado[0] != ' ') {
      resultado.erase(0, 1);
    }
    resultado.erase(0, 1);
    resultado.pop_back();
    return resultado;
  } catch(std::string erro) {
    std::cout << erro << std::endl;
  }
  return "";
}

/*
* Adiciona uma requisição para um arquivo que foi previamente lido no método adicionarPastaRaiz
*/
void Servidor::adicionarRequisicaoParaArquivoLido(std::string metodo, std::string rota, std::string nomeArquivo) {
  try {
    if (std::find(arquivosHTML.begin(), arquivosHTML.end(), nomeArquivo) != arquivosHTML.end()) {
      Servidor::adicionarRequisicaoParaArquivo(metodo, rota, "text/html", nomeArquivo);
    } else 
      throw std::string("O arquivo especificado não foi lido no diretório.");
  } catch(std::string erro) {
    std::cout << erro << std::endl;
  }
}

/*
* Inicializa as operações do servidor
*/
void Servidor::iniciar() {
  while (true) {
    Servidor::comunicacao(mensagemInicial);
    std::cout << "--------------------------------------" << std::endl;
  }
  // socket.encerrar(); // O socket é encerrado ao se encerrar o servidor
}