#include <iostream>
#include <locale>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <unordered_map>

using namespace std;

struct Produto {
    int id;
    string nome;
    bool vendidoPorPeso; // true = peso, false = unidade
    float valor;
    int quantidadeDisponivel; // Quantidade em estoque
};

// Função para carregar produtos do arquivo
vector<Produto> carregarProdutos(const string& nomeArquivo) {
    vector<Produto> produtos;
    ifstream arquivo(nomeArquivo);
    if (arquivo.is_open()) {
        Produto produto;
        while (arquivo >> produto.id >> produto.nome >> produto.vendidoPorPeso >> produto.valor >> produto.quantidadeDisponivel) {
            produtos.push_back(produto);
        }
        arquivo.close();
    }
    return produtos;
}

// Função para salvar produtos no arquivo
void salvarProdutos(const vector<Produto>& produtos, const string& nomeArquivo) {
    ofstream arquivo(nomeArquivo);
    for (const auto& produto : produtos) {
        arquivo << produto.id << " " << produto.nome << " " << produto.vendidoPorPeso << " " 
                << fixed << setprecision(2) << produto.valor << " "
                << produto.quantidadeDisponivel << "\n";
    }
}

// Função para listar produtos
void listarProdutos(const vector<Produto>& produtos) {
    cout << "Lista de Produtos:\n";
    for (const auto& produto : produtos) {
        cout << "ID: " << produto.id << ", Nome: " << produto.nome 
             << ", Tipo: " << (produto.vendidoPorPeso ? "Peso" : "Unidade") 
             << ", Valor: R$ " << fixed << setprecision(2) << produto.valor 
             << ", Quantidade disponível: " << produto.quantidadeDisponivel << "\n";
    }
}

// Função para reindexar os IDs dos produtos
void reindexarProdutos(vector<Produto>& produtos) {
    for (size_t i = 0; i < produtos.size(); ++i) {
        produtos[i].id = i + 1;
    }
}

// Função para ler o valor do produto, aceitando ponto ou vírgula como separador decimal
float lerValorProduto() {
    string valorStr;
    while (true) {
        cout << "Valor: ";
        cin >> valorStr;

        // Substituir vírgula por ponto, se houver
        for (char& c : valorStr) {
            if (c == ',') c = '.';
        }

        try {
            // Tentar converter para float
            return stof(valorStr);
        } catch (invalid_argument&) {
            cout << "Entrada inválida. Insira um valor numérico.\n";
        }
    }
}

// Função para definir se o produto é vendido por peso ou unidade
bool definirVendidoPorPeso() {
    int tipo;
    while (true) {
        cout << "Vendido por peso (1) ou unidade (0): ";
        cin >> tipo;

        if (!cin.fail() && (tipo == 0 || tipo == 1)) {
            return tipo == 1; // Retorna true se for vendido por peso (1), false se for unidade (0)
        } else {
            cout << "Entrada inválida. Por favor, digite 1 para peso ou 0 para unidade.\n";
            cin.clear(); // Limpa o estado de erro
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignora a entrada inválida
        }
    }
}

// Função para criar um novo produto
bool criarProduto(vector<Produto>& produtos) {
    Produto novoProduto;
    novoProduto.id = produtos.size() + 1;
    
    cout << "Nome do novo produto: ";
    cin >> novoProduto.nome;
    if (novoProduto.nome == "voltar") return false;

    novoProduto.vendidoPorPeso = definirVendidoPorPeso();
    novoProduto.valor = lerValorProduto();

    cout << "Quantidade disponível: ";
    cin >> novoProduto.quantidadeDisponivel;

    produtos.push_back(novoProduto);
    cout << "Produto criado com sucesso! ID: " << novoProduto.id << "\n";
    return true;
}

