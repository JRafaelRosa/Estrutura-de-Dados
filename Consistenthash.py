import json


class Node:
    def __init__(self, data):
        """
        Inicializa um nó (físico ou virtual).
        'data' é o dicionário que identifica o nó físico (ex: {"name": "ServidorA"}).
        'ang' é a posição no anel (0-359).
        'keys' armazena as chaves (como strings JSON) pelas quais este nó é responsável.
        """
        self.data = data  # dict com pelo menos "name"
        self.ang = None
        self.keys = []

    def __repr__(self):
        """
        Fornece uma representação em string do nó, útil para debugging.
        Mostra o nome, o ângulo e a quantidade de chaves que ele armazena.
        """
        return f"{self.data['name']}:{self.ang} -> {len(self.keys)} keys"


class Ring:
    def __init__(self, replicas=50):
        """
        Inicializa o anel de hash consistente.
        'replicas' é o número de nós virtuais a criar para cada nó físico,
        para garantir uma distribuição de chaves mais uniforme.
        'ring' é a lista que armazena todos os nós (físicos e virtuais),
        ordenada por ângulo.
        """
        self.ring = []
        self.replicas = replicas

    def _hash_node(self, node_data):
        """
        Calcula o hash de um nó (físico ou virtual) para uma posição no anel.
        - Se 'node_data' for um dict (nó físico), ele o converte para uma string JSON
          ordenada para garantir um hash consistente.
        - Se 'node_data' for uma string (nó virtual, ex: "ServidorA:1"),
          ele faz o hash da string.
        O resultado é mapeado para um "ângulo" entre 0 e 359.
        """
        if isinstance(node_data, dict):
            return abs(hash(json.dumps(node_data, sort_keys=True))) % 360
        return abs(hash(node_data)) % 360

    def _hash_key(self, key):
        """
        Calcula o hash de uma chave para uma posição no anel.
        Para garantir que a chave seja "hashable" (possível de colocar em sets)
        e consistente, esta função sempre faz o hash da *representação em string*
        da chave.
        - Se 'key' for um dict, ele o converte para uma string JSON canônica.
        - Se 'key' já for uma string, ele a utiliza.
        O resultado é mapeado para um "ângulo" entre 0 e 359.
        """
        if isinstance(key, dict):
            # Converte o dict para sua string canônica
            key_str = json.dumps(key, sort_keys=True)
        else:
            # Assume que 'key' JÁ É a string canônica
            key_str = key

        return abs(hash(key_str)) % 360

    def add(self, data):
        """
        Adiciona um novo nó físico ao anel, junto com suas réplicas virtuais.
        Esta função implementa o "rebalanceamento mínimo".
        1. Cria o nó físico e todas as suas réplicas virtuais.
        2. Adiciona todos esses nós ao anel e reordena o anel.
        3. Identifica quais chaves, que antes pertenciam a outros nós, agora
           devem ser movidas para este novo nó.
        4. Chama '_rebalance_keys' passando *apenas* a lista de chaves a serem
           movidas, minimizando a disrupção no anel.
        """
        # --- ETAPA 1: Preparar e adicionar todos os nós (físico + réplicas) ---
        node = Node(data)
        node.ang = self._hash_node(node.data)

        new_nodes_added = [node]  # Lista para rastrear quem exatamente adicionamos
        for i in range(self.replicas):
            key = f"{data['name']}:{i}"
            ang = self._hash_node(key)
            replica = Node(data)
            replica.ang = ang
            new_nodes_added.append(replica)

        self.ring.extend(new_nodes_added)
        self.ring.sort(key=lambda n: n.ang)

        # --- ETAPA 2: Encontrar as chaves que precisam ser movidas ---
        keys_to_rebalance = set()  # Usar um set para evitar checagens duplicadas

        for new_node in new_nodes_added:
            idx = self.ring.index(new_node)
            successor = self.ring[(idx + 1) % len(self.ring)]

            if successor.data['name'] == data['name']:
                continue

            physical_successor = self.get_physical_node(successor.data)
            if not physical_successor: continue

            predecessor_idx = (idx - 1 + len(self.ring)) % len(self.ring)
            predecessor = self.ring[predecessor_idx]
            predecessor_ang = predecessor.ang
            new_node_ang = new_node.ang

            # Itera pelas chaves (strings) do nó físico do sucessor
            for key_str in physical_successor.keys:
                key_ang = self._hash_key(key_str)

                # Caso 1: O anel "deu a volta" (ex: pred=350, new=10)
                if predecessor_ang > new_node_ang:
                    if key_ang > predecessor_ang or key_ang <= new_node_ang:
                        keys_to_rebalance.add(key_str)
                # Caso 2: Range normal (ex: pred=100, new=120)
                else:
                    if key_ang > predecessor_ang and key_ang <= new_node_ang:
                        keys_to_rebalance.add(key_str)

        # --- ETAPA 3: Chamar o rebalanceamento MÍNIMO ---
        if keys_to_rebalance:
            self._rebalance_keys(list(keys_to_rebalance))

    def remove(self, data):
        """
        Remove um nó físico e todas as suas réplicas virtuais do anel.
        Esta função implementa o "rebalanceamento mínimo".
        1. Coleta todas as chaves (strings) que estavam armazenadas no nó físico
           que está sendo removido.
        2. Remove todos os nós (físico e virtuais) associados a 'data' do anel.
        3. Chama '_rebalance_keys' passando *apenas* a lista de chaves que ficaram
           "órfãs", para que sejam redistribuídas aos seus novos responsáveis.
        """
        removed_keys = []
        # Encontra o nó físico para coletar as chaves
        physical_node = self.get_physical_node(data)
        if physical_node:
            removed_keys = physical_node.keys

        # Remove todos os nós (físicos e réplicas) do anel
        self.ring = [n for n in self.ring if n.data['name'] != data['name']]

        # Reatribui apenas as chaves órfãs
        if removed_keys:
            self._rebalance_keys(removed_keys)

    def _rebalance_keys(self, keys_to_assign=None):
        """
        Motor central de atribuição e reatribuição de chaves.
        Opera em dois modos:

        1. "Full Rebalance" (keys_to_assign is None):
           Usado na inicialização do 'add'. Coleta *todas* as chaves de *todos*
           os nós, limpa todos os nós, e reatribui tudo do zero.

        2. "Minimum Rebalance" (keys_to_assign é uma lista):
           Usado por 'add', 'remove' e 'assign_keys'.
           Recebe uma lista de chaves para mover.
           - Converte a lista para um `set` de strings JSON para performance.
           - Remove essas chaves (strings) de qualquer nó físico que as possua.
           - Reatribui cada chave da lista ao seu novo nó correto.

        Sempre armazena as chaves como *strings JSON* em 'node.keys'.
        """
        keys_to_reassign_strings = []  # Lista final de strings a reatribuir

        if keys_to_assign is None:
            # Bloco "Full Rebalance" (usado pelo 'add' antigo, agora menos comum)
            for n in self.ring:
                # n.keys já contém strings (das atribuições anteriores)
                keys_to_reassign_strings.extend(n.keys)
                n.keys.clear()
        else:
            # Bloco "Minimum Rebalance" (usado por add, remove, assign_keys)

            # 1. Construir o set de remoção com STRINGS
            keys_to_remove_set = set()
            for key_item in keys_to_assign:
                if isinstance(key_item, dict):
                    keys_to_remove_set.add(json.dumps(key_item, sort_keys=True))
                else:
                    keys_to_remove_set.add(key_item)  # Já é uma string

            # 2. Popular a lista de reatribuição (são as mesmas chaves)
            keys_to_reassign_strings = list(keys_to_remove_set)

            # 3. Remover essas chaves dos nós físicos
            for n in self.ring:
                # Otimização: só precisamos limpar chaves dos nós físicos
                if n.ang != self._hash_node(n.data):
                    continue

                # n.keys contém strings, keys_to_remove_set contém strings
                if n.keys:
                    n.keys = [k_str for k_str in n.keys if k_str not in keys_to_remove_set]

        # --- Bloco de Reatribuição (Comum a ambos os casos) ---

        # Itera sobre a lista de STRINGS
        for key_str in keys_to_reassign_strings:
            # Encontra o nó usando o hash da STRING
            node = self.get_node(key_str)
            if node:
                # Armazena a STRING no nó
                node.keys.append(key_str)

    def get_node(self, key):
        """
        Encontra o nó FÍSICO responsável por uma determinada chave.
        1. Calcula o hash da chave para encontrar seu ângulo no anel.
        2. Percorre o anel ordenado até encontrar o primeiro nó (físico ou virtual)
           cujo ângulo é maior ou igual ao ângulo da chave.
        3. Se "der a volta" no anel, retorna o primeiro nó (posição 0).
        4. Uma vez que o nó (potencialmente virtual) é encontrado, usa
           'get_physical_node' para retornar o nó físico real associado a ele,
           que é onde a chave será de fato armazenada.
        """
        if not self.ring:
            return None

        # Garante que estamos hasheando a string da chave
        key_ang = self._hash_key(key)

        for node in self.ring:
            if node.ang >= key_ang:
                return self.get_physical_node(node.data)

        # Se "deu a volta", retorna o primeiro nó do anel
        return self.get_physical_node(self.ring[0].data)

    def get_physical_node(self, data):
        """
        Encontra e retorna o objeto do nó FÍSICO original com base em seus 'data'.
        As réplicas virtuais compartilham o mesmo 'data', mas têm ângulos diferentes.
        Esta função localiza especificamente o nó "mestre" (físico), cuja
        identidade é que seu 'ang' é o hash do seu próprio 'data', e não de uma
        string de réplica (ex: "ServidorA:1").
        """
        target_ang = self._hash_node(data)
        for node in self.ring:
            if node.data == data and node.ang == target_ang:
                return node
        return None  # Pode acontecer se o nó foi removido

    def assign_keys(self, keys):
        """
        Atribui uma lista inicial de chaves (dicts) ao anel.
        Esta é uma função de conveniência que simplesmente chama
        '_rebalance_keys' no modo "Minimum Rebalance".
        Todas as chaves da lista serão removidas de onde estiverem (se estiverem)
        e atribuídas aos seus nós corretos.
        """
        self._rebalance_keys(keys)

    def print_load(self):
        """
        Imprime a carga atual (número de chaves) de cada nó FÍSICO no anel.
        1. Encontra todos os servidores únicos ('data') presentes no anel.
        2. Para cada servidor, usa 'get_physical_node' para buscar o objeto
           do nó físico correspondente.
        3. Imprime o nome do servidor e o 'len(physical_node.keys)', que é a
           carga real de chaves armazenadas.
        """
        unique_data_map = {}
        for n in self.ring:
            data_str = json.dumps(n.data, sort_keys=True)
            if data_str not in unique_data_map:
                unique_data_map[data_str] = n.data

        # Itera sobre os 'data' únicos, ordenados pelo nome do servidor
        sorted_data = sorted(unique_data_map.values(), key=lambda item: item['name'])

        for data_dict in sorted_data:
            # Para cada servidor, busca o SEU nó físico
            physical_node = self.get_physical_node(data_dict)

            # Imprime a carga APENAS do nó físico
            if physical_node:
                print(f"Nó {physical_node.data['name']}: {len(physical_node.keys)} chaves")
                # Descomente abaixo para ver a lista de chaves (pode ser longa)
                # print(f"  -> {physical_node.keys}")


