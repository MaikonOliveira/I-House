# I-House
Automação Residêncial utilizando ESP-32.

O Projeto consiste em automatizar uma residência de uma forma mobile, ou seja através de um aplicativo instalado em um smartphone, o sistema realiza a leitura dos dados de temperatura e umidade de uma sensor DHT-11 e insere em um banco de dados no firebase, assim o app é capaz de realizar a leitura em tempo real dos dados do ESP-32.

O controle dos relés tambem é realizada através de um banco de dados no firebase, onde o app realiza a alteração de parâmetros de acionamento dos relés e o ESP-32 realiza a leitura do parâmetros também pelo banco de dados.

# Placa para o ESP-32 com relés e sensor:
![image](https://user-images.githubusercontent.com/115705852/200428689-73f683b9-b971-4219-8d4d-4c9a97d580e7.png)
