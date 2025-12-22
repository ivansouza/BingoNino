# BingoNino 🎰

O **BingoNino** é uma máquina de bingo eletrônica baseada na plataforma Arduino, desenvolvida por Ivan Tadeu dos Santos Souza. O sistema automatiza o sorteio de pedras de bingo (75 números) e utiliza componentes visuais e sonoros para proporcionar uma experiência interativa e completa.

## 🚀 Funcionalidades

* **Sorteio Aleatório Inteligente**: O sistema utiliza uma semente de número aleatório baseada em ruído analógico para garantir que os sorteios sejam imprevisíveis.
* **Interface Visual Avançada**: Exibe o número sorteado em "números grandes" customizados (ocupando as duas linhas do LCD), facilitando a leitura.
* **Exibição de Status**: Mostra a letra correspondente do Bingo (B, I, N, G, O), o progresso do sorteio (ex: 1/75) e qual foi o número sorteado anteriormente.
* **Navegação Histórica**: Permite navegar pelos números já sorteados utilizando um Rotary Encoder.
* **Feedback Sonoro**: Possui melodias de inicialização e bips de confirmação para ações como sorteio e limites de navegação.

## 🛠️ Componentes de Hardware

Para montar o projeto, são necessários os seguintes itens:

* **Arduino** (Uno, Nano ou compatível).
* **LCD 16x2**.
* **Rotary Encoder**.
* **Buzzer** (para efeitos sonoros).
* **Botão de Pressão (Push-button)**: Para realizar o sorteio (pode ser usado o switch do próprio Rotary Encoder).

## 🔌 Pinagem e Conexões

Conecte os componentes aos pinos do Arduino conforme a configuração definida no código:

| Componente | Pinos do Arduino |
| :--- | :--- |
| **LCD 16x2 (RS, E, D4, D5, D6, D7)** | A2, A0, 12, 11, 10, 9 |
| **Buzzer** | Pino 8 |
| **Botão de Sorteio** | Pino 5 |
| **Rotary Encoder (CLK, DT)** | Pinos 2 e 3 |

## 💻 Como Usar

1.  **Arquivos**: Certifique-se de que os arquivos `bingo.ino` e `pitches.h` estejam na mesma pasta.
2.  **Upload**: Carregue o código `bingo.ino` para o seu Arduino através da IDE oficial.
3.  **Início**: Ao ligar, o sistema tocará uma melodia de boas-vindas. Pressione o botão no pino 5 para iniciar o jogo.
4.  **Sortear**: Cada pressão no botão sorteia um novo número.
5.  **Consultar**: Gire o Rotary Encoder para retroceder e verificar números que já foram sorteados anteriormente.

## 📄 Licença

Este software é distribuído sob a licença **BSD New License**.

* **Copyright (c) 2011-2015, Ivan Tadeu dos Santos Souza.**
* A redistribuição e o uso em formatos de código-fonte ou binários são permitidos, desde que as condições da licença (como a manutenção do aviso de copyright) sejam respeitadas.