def read_json_file(file_path):
    """
    Função utilitária para ler um arquivo JSON e retornar seu conteúdo
    como um objeto Python (lista ou dicionário).
    """
    with open(file_path, "r", encoding="utf-8") as f:
        return json.load(f)


def main():
    """
    Função principal que executa a simulação.
    1. Lê os arquivos 'nodes.json' e 'keys.json'.
    2. Inicializa o Ring com 50 réplicas.
    3. Adiciona os nós iniciais (ServidorA, B, C).
    4. Atribui a carga inicial de chaves e imprime a distribuição.
    5. Simula mudanças na topologia:
       - Adiciona "ServidorD" (demonstrando rebalanceamento mínimo de adição).
       - Remove "ServidorB" (demonstrando rebalanceamento mínimo de remoção).
    6. Imprime a distribuição final para mostrar como as chaves foram movidas.
    """
    nodes = read_json_file("nodes.json")
    keys = read_json_file("keys.json")

    ring = Ring(replicas=50)

    # Adicionar nós causa rebalanceamento mínimo (de 0 chaves)
    for node in nodes:
        print(f"Adicionando {node['name']}...")
        ring.add(node)

    # Atribui a carga inicial de chaves
    print("\nAtribuindo chaves...")
    ring.assign_keys(keys)
    print("Distribuição inicial:")
    ring.print_load()
    print("\n---\n")

    # Simula mudanças na topologia
    print("Adicionando ServidorD...")
    ring.add({"name": "ServidorD"})
    print("\nRemovendo ServidorB...")
    ring.remove({"name": "ServidorB"})

    print("\nDistribuição após alterações:")
    ring.print_load()
    print("\n---\n")


if __name__ == "__main__":
    main()