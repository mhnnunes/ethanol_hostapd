COMUNICAÇÃO PARA ETHANOL
========================

Para garantir a comunicação entre o controlador ethanol e seus agentes precisamos desenvolver um canal de comunicação segura entre os dois equipamentos. Será necessário criar uma aplicação servidor e uma aplicação cliente para cada um dos equipamentos. No controlador, as aplicações deverão ser desenvolvidas em Python para que sejam compatíveis com o controlador Pox (openflow). Já no lado dos clientes, as aplicações deverão ser em C ou C++. Todos os programas em C/C++ deverão ser compilados e montados por arquivo make apropriado, todas as dependências deverão ser verificadas por arquivo configure.

Utilizaremos para a comunicação o protocolo XML-RPC que é um protocolo de chamada de procedimento remoto (RPC) que utiliza XML para codificar a comunicação e como mecanismo de transporte é utilizado o protocolo HTTP (ou HTTPS).

O servidor deve ser multithreaded para suportar diversas conexões simultâneas.

As aplicações cliente e servidor deverão evoluir para que a conexão seja inicialmente aberta e permaneça aberta até que seja chamado procedimento específico para fechar a conexão no cliente (ou ocorra timeout no servidor). Deverá ser previsto procedimento para tratamento de erro no caso da queda da conexão.

Deverá ser gerado um arquivo de log (em formato txt) para todas as requisições e respostas em ambas as aplicações. Deve ser prevista a possibilidade de remover o log em tempo de compilação e mediante configuração