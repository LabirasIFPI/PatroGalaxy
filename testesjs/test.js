const net = require('net');

// Configurações do servidor
const HOST = '0.0.0.0'; // Escuta em todas as interfaces de rede disponíveis
const PORT = 5000;      // Porta que o servidor irá escutar (mesma usada no seu código C)

// Cria um servidor TCP
const server = net.createServer((socket) => {
    console.log('Cliente conectado:', socket.remoteAddress, socket.remotePort);

    // Função de callback para quando o servidor recebe dados
    socket.on('data', (data) => {
        console.log('Mensagem recebida:', data.toString());
        
        // Envia uma resposta ao cliente
        socket.write('Mensagem recebida pelo servidor');
    });

    // Função de callback para quando o cliente se desconecta
    socket.on('end', () => {
        console.log('Cliente desconectado:', socket.remoteAddress, socket.remotePort);
    });

    // Função de callback para erros
    socket.on('error', (err) => {
        console.error('Erro com o cliente:', err.message);
    });
});

// Inicia o servidor e escuta por coneções
server.listen(PORT, HOST, () => {
    console.log(`Servidor TCP escutando em ${HOST}:${PORT}`);
});
