import csv
import os #limpar terminal / pausar terminal
import sys #fechar programa

def limpar_tela():
    os.system('cls' if os.name == 'nt' else 'clear')


def pausar():
    os.system('pause' if os.name == 'nt' else 'read -p "Pressione Enter para continuar..."')


class Registro:
    def __init__(self, arquivo_path):
        self.arquivo_path = arquivo_path
        self.dados_jogos = DadosJogos()

    def ler_arquivo(self):
        with open(self.arquivo_path, 'r') as arquivo:
            leitor = csv.reader(arquivo)
            next(leitor)  # Pula o cabeçalho
            for linha in leitor:
                jogo = Jogo(
                    linha[0],
                    linha[1],
                    linha[2],
                    float(linha[3]),
                    float(linha[4]),
                    linha[5],
                    float(linha[6].rstrip('M'))
                )
                self.dados_jogos.todos_jogos.append(jogo)
                self.dados_jogos.atualizar_menor_players(jogo)
                self.dados_jogos.atualizar_maior_players(jogo)
                self.dados_jogos.atualizar_maior_vendas(jogo)
                self.dados_jogos.atualizar_menor_vendas(jogo)


class DadosJogos:
    def __init__(self):
        self.menor_players = None
        self.maior_players = None
        self.maior_vendas = None
        self.menor_vendas = None
        self.todos_jogos = []

    def atualizar_menor_players(self, jogo):
        if self.menor_players is None or jogo.player_min < self.menor_players.player_min:
            self.menor_players = jogo

    def atualizar_maior_players(self, jogo):
        if self.maior_players is None or jogo.player_min > self.maior_players.player_min:
            self.maior_players = jogo

    def atualizar_maior_vendas(self, jogo):
        if self.maior_vendas is None or jogo.vendas > self.maior_vendas.vendas:
            self.maior_vendas = jogo

    def atualizar_menor_vendas(self, jogo):
        if self.menor_vendas is None or jogo.vendas < self.menor_vendas.vendas:
            self.menor_vendas = jogo

    def menu(self):
        print("1 - Jogo com menor número de players")
        print("2 - Jogo com maior número de players")
        print("3 - Jogo com maior número de vendas")
        print("4 - Jogo com menor número de vendas")
        print("5 - Pesquisar por data")
        print("6 - Pesquisar por nome do jogo")
        print("7 - Visualizar Jogos do arquivo")
        print("8 - Visualizar arquivo completo")
        print("9 - Sair")
        try:
            valor = int(input("Digite o valor: "))
        except ValueError:
            limpar_tela()
            print("Opção inválida!")
            self.menu()
            return
        self.exibir_dados(valor)

    def exibir_dados(self, valor):
        limpar_tela()
        if valor == 1 and self.menor_players:
            print(f"Jogo com menor número de players: {self.menor_players.nome_jogo}")
            print(f"Data: {self.menor_players.data}")
            print(f"Número de players: {self.menor_players.player_min}")
        elif valor == 2 and self.maior_players:
            print(f"Jogo com maior número de players: {self.maior_players.nome_jogo}")
            print(f"Data: {self.maior_players.data}")
            print(f"Número de players: {self.maior_players.player_min}")
        elif valor == 3 and self.maior_vendas:
            print(f"Jogo com maior número de vendas: {self.maior_vendas.nome_jogo}")
            print(f"Data: {self.maior_vendas.data}")
            print(f"Número de vendas: {self.maior_vendas.vendas}M")
        elif valor == 4 and self.menor_vendas:
            print(f"Jogo com menor número de vendas: {self.menor_vendas.nome_jogo}")
            print(f"Data: {self.menor_vendas.data}")
            print(f"Número de vendas: {self.menor_vendas.vendas}M")
        elif valor == 5:
            data = input("Digite a data para busca (formato igual ao do CSV): ")
            self.buscar_jogo(data)
        elif valor == 6:
            nome_jogo = input("Digite o nome do jogo para busca: ")
            self.listar_jogo(nome_jogo)
        elif valor == 7:
            self.listar_nome_jogo()
        elif valor == 8:
            self.listar_arquivo()
        elif valor == 9:
            print("Saindo...")
            sys.exit()
        else:
            print("Opção inválida ou dados indisponíveis.")

        pausar()
        limpar_tela()
        self.menu()

    def listar_jogo(self, nome_jogo):
        encontrados = False
        for jogo in self.todos_jogos:
            if jogo.nome_jogo.lower() == nome_jogo.lower():
                jogo.listar_jogos()
                encontrados = True
        if not encontrados:
            print("Jogo não encontrado.")

    def buscar_jogo(self, data):
        encontrados = False
        for jogo in self.todos_jogos:
            if jogo.data == data:
                encontrados = True
                jogo.listar_jogos()
        if not encontrados:
            print("Nenhum jogo encontrado para essa data.")

    def listar_nome_jogo(self):
        nomes_exibidos = set()
        for jogo in self.todos_jogos:
            if jogo.nome_jogo not in nomes_exibidos:
                print(jogo.nome_jogo)
                nomes_exibidos.add(jogo.nome_jogo)

    def listar_arquivo(self):
        with open('tabela_jogos.csv', 'r') as arquivo:
            leitor = csv.reader(arquivo)
            for linha in leitor:
                print(f"{linha[0]} |\t {linha[1]} |\t {linha[2]} |\t {linha[3]} |\t {linha[4]} |\t {linha[5]} |\t {linha[6]}\n")



class Jogo:
    def __init__(self, data, nome_jogo, plataforma, player_min, player_max, variacao_preco, vendas):
        self.data = data
        self.nome_jogo = nome_jogo
        self.plataforma = plataforma
        self.player_min = player_min
        self.player_max = player_max
        self.variacao_preco = variacao_preco
        self.vendas = vendas

    def listar_jogos(self):
        print(f"Data: {self.data}")
        print(f"Nome do jogo: {self.nome_jogo}")
        print(f"Plataforma: {self.plataforma}")
        print(f"Número mínimo de players: {self.player_min}")
        print(f"Número máximo de players: {self.player_max}")
        print(f"Variação de preço: {self.variacao_preco}")
        print(f"Vendas: {self.vendas}M")
        print("-" * 30)


def main():
    registro = Registro('tabela_jogos.csv')
    registro.ler_arquivo()
    registro.dados_jogos.menu()

if __name__ == '__main__':
    main()
