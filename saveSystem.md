# Documentação de Salvamento e Carregamento de Dados no PICO W

## Introdução

Essa documentação tem como objetivo guiar o desenvolvimento de funções para salvar e carregar dados do usuário utilizando o Raspberry Pi Pico W. O foco será na implementação em linguagem C, abordando os conceitos e funções necessárias para manipular a memória flash da placa.

## Criação de Bibliotecas

Nós queremos criar um arquivo de biblioteca para gerenciar o sistema de salvamento. Para isso, vamos criar dois arquivos: `saveSystem.h` e `saveSystem.c`.

### saveSystem.h

Este é o arquivo de cabeçalho. Ele contém as declarações das funções, macros, e tipos de dados que serão utilizados no `saveSystem.c`. Em outras palavras, ele define a interface pública do sistema de salvamento, permitindo que outras partes do programa saibam como interagir com ele.
Na prática, o arquivo irá declarar as funções e definir as constantes.

```C
#ifndef SAVESYSTEM_H
#define SAVESYSTEM_H

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

#define FLASH_TARGET_OFFSET (256 * 1024)

void saveProgress(uint16_t *progressString);
void loadProgress(uint16_t *buffer, size_t tamanho);
void clearSaveData();

#endif // SAVESYSTEM_H
```

### saveSystem.c

Este é o arquivo de implementação. Ele contém as definições das funções declaradas no `saveSystem.h`. Aqui é onde a lógica real do sistema de salvamento é implementada. Este arquivo faz o trabalho pesado de salvar e carregar dados, enquanto o arquivo de cabeçalho apenas expõe as funções necessárias para realizar essas operações.

```C
#include "saveSystem.h"
#include <string.h>

void saveProgress(uint16_t *buffer) {
    uint8_t buffer_completo[FLASH_PAGE_SIZE];
    memcpy(buffer_completo, buffer, tamanho);

    // Preencher o restante da página
    memset(buffer_completo + tamanho, 0x00, FLASH_PAGE_SIZE - tamanho);

    uint32_t interruptions = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, buffer_completo, FLASH_PAGE_SIZE);
    restore_interrupts(interruptions);
}

void loadProgress(uint16_t *buffer, size_t tamanho) {
    uint8_t *address = (uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
    memcpy(buffer, address, tamanho);
}

void clearSaveData() {
    uint32_t interruptions = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, 0x00, FLASH_PAGE_SIZE);
    restore_interrupts(interruptions);
}
```

## Atualizando o CMakeLists.txt

Adicione seu novo arquivo de código-fonte `saveSystem.c` no seu `CMakeLists.txt`:

```diff
player.c
background.c
utils.c
asteroids.c
+saveSystem.c
)
```

Lembrando que temos também que linkar as demais bibliotecas de hardware padrão utilizadas:

```cmake
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_flash hardware_sync)
```

No caso são bibliotecas relevantes para o seu projeto, como `pico_stdlib`, `hardware_flash`, e `hardware_sync`. Isso vai garantir que todas as funcionalidades utilizadas no código estejam disponíveis durante a compilação.

## Funções Auxiliares

### Encapsulando Dados no Buffer

Para encapsular dados, como `level` e `highScore`, em um buffer, antes de salvar:

```C
void encapsular_dados(uint8_t level, uint16_t highScore, uint8_t *buffer) {
    buffer[0] = level;
    buffer[1] = (highScore >> 8) & 0xFF;
    buffer[2] = highScore & 0xFF;
}
```

Como podemos observar, aplicamos duas operações especiais: **bitwise** e **bitshift**. Essas operações são muito importantes para obtermos bytes específicos que compõem cada informação. A operação de bitshift (`>>`) desloca os bits para a direita, enquanto a operação bitwise (`&`) é usada para mascarar bits específicos. Isso nos permite extrair e armazenar partes específicas dos dados no buffer.

Isso será explicado em detalhes logo abaixo.

### Recuperando Dados do Buffer

Para recuperar dados a partir de um buffer após o carregamento:

```C

// Função para recuperar o level a partir do buffer
uint16_t recuperar_level(uint8_t *buffer) {
    // O level está armazenado no primeiro byte do buffer
    return buffer[0];
}

// Função para recuperar o highScore a partir do buffer
uint16_t recuperar_highScore(uint8_t *buffer) {
    // Combina os bytes do buffer para formar o highScore
    return (buffer[1] << 8) | buffer[2];
}
```

### Integrando no main.c

No código principal, é importante lembrar de primeiramente adicionar o cabeçalho `saveSystem.h`.
Segue um exemplo de como ficaria a implementação da biblioteca num código:

```C
#include "saveSystem.h"

int main()
{
    uint8_t level = 5;
    uint16_t highScore = 12345;
    uint8_t buffer[FLASH_PAGE_SIZE];

    // Encapsular dados no buffer
    encapsular_dados(level, highScore, buffer);

    // Salvar o buffer na memória flash
    saveProgress((uint16_t *)buffer);

    // Criar buffer temporário para carregar os dados
    uint8_t buffer_carregado[FLASH_PAGE_SIZE];

    // Carregar o buffer da memória flash
    loadProgress((uint16_t *)buffer_carregado, FLASH_PAGE_SIZE);

    // Recuperar os dados do buffer carregado
    uint8_t level_carregado = recuperar_level(buffer_carregado);
    uint16_t highScore_carregado = recuperar_highScore(buffer_carregado);

    // Exibir os dados carregados
    printf("Level carregado: %u\n", level_carregado);
    printf("HighScore carregado: %u\n", highScore_carregado);

    return 0;
}
```

## Explicações Detalhadas:

### Bitshift (>>)

Desloca os bits de um valor para a direita.

Exemplo: `highScore >> 8` move os bits 8 posições para a direita, obtendo o byte mais significativo.

O bitshift é crucial em operações de leitura e gravação, principalmente para manipular dados armazenados em múltiplos bytes.

- Deslocamento para a direita (>>): Permite extrair bytes individuais de um dado maior. Por exemplo, ao salvar um uint16_t (16 bits) em dois bytes separados, highScore >> 8 extrai o byte mais significativo.

- Deslocamento para a esquerda (<<): Reverte o processo durante a leitura, combinando bytes separados em um dado maior.

### Bitwise AND (&)

Mantém apenas os bits especificados.

Exemplo: `0xFF` mantém apenas os 8 bits menos significativos.

O operador bitwise AND é essencial para isolar bits específicos de um dado.

- Isolamento de bytes: Utilizado para garantir que apenas os bits desejados sejam mantidos. Por exemplo, highScore & 0xFF mantém apenas os 8 bits menos significativos, removendo os bits superiores que não são necessários.

- Manipulação precisa de dados: Ajuda a controlar e acessar diretamente partes específicas de um dado, fundamental para leitura e gravação precisa.

Esses conceitos permitem encapsular e recuperar dados de maneira eficiente e segura, garantindo a integridade e correta interpretação dos dados ao salvar e carregar informações na memória flash.

### Bitwise OR (|)

Combina os bits de dois valores.

Exemplo: (buffer[0] << 8) | buffer[1] combina dois bytes para formar um valor uint16_t.

O operador Bitwise OR (|) é fundamental para combinar bits individuais ou grupos de bits em uma operação de leitura ou gravação de dados, especialmente quando os dados são armazenados ou recuperados de múltiplos bytes.

- **Combinação de Bytes**: No contexto de leitura, o Bitwise OR é utilizado para combinar bytes separados em um valor completo. Por exemplo, ao ler dados armazenados em dois bytes distintos, `(buffer[0] << 8) | buffer[1]` combina esses bytes para formar o valor original.

```C
uint16_t highscore = (buffer[0] << 8) | buffer[1];
```

- **Manipulação de Bits**: Permite configurar ou ajustar bits específicos em uma posição desejada sem alterar os outros bits.

```C
uint16_t flags = 0;
flags |= (1 << 3); // Ajusta o bit 3 sem alterar os outros bits.
```

