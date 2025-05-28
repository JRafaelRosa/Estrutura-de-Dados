import csv
import os
from datetime import datetime

class Registro():
    
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
                    float(linha[3]),  # Converte para número
                    float(linha[4]),  # Converte para número
                    linha[5],
                    float(linha[6].rstrip('M'))  # Remove o 'M' e converte para número
                )
                self.dados_jogos.atulizar_menor_players(jogo)
                self.dados_jogos.atulizar_maior_players(jogo)
                self.dados_jogos.atulizar_maior_vendas(jogo)
                self.dados_jogos.atulizar_menor_vendas(jogo)

class DadosJogos:
    def __init__(self):
        self.menor_players = None
        self.maior_players = None
        self.maior_vendas = None
        self.menor_vendas = None

    def atulizar_menor_players(self, jogo):
        if self.menor_players is None or jogo.player_min < self.menor_players.player_min:
            self.menor_players = jogo

    def atulizar_maior_players(self, jogo):
        if self.maior_players is None or jogo.player_min > self.maior_players.player_min:
            self.maior_players = jogo

    def atulizar_maior_vendas(self, jogo):
        if self.maior_vendas is None or jogo.vendas > self.maior_vendas.vendas:
            self.maior_vendas = jogo
    
    def atulizar_menor_vendas(self, jogo):
        if self.menor_vendas is None or jogo.vendas < self.menor_vendas.vendas:
            self.menor_vendas = jogo

    def menu(self):
        print("1 - Jogo com menor número de players")
        print("2 - Jogo com maior número de players")
        print("3 - Jogo com maior número de vendas")
        print("4 - Jogo com menor número de vendas")
        print("5 - Pesquisar por data")
        print("6 - Sair")
        valor = int(input("Digite o valor: "))
        self.exibir_dados(valor)

    def exibir_dados(self, valor):
        if valor == 1:
            os.system('cls' if os.name == 'nt' else 'clear')
            print(f"Jogo com menor número de players: {self.menor_players.nome_jogo}")
            print(f"Data: {self.menor_players.data}")
            print(f"Número de players: {self.menor_players.player_min}")
            os.system('pause' if os.name == 'nt' else 'read -p "Pressione Enter para continuar..."')
            os.system('cls' if os.name == 'nt' else 'clear')
            self.menu()
        elif valor == 2:
            os.system('cls' if os.name == 'nt' else 'clear')
            print(f"Jogo com maior número de players: {self.maior_players.nome_jogo}")
            print(f"Data: {self.maior_players.data}")
            print(f"Número de players: {self.maior_players.player_min}")
            os.system('pause' if os.name == 'nt' else 'read -p "Pressione Enter para continuar..."')
            os.system('cls' if os.name == 'nt' else 'clear')
            self.menu()
        elif valor == 3:
            os.system('cls' if os.name == 'nt' else 'clear')
            print(f"Jogo com maior número de vendas: {self.maior_vendas.nome_jogo}")
            print(f"Data: {self.maior_vendas.data}")
            print(f"Número de vendas: {self.maior_vendas.vendas}M")
            os.system('pause' if os.name == 'nt' else 'read -p "Pressione Enter para continuar..."')
            os.system('cls' if os.name == 'nt' else 'clear')
            self.menu()
        elif valor == 4:
            os.system('cls' if os.name == 'nt' else 'clear')
            print(f"Jogo com menor número de vendas: {self.menor_vendas.nome_jogo}")
            print(f"Data: {self.menor_vendas.data}")
            print(f"Número de vendas: {self.menor_vendas.vendas}M")
            os.system('pause' if os.name == 'nt' else 'read -p "Pressione Enter para continuar..."')
            os.system('cls' if os.name == 'nt' else 'clear')
            self.menu()
        elif valor == 5:
            os.system('cls' if os.name == 'nt' else 'clear')
            data = input("Data:(YYYY-MM) ")
            data = datetime.strptime(data, '%Y-%m')
            self.buscar_jogo(data)
        elif valor == 6:
            os.system('cls' if os.name == 'nt' else 'clear')
            print("Saindo...")
            exit()
        else:
            os.system('pause' if os.name == 'nt' else 'read -p "Pressione Enter para continuar..."')
            os.system('cls' if os.name == 'nt' else 'clear')
            self.menu()

class Jogo:
    def __init__(self, data, nome_jogo, plataforma, player_min, player_max, variacao_preco, vendas):
        self.data = data
        self.nome_jogo = nome_jogo
        self.plataforma = plataforma
        self.player_min = player_min
        self.player_max = player_max
        self.variacao_preco = variacao_preco
        self.vendas = vendas

    def listar_jogos(self, nome_jogo):
        if self.nome_jogo == nome_jogo:
            print(f"Data: {self.data}")
            print(f"Nome do jogo: {self.nome_jogo}")
            print(f"Plataforma: {self.plataforma}")
            print(f"Número de players: {self.player_min}")
            print(f"Número de players: {self.player_max}")
            print(f"Variação de preço: {self.variacao_preco}")
            print(f"Vendas: {self.vendas}M")
        else:
            print("Jogo não encontrado")    


    def buscar_jogo(self, data):
        print(f"teste {self.data}")


def main():
    registro = Registro('tabela_jogos.csv')
    registro.ler_arquivo()
    registro.dados_jogos.menu()

if __name__ == '__main__':
    main()