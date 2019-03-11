![Imgur](https://i.imgur.com/9MGCRiz.png?1)

# Saturn
O Saturn é um sistema que permite monitorar remotamente os dados de um sonar, no caso, o HRC-SR04. É composto por um circuito eletrônico programado e um software servidor para processar os dados recebidos do sonar.

# Ferramentas necessárias
* Arduino (Wemos) ou Arduino Uno
* Módulo WiFi (ESP8266)
* Arduino IDE - https://www.arduino.cc/en/main/software
* MSYS - https://www.msys2.org/
* Git - https://git-scm.com/download
* SQLite 3 - https://www.sqlite.org/index.html

# Como compilar
* Software do arduino
    * Instale a IDE do arduino
    * Abra o arquivo ".ino" do projeto
    * Aperte Ctrl+U para compilar e carregar o programa para a placa (arduino)
* Servidor
    * Abra o shell do msys 
    * Digite os comandos a seguir: 
    * **cd servidor**
    * **./compilar <64 ou 32>**
	* **./compilar 32** irá compilar o programa para a plataforma 32 bits
	* **./compilar 64** irá compilar o programa para a plataforma 64 bits
    * **make -f Makefile_x86 run** - para executar o programa 32 bits
	* **make -f Makefile_x64 run** - para executar o programa 64 bits

# Autor
* Lucas Vieira de Jesus - <lucas.engen.cc@gmail.com>

# Plataformas suportadas
* Windows (x86 e x64)
* Arduino