- **Reconstrução de Dados**: Ajuda a reconstituir dados complexos a partir de suas partes mais simples. Essa técnica é essencial quando se trabalha com valores maiores que 8 bits, permitindo a reconstrução correta de valores como uint16_t ou uint32_t a partir de seus componentes armazenados separadamente.

### FLASH_SECTOR_SIZE

A constante `FLASH_SECTOR_SIZE` define o tamanho de um setor na memória flash. Em sistemas embarcados, a memória flash é organizada em setores, que são as menores unidades de memória que podem ser apagadas de uma só vez. Isso significa que, para modificar qualquer dado dentro de um setor, todo o setor deve ser apagado e reescrito. O tamanho do setor pode variar dependendo do dispositivo, mas é uma característica crucial para operações de salvamento e carregamento de dados, pois influencia diretamente a eficiência e a complexidade dessas operações.

### FLASH_PAGE_SIZE

`FLASH_PAGE_SIZE` define o tamanho de uma página de memória flash. Uma página é a menor unidade de memória que pode ser escrita de uma vez só. Em sistemas embarcados, é comum que a memória flash seja organizada em páginas de 256 bytes. Durante operações de gravação, é necessário preencher a página inteira, mesmo que a quantidade de dados seja menor que o tamanho da página.

### FLASH_TARGET_OFFSET

O `FLASH_TARGET_OFFSET` é um endereço na memória flash onde os dados serão armazenados. Esse offset é escolhido para garantir que os dados do usuário não sobrescrevam áreas importantes, como o bootloader ou o código do programa. No Raspberry Pi Pico W, esse valor pode ser definido como 256 \* 1024 (256 KB) para garantir segurança e eficiência.

### Interrupções ao Salvar

Para garantir que as operações de gravação na memória flash ocorram sem interrupções, é necessário desativar as interrupções durante o processo. Isso é feito usando as funções `save_and_disable_interrupts()` e `restore_interrupts(interruptions)`. A função `save_and_disable_interrupts()` desativa todas as interrupções e salva o estado anterior das interrupções em uma variável. Após completar a gravação, a função `restore_interrupts(interruptions)` restaura o estado anterior das interrupções.

## Considerações Finais

Gerenciar a memória em sistemas embarcados é bastante difícil e crítico, já que cada bit e byte armazenado importa. Isso porque o espaço é limitado e as operações de leitura e gravação precisam ser executadas com precisão. Um pequeno erro pode levar à corrupção de dados ou ao mau funcionamento do sistema.

Por isso acaba sendo muito importante adotar práticas rigorosas e compreender 100% os conceitos de manipulação de bits e bytes.

### Preenchimento de Páginas:

- **Apagar Setores**: Em unidades de memória flash, os setores são apagados em blocos de 4096 bytes. Este é o menor tamanho de bloco que pode ser apagado de uma vez. Quando você precisar modificar qualquer dado dentro de um setor, todo o setor deve ser apagado primeiro. Essa operação é essencial para preparar a memória para novas gravações.

- **Escrever Páginas**: As páginas de memória flash geralmente têm 256 bytes. Elas são a menor unidade de gravação. Isso significa que, ao escrever dados na memória flash, é necessário preencher uma página inteira. Se os dados que você deseja gravar não ocuparem toda a página, você ainda precisa gravar 256 bytes, preenchendo o restante da página com valores padronizados (por exemplo, zeros).

- **Preencher com Zeros**: Utilize a função memset para preencher o restante da página com zeros se os dados não ocuparem toda a página. Isso garante que toda a página esteja completamente preenchida, evitando a presença de dados residuais ou lixo que poderiam corromper as operações de leitura futuras.

```C
uint8_t buffer_completo[FLASH_PAGE_SIZE];
memcpy(buffer_completo, dados, tamanho_dados);
memset(buffer_completo + tamanho_dados, 0x00, FLASH_PAGE_SIZE - tamanho_dados);  // Preenche com zeros
```

Lembrando que cada bit conta, por isso todo esse cuidado com a precisão em cada operação. E isso que faz a diferença de um sistema robusto para um sistema frágil e ruim. Valeuzão!
