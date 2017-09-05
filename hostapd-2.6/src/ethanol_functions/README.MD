Diretório de testes com netlink
===============================

Neste diretório estão diversos programas feitos para testar o acesso às estruturas de dados do netlink e nl80211.

Utilize:<br/>
1) <b>make</b>
para compilar o programa de teste

2) <b>make getmac</b>
para gerar um executável com o getmac.c

3) <b>make clean</b>
limpa os objetos e backup

<h1>O que está aqui<h1>
<ul>
<li>ap<li>
<li>channel<li>
<li>getmac<li>
<li>get_phyintf<li>
<li>nl80211_socket<li>
<li>phy_info<li>
<li>scan<li>
<li>station<li>
<li>teste<li>
<li>utils<li>
<li>wlan<li>
<li>wlan_info<li>
</ul>

<h1>Monitoramento</h1>

<b>Limitação</b>: normalmente a interface wireless não consegue transmitir quando em modo monitor e está restrita a um único canal (porem isto pode depender do driver, firmware e chipset). Ainda o dispositivo 802.11 não pode: (1) enviar quadros ACK 802.11 para cada pacote recebido, (2) rejeitar pacotes duplicados e (3) sair do modo de energia denominado 802.11 Constantly Awake Mode (CAM) se estiver no estado D0.

habilita captura como monitor<br/>
sudo tcpdump -i wlan0 -e -I -t

troca canal<br/>
sudo iw wlan0 set channel <b>xx</b><br/>
xx = canal