// Função para verificar a entrada do ID e garantir que seja um número válido
int lerIdProduto() {
    string entrada;
    while (true) {
        cout << "Digite o ID do produto (ou digite 'voltar' para retornar): ";
        cin >> entrada;

        if (entrada == "voltar") {
            return -1; // Retorna -1 para indicar que o usuário decidiu voltar
        }

        try {
            return stoi(entrada); // Tenta converter a entrada para um inteiro
        } catch (invalid_argument&) {
            cout << "Entrada inválida. Por favor, insira um número.\n";
        }
    }
}

// Função para modificar um produto existente
bool modificarProduto(vector<Produto>& produtos) {
    if (produtos.empty()) {
        cout << "Nenhum produto disponível para modificar.\n";
        return true;
    }
    
    listarProdutos(produtos);
    int id;
    bool produtoEncontrado = false;

    while (!produtoEncontrado) {
        id = lerIdProduto();
        if (id == -1) return false; // Se o usuário decidiu voltar

        for (auto& produto : produtos) {
            if (produto.id == id) {
                cout << "Novo nome do produto: ";
                cin >> produto.nome;
                if (produto.nome == "voltar") return false;

                produto.vendidoPorPeso = definirVendidoPorPeso();
                produto.valor = lerValorProduto();
                cout << "Produto modificado com sucesso!\n";
                produtoEncontrado = true;
                break;
            }
        }
        
        if (!produtoEncontrado) {
            cout << "Produto com ID " << id << " não encontrado. Tente novamente.\n";
        }
    }
    return true;
}

// Função para remover um produto
bool removerProduto(vector<Produto>& produtos) {
    if (produtos.empty()) {
        cout << "Nenhum produto disponível para remover.\n";
        return true;
    }
    
    listarProdutos(produtos);
    int id;
    bool produtoEncontrado = false;

    while (!produtoEncontrado) {
        id = lerIdProduto();
        if (id == -1) return false; // Se o usuário decidiu voltar

        for (auto it = produtos.begin(); it != produtos.end(); ++it) {
            if (it->id == id) {
                string confirmacao;
                cout << "Tem certeza que deseja remover o produto '" << it->nome << "'? (digite 'sim' para confirmar ou 'voltar' para retornar): ";
                cin >> confirmacao;

                if (confirmacao == "voltar") return false;

                if (confirmacao == "sim") {
                    produtos.erase(it);
                    reindexarProdutos(produtos); // Reindexa os IDs após a remoção
                    cout << "Produto removido e IDs atualizados com sucesso!\n";
                } else {
                    cout << "Remoção cancelada.\n";
                }
                produtoEncontrado = true;
                break;
            }
        }
        
        if (!produtoEncontrado) {
            cout << "Produto com ID " << id << " não encontrado. Tente novamente.\n";
        }
    }
    return true;
}

// Função para adicionar quantidade ao estoque
void adicionarEstoque(vector<Produto>& produtos) {
    if (produtos.empty()) {
        cout << "Nenhum produto disponível para adicionar ao estoque.\n";
        return;
    }
    
    listarProdutos(produtos);
    int id = lerIdProduto();
    if (id == -1) return;

    for (auto& produto : produtos) {
        if (produto.id == id) {
            int quantidade;
            cout << "Digite quanto deseja adicionar ou remover do estoque: ";
            cin >> quantidade;
            produto.quantidadeDisponivel += quantidade;
            cout << "Estoque atualizado! Nova quantidade disponível: " << produto.quantidadeDisponivel << "\n";
            return;
        }
    }
    cout << "Produto com ID " << id << " não encontrado.\n";
}

struct Venda {
    int id;
    string nome;
    double faturamento;
    double quantidade;
    string data;
};

// Função para carregar as vendas do arquivo "vendas.txt"
vector<Venda> carregarVendas(const string& nomeArquivo) {
    vector<Venda> vendas;
    ifstream arquivo(nomeArquivo);

    if (arquivo.is_open()) {
        Venda venda;
        while (arquivo >> venda.id >> venda.nome >> venda.faturamento >> venda.quantidade >> venda.data) {
            vendas.push_back(venda);
        }
        arquivo.close();
    } else {
        cout << "Erro ao abrir o arquivo " << nomeArquivo << endl;
    }
    return vendas;
}

