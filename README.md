# Problemática

O uso de hortas verticais para produção doméstica é uma das formas encontradas por moradores em casas para conseguir ter alimentos saudáveis para a família. Perante a dificuldade de uma vida estressante, normalmente a pessoa se esquece de regar as plantas ou de adubar elas. Com o sistema proposto a pessoa conseguirá monitorar em tempo real o crescimento das plantas e receberá alertas de quando elas necessitam de cuidados especiais.

# Objetivos

Criar sistema de monitoramento de horta vertical

# Materiais

* Esp32 dev
* DHT11
* Sensor de Humidade
* Relé
* Sensor de Distância HC-SR04
* Bomba de água
* Raspberry 3


# Software

* Arduino IDE
* ScadaBR

# Funcionalidades

O sistema coletará os dados de humidade do solo, temperatura e humidade do ambiente e terá sistema de rega automático usando uma bomba controlada via temporizador. Esse temporizador será implementado no ScadaBR.


# Etapas Desenvolvimento

- [x] Ligação DHT11 ao ESP32
- [x] Leitura DHT11 ao ESP32
- [x] Ligação de relé ao ESP32
- [x] Controle de bomba de água via relé
- [ ] Ligação sensor humidade do solo ao ESP32
- [ ] Leitura sensor de humidade ao ESP32
- [ ] Instalação ScadaBR no Raspberry 3
- [ ] Desenvolvimento de supervisório no Scada BR
- [ ] Transmissão de dados aos Raspberry usando a biblioteca modbus ip
- [ ] Implementação e teste de temporizador no Raspberry
- [ ] Testes em campo
