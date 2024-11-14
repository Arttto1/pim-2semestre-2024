#include <iostream>
#include <locale>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>
#include <sstream>

using namespace std;

struct Produto {
    int id;
    string nome;
    bool vendidoPorPeso;
    float valor;
    float quantidadeDisponivel;
};

struct ItemCompra {
    Produto produto;
    float quantidade;
};

// Função para obter a data atual no formato "YYYY-MM-DD"
string obterDataAtual() {
    time_t t = time(0);
    struct tm* now = localtime(&t);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", now);
    return buffer;
}

// Função para carregar produtos do arquivo
vector<Produto> carregarProdutos(const string& nomeArquivo) {
    vector<Produto> produtos;
    ifstream arquivo(nomeArquivo);

    if (arquivo.is_open()) {
        string linha;
        while (getline(arquivo, linha)) {
            istringstream iss(linha);
            Produto produto;
            int vendidoPorPeso;

            if (iss >> produto.id >> produto.nome >> vendidoPorPeso >> produto.valor >> produto.quantidadeDisponivel) {
                produto.vendidoPorPeso = (vendidoPorPeso != 0);
                produtos.push_back(produto);
            }
        }
        arquivo.close();
    } else {
        cout << "Erro ao abrir o arquivo de produtos.\n";
    }
    return produtos;
}

// Função para salvar produtos no arquivo após atualização
void salvarProdutos(const vector<Produto>& produtos, const string& nomeArquivo) {
    ofstream arquivo(nomeArquivo, ios::trunc);
    if (arquivo.is_open()) {
        for (const auto& produto : produtos) {
            arquivo << produto.id << " " << produto.nome << " " << produto.vendidoPorPeso << " "
                    << fixed << setprecision(2) << produto.valor << " "
                    << produto.quantidadeDisponivel << "\n";
        }
        arquivo.close();
    } else {
        cout << "Erro ao salvar os produtos no arquivo.\n";
    }
}

// Função para listar produtos
void listarProdutos(const vector<Produto>& produtos) {
    cout << "Lista de Produtos:\n";
    for (const auto& produto : produtos) {
        cout << "ID: " << produto.id << ", Nome: " << produto.nome
             << ", Tipo: " << (produto.vendidoPorPeso ? "Peso" : "Unidade")
             << ", Valor: R$ " << fixed << setprecision(2) << produto.valor
             << ", Quantidade Disponível: " << produto.quantidadeDisponivel << "\n";
    }
}

// Função para listar produtos no carrinho
void listarCarrinho(const vector<ItemCompra>& carrinho) {
    cout << "Produtos no Carrinho:\n";
    for (const auto& item : carrinho) {
        cout << "ID: " << item.produto.id << ", Nome: " << item.produto.nome
             << ", Quantidade: " << item.quantidade
             << (item.produto.vendidoPorPeso ? " kg" : " unidades") << "\n";
    }
}

// Função para adicionar produto ao carrinho
void adicionarProduto(vector<ItemCompra>& carrinho, vector<Produto>& produtos) {
    listarProdutos(produtos);
    int id;
    cout << "Digite o ID do produto para adicionar (ou digite 'voltar' para retornar): ";
    string entrada;
    cin >> entrada;

    if (entrada == "voltar") return;

    try {
        id = stoi(entrada);
    } catch (invalid_argument&) {
        cout << "Entrada inválida. Por favor, insira um número ou 'voltar'.\n";
        return;
    }

    for (auto& produto : produtos) {
        if (produto.id == id) {
            cout << "Quantidade disponível: " << produto.quantidadeDisponivel << "\n";
            float quantidade;
            if (produto.vendidoPorPeso) {
                cout << "Digite o peso (kg): ";
            } else {
                cout << "Digite a quantidade: ";
            }
            cin >> quantidade;

            if (quantidade > produto.quantidadeDisponivel) {
                cout << "Quantidade insuficiente em estoque.\n";
                return;
            }

            produto.quantidadeDisponivel -= quantidade;
            carrinho.push_back({produto, quantidade});
            cout << "Produto adicionado ao carrinho.\n";
            return;
        }
    }
    cout << "Produto com ID " << id << " não encontrado.\n";
}

