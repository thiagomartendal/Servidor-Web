#include <iostream>
#include "servidor.h"

void teste1() {
    Servidor servidor = Servidor(4000);
    servidor.adicionarRequisicaoParaArquivo("GET", "/", "text/html", "public/index.html");
    servidor.adicionarRequisicaoParaArquivo("GET", "/index.html", "text/html", "public/index.html");
    servidor.adicionarRequisicaoParaArquivo("GET", "/estilo.css", "text/css", "public/estilo.css");
    servidor.adicionarRequisicaoParaArquivo("GET", "/logo.png", "image/png", "public/logo.png");
    servidor.adicionarRequisicao("GET", "/api", "text/html", "<!DOCTYPE html>\n<link rel=\"icon\" href=\"data:;base64,=\">\n<h2>valor: 123</h2>");
    servidor.adicionarRequisicao("PUT", "/", "application/json", "{\"valor\": \"ABC\"}");
    servidor.adicionarRequisicao("POST", "/api", "application/json", "{\"valor\": \"123\"}");
    servidor.iniciar();
}

void teste2() {
    Servidor servidor = Servidor(4000);
    servidor.bloquearRotasAutomaticas();
    servidor.adicionarRequisicao("GET", "/api", "text/html", "<!DOCTYPE html>\n<link rel=\"icon\" href=\"data:;base64,=\">\n<h2>valor: 123</h2>");
    servidor.adicionarRequisicao("PUT", "/", "application/json", "{\"valor\": \"ABC\"}");
    servidor.adicionarRequisicao("POST", "/api", "application/json", "{\"valor\": \"123\"}");
    servidor.adicionarPastaRaiz("public/"); // Ou "public"
    servidor.adicionarRequisicaoParaArquivoLido("GET", "/", "public/index.html");
    servidor.iniciar();
}

int main() {
    // teste1();
    teste2();
    return 0;
}