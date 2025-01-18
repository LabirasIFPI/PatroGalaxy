import socket

# Configurações do servidor
HOST = 'localhost'  # Endereço do servidor (use '0.0.0.0' para escutar em todas as interfaces)
PORT = 5000    # Porta que será usada

# Cria um socket TCP/IP
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))  # Liga o socket a um endereço e porta
    s.listen()            # Coloca o servidor em modo de escuta
    print(f"Servidor escutando em {HOST}:{PORT}")

    while True:
        try:
            conn, addr = s.accept()  # Aguarda por uma nova conexão
            with conn:
                print(f"Conectado por {addr}")
                while True:
                    data = conn.recv(1024)  # Recebe dados (até 1024 bytes)
                    if not data:
                        break
                    print(f"Mensagem recebida: {data.decode('utf-8')}")  # Imprime na tela

                    conn.sendall(b"Mensagem recebida")  # Envia confirmação
        except KeyboardInterrupt:
            break
        
    print("Servidor finalizado")
