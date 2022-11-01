# I-House
Automação Residêncial utilizando ESP-32.

O Projeto consiste em automatizar uma residência de uma forma mobile, ou seja através de um aplicativo instalado em um smartphone, o sistema realiza a leitura dos dados de temperatura e umidade de uma sensor DHT-11 e insere em um banco de dados no firebase, assim o app é capaz de realizar a leitura em temp real dos dados do ESP-32.

O controle dos relés tambem é realizada através de um banco de dados no firebase, onde o app realiza a alteração de parâmetros de acionamento dos relés e o ESP-32 realiza a leitura do parâmetros também pelo banco de dados.
