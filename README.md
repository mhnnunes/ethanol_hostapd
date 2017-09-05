# Agente Ethanol #


Desenvolvimento de agente ethanol para ambientes linux utilizando como base o hostapd e as chamadas para nl80211 (como feitas pelo iw). O agente ethanol (em hostapd) precisa que o iw gerado neste repositoria seja copia para o mesmo diretorio onde o hostapd será instalado. Também é necessário que o arquivo my_cert.pem (em messaging) esteja no mesmo diretório do hostapd.
Para configurar o ethanol (em hostapd) é necessário a instalação do arquivo /etc/ethanol.ini. Um exemplo deste arquivo pode ser visto dentro do diretório src/ini. A opção mais importante é *enabled* que controla se as características do ethanol estarão ativas ou não no hostapd.

O Ethanol utiliza a biblioteca **libnl** que foi deprecated. Ela só existe até a versão 14.04 do Ubuntu.
Portanto se você quer utilizar o Ethanol sem ter que fazer suas próprias modificações para que ele funcione, você deve utilizar o Ubuntu LTS 14.04.5 (última verificação em 26/06/2017).

## Instalação ##

### Clone do repositório com git ###

Para baixar este repositorio, voce precisa ter o git instalado. Para isto use o comando abaixo para instalar. Sugerimos a instalação do gitk para uma interface gráfica (segunda linha abaixo):

```bash
sudo apt-get install git
sudo apt-get install gitk git-gui
```

Com o git instalado, vá para o diretório desejado (por exemplo, /home/winet) e dê o comando

```bash
git clone ssh://<nome do usuario>@disk.winet.dcc.ufmg.br/volume1/git/ethanol
git submodule update --init --recursive
```


### Heap ###


A implementação do HNR utiliza um heap em python.
Aproveitamos um implementação feita por h3dema em python. Esta implementação também é um submodulo do git.
O git baixa, portanto, diretamente da internet a versão mais atual da classe desenvolvida.
Para atualizar o diretório, usar

```bash
cd ethanol
git submodule update --init --recursive
```

## Documentos ##

Na raiz do repositório encontramos o diretório **docs** que contem os arquivos com a documentação com as orientações para desenvolvimento do Ethanol, bem como outras informações para instalação e preparação do ambiente de desenvolvimento.

## Ethanol's hostapd ##

Na raiz do repositório encontramos o diretório **hostapd-2.6**, que é um fork do diretório do hostapd.
Este diretório não é atualizado a partir da internet.
As modificações nele são locais para o projeto do Ethanol.
Os arquivos fonte são utilizado para fazer as adaptações em
ieee802_11_mgmt() em hostapd-2.6/src/ap/ieee802_11.c onde encontramos o tratamento de associação, autenticação, reassociação, desassociação e desautenticação.

Para compilar a versão do hostapd com ethanol, depois de fazer o download do repositório, faça

```bash
cd hostapd-2.6/hostapd

make clean
make ethanol
```

### Se você quiser um hostapd não modificado ###

Você pode utilizar de duas formas:

1- baixando o arquivo compactado:

Baixar o arquivo original a partir do link http://w1.fi/releases/hostapd-2.*.tar.gz, onde * é a versão. Estamos utilizando a versão 2.6 que foi descompactada e copiada para o diretório agente/hostapd-2.6.

Dentro do diretório hostapd-2.6/hostapd, devemos executar os comandos:

```bash
cd hostapd-2.6/hostapd
cp defconfig .config
```

2 - Você pode fazer um clone da última versão estável usando o github.

```bash
git clone git://w1.fi/srv/git/hostap.git
cd hostap/hostapd
```

### Instalar as dependências ###

Preparando o ambiente para compilação e execução do ethanol


```bash
sudo apt-get install -y libnl-dev libnl1
sudo apt-get install -y libssl-dev
sudo apt-get install -y libiw-dev
sudo apt-get install -y curl libcurl3 libcurl3-dev
sudo apt-get install -y wireless-tools
make
```

No ubuntu como o build-essential é pré instalado, basta realizar estas orientações para compilar o hostapd. A instalação é feita com **make install**.

No CentOS é necessário criar alguns links simbólicos para o libssl funcionar.

```bash
cd /lib/x86_64-linux-gnu
sudo ln -s libssl.so.1.0.0 libssl.so.10
sudo ln -s libcrypto.so.1.0.0 libcrypto.so.10
```

### Compilar ###

Para gerar uma versao do hostapd compativel com o Ethanol

cd hostapd/hostapd
make clean
make ethanol

serao gerados dois executaveis: hostapd e hostapd_cli
o primeiro eh o programa que contem o agente ethanol para redes sem fio


### IW ###

Utilizamos uma versão adaptada do iw-4.9.
Os fontes modificados estão no diretório **iw-4.9**.

Muitas das operações usando **nl80211** são semelhantes àquelas encontradas no programa *iw*, que pode ser usado para identificação das chamadas à **nl80211**. É a melhor documentação encontrada sobre o assunto. Sobre o *iw*, referenciar o link https://wireless.wiki.kernel.org/en/users/documentation/iw.

Preparando o ambiente

```bash
cd iw-4.9
sudo apt-get install libnl-dev libnl1

make clean
make ethanol
```

A instalação do iw (a partir dos fontes) pode ser feita com **make install**.
Para download usar **apt-get install iw**, contudo normalmente a versão baixada com apt-get é mais antiga.
Os tarballs do iw estão disponíveis em http://kernel.org/pub/software/network/iw/.



## Controlador OpenFlow e Ethanol ##

### OpenFlow ###

O Ethanol utiliza o controlador OpenFlow, baseado em [POX](https://github.com/noxrepo/pox), para controle dos elementos de rede cabeada (switches OpenFlow).
Neste projeto, o POX é um submodulo do git, que ao ser baixado cria um diretório **pox** dentro da árvore de diretórios do repositório.
O git baixa diretamente da internet a versao do POX CARP. 
Para atualizar o diretorio, usar o comando **git submodules update**.

### python ###

Arquivos do controlador ethanol em python. Deve ser utilizado em conjunto com o POX.
Eh necessario que a biblioteca CONSTRUCT tenha sido instalada para o servidor Ethanol funcionar.
Veja mais detalhas de instalaçao do CONSTRUCT no arquivo python/ssl_message/msg_core.py

Existe um arquivo de documentacao dos programas python denominado api.pdf no diretorio python/doc.

Controlador
-----------
Para rodar o controlador simplificado, use

```bash
./run.ethanol
```

Este script ira chamar o pox, carregando o modulo do l2_learning e do ethanol.server.


# Estatisticas #

Dados estatisticos de desenvolvimento do Ethanol.
Usamos gitstats, que pode ser obtido em https://github.com/hoxu/gitstats.git.

Veja no diretório mais informações.