// Função para remover produto do carrinho
void removerProduto(vector<ItemCompra>& carrinho, vector<Produto>& produtos) {
    if (carrinho.empty()) {
        cout << "Carrinho vazio.\n";
        return;
    }

    listarCarrinho(carrinho);
    int id;
    cout << "Digite o ID do produto para remover (ou digite 'voltar' para retornar): ";
    string entrada;
    cin >> entrada;

    if (entrada == "voltar") return;

    try {
        id = stoi(entrada);
    } catch (invalid_argument&) {
        cout << "Entrada inválida. Por favor, insira um número ou 'voltar'.\n";
        return;
    }

    for (auto it = carrinho.begin(); it != carrinho.end(); ++it) {
        if (it->produto.id == id) {
            // Restaurar a quantidade do produto no estoque
            for (auto& produto : produtos) {
                if (produto.id == it->produto.id) {
                    produto.quantidadeDisponivel += it->quantidade;
                    break;
                }
            }

            carrinho.erase(it);
            cout << "Produto removido do carrinho.\n";
            return;
        }
    }
    cout << "Produto com ID " << id << " não está no carrinho.\n";
}

// Função para atualizar o arquivo de vendas
void atualizarVendas(const vector<ItemCompra>& carrinho) {
    string dataAtual = obterDataAtual();
    fstream arquivo("vendas.txt", ios::in | ios::out | ios::app);

    if (!arquivo.is_open()) {
        cout << "Erro ao abrir o arquivo de vendas.\n";
        return;
    }

    vector<string> linhas;
    string linha;
    bool atualizado;

    while (getline(arquivo, linha)) {
        linhas.push_back(linha);
    }
    arquivo.close();

    for (const auto& item : carrinho) {
        atualizado = false;

        for (auto& linhaExistente : linhas) {
            int idProduto;
            string nomeProduto, data;
            float valorTotal, quantidadeTotal;

            istringstream iss(linhaExistente);
            iss >> idProduto >> nomeProduto >> valorTotal >> quantidadeTotal >> data;

            if (idProduto == item.produto.id && data == dataAtual) {
                valorTotal += item.quantidade * item.produto.valor;
                quantidadeTotal += item.quantidade;

                ostringstream novaLinha;
                novaLinha << idProduto << " " << nomeProduto << " "
                          << fixed << setprecision(2) << valorTotal << " "
                          << quantidadeTotal << " " << data;

                linhaExistente = novaLinha.str();
                atualizado = true;
                break;
            }
        }

        if (!atualizado) {
            ostringstream novaLinha;
            novaLinha << item.produto.id << " " << item.produto.nome << " "
                      << fixed << setprecision(2) << item.quantidade * item.produto.valor << " "
                      << item.quantidade << " " << dataAtual;
            linhas.push_back(novaLinha.str());
        }
    }

    ofstream outFile("vendas.txt", ios::trunc);
    for (const auto& linha : linhas) {
        outFile << linha << "\n";
    }
    outFile.close();
    cout << "Arquivo de vendas atualizado.\n";
}

// Função para fechar a compra e exibir o total
void fecharCompra(vector<ItemCompra>& carrinho, vector<Produto>& produtos, const string& nomeArquivoProdutos) {
    float total = 0.0;
    for (const auto& item : carrinho) {
        float valorProduto = item.quantidade * item.produto.valor;
        total += valorProduto;
        cout << item.produto.nome << " - " << item.quantidade
             << (item.produto.vendidoPorPeso ? " kg: " : " unidades: ")
             << "R$ " << fixed << setprecision(2) << valorProduto << "\n";
    }
    cout << "Total da compra: R$ " << fixed << setprecision(2) << total << "\n";

    atualizarVendas(carrinho);
    salvarProdutos(produtos, nomeArquivoProdutos);
    carrinho.clear();
}

int main() {
     std::setlocale(LC_ALL, "en_US.UTF-8");

    const string nomeArquivo = "produtos.txt";
    vector<Produto> produtos = carregarProdutos(nomeArquivo);
    vector<ItemCompra> carrinho;

    string entrada;
    int opcao;
    do {
        cout << "\n1. Adicionar produto à compra\n2. Remover produto da compra\n3. Fechar compra\n4. Cancelar compra\n5. Sair\nEscolha uma opção: ";
        cin >> entrada;

        if (entrada == "voltar") {
            cout << "Retornando ao menu inicial...\n";
            continue;
        }

        try {
            opcao = stoi(entrada); // Converte a entrada para um inteiro
        } catch (invalid_argument&) {
            cout << "Entrada inválida. Por favor, insira um número ou 'voltar'.\n";
            continue; // Retorna ao início do loop
        }

        switch (opcao) {
            case 1:
                adicionarProduto(carrinho, produtos);
                break;
            case 2:
                removerProduto(carrinho, produtos);
                break;
            case 3:
                fecharCompra(carrinho, produtos, nomeArquivo);
                break;
            case 4:
                carrinho.clear();
                cout << "Compra cancelada.\n";
                break;
            case 5:
                cout << "Saindo...\n";
                break;
            default:
                cout << "Opção inválida.\n";
        }
    } while (opcao != 5);

    return 0;
}