// Função para verificar se a data está no intervalo especificado
bool dataNoIntervalo(const string& data, const string& dataInicio, const string& dataFim) {
    return data >= dataInicio && data <= dataFim;
}

// Função para gerar o relatório de vendas com base em uma data de início e uma data de fim
void gerarRelatorioVendas(const vector<Venda>& vendas, const string& dataInicio, const string& dataFim) {
    unordered_map<string, pair<double, double>> resumoVendas; // Map para acumular faturamento e quantidade por produto

    // Processa cada venda e acumula os dados para produtos no intervalo especificado
    for (const auto& venda : vendas) {
        if (dataNoIntervalo(venda.data, dataInicio, dataFim)) {
            resumoVendas[venda.nome].first += venda.faturamento;  // Soma o faturamento
            resumoVendas[venda.nome].second += venda.quantidade;  // Soma a quantidade
        }
    }

    // Exibe o relatório de vendas consolidado
    cout << "\nRelatório de Vendas de " << dataInicio << " a " << dataFim << ":\n";
    cout << setw(15) << "Produto" << setw(15) << "Faturamento" << setw(15) << "Quantidade" << endl;

    double totalFaturamento = 0.0;
    double totalQuantidade = 0.0;

    for (const auto& item : resumoVendas) {
        const string& nome = item.first;
        double faturamento = item.second.first;
        double quantidade = item.second.second;

        cout << setw(15) << nome
             << setw(15) << faturamento
             << setw(15) << quantidade << endl;

        totalFaturamento += faturamento;
        totalQuantidade += quantidade;
    }

    cout << "\nTotal Faturamento: " << totalFaturamento << endl;
    cout << "Total Quantidade Vendida: " << totalQuantidade << endl;
}

int main() {
    std::setlocale(LC_ALL, "en_US.UTF-8");
    
    const string nomeArquivo = "produtos.txt";
    vector<Produto> produtos = carregarProdutos(nomeArquivo);

    const string nomeArquivoVendas = "vendas.txt";
    vector<Venda> vendas = carregarVendas(nomeArquivoVendas);


    string entrada;
    int opcao;
    do {
        cout << "\n1. Criar novo produto\n";
        if (!produtos.empty()) {
            cout << "2. Modificar produto existente\n3. Remover produto\n4. Atualizar estoque\n5. Gerar relatório de vendas\n";
        }
        cout << "6. Sair\nEscolha uma opção: ";
        cin >> entrada;

        if (entrada == "voltar") continue;

        try {
            opcao = stoi(entrada);
        } catch (invalid_argument&) {
            cout << "Entrada inválida. Por favor, insira um número ou 'voltar'.\n";
            continue;
        }

        if (opcao == 1) {
            if (criarProduto(produtos)) {
                salvarProdutos(produtos, nomeArquivo);
            }
        } else if (opcao == 2 && !produtos.empty()) {
            if (modificarProduto(produtos)) {
                salvarProdutos(produtos, nomeArquivo);
            }
        } else if (opcao == 3 && !produtos.empty()) {
            if (removerProduto(produtos)) {
                salvarProdutos(produtos, nomeArquivo);
            }
        } else if (opcao == 4 && !produtos.empty()) {
            adicionarEstoque(produtos);
            salvarProdutos(produtos, nomeArquivo);
        } else if (opcao == 5) {
            string dataInicio, dataFim;
            cout << "Digite a data de início (AAAA-MM-DD): ";
            cin >> dataInicio;
            cout << "Digite a data de fim (AAAA-MM-DD): ";
            cin >> dataFim;

            gerarRelatorioVendas(vendas, dataInicio, dataFim);
        } else if (opcao == 6) {
            cout << "Saindo...\n";
        } else {
            cout << "Opção inválida.\n";
        }
    } while (opcao != 6);

    return 0;
}


