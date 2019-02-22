# Descrição do projeto

O software é dividido em duas partes: o cliente e o servidor  
O cliente será responsável por realizar uma leitura a cada 500 ms do sonar e enviá-la via TCP
para um servidor contendo a distância lida e um timestamp. Assim que o servidor receber a informação, ele
deve armazená-la de modo que esta possa ser ordenada por horário ou pelo valor da distância lida.