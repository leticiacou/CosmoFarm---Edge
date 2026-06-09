# CosmoFarm---Edge

# Descrição:
Repositório do projeto arduino do sistema CosmoFarm
CosmoFarm é um sistema operacional de agricultura espacial autônoma que controla toda a produção de alimentos dentro de naves ou bases em Marte — sem intervenção humana constante. Ele gerencia irrigação, iluminação, temperatura e nutrientes via sensores, algoritmos e atuadores, garantindo colheitas contínuas para tripulações em missões de longa duração. O mesmo sistema pode ser aplicado na Terra para cultivar alimentos em regiões áridas ou urbanas, usando hidroponia com até 90% menos água que a agricultura tradicional.

# Objetivo:
Garantir a sobrevivência de tripulações em missões espaciais de longa duração através da produção autônoma e contínua de alimentos frescos — eliminando a depen…Garantir a sobrevivência de tripulações em missões espaciais de longa duração através da produção autônoma e contínua de alimentos frescos — eliminando a dependência de reabastecimento externo e resolvendo os problemas de custo, validade e nutrição que tornam missões a Marte inviáveis hoje.

# Componentes:
1× DHT22 — temperatura e umidade do ar
1× LDR (fotoresistor) — intensidade luminosa
1× Arduino Mega 2560
2× Módulo relay — um para a bomba de irrigação, um para o ventilador
2× LED vermelho — grow espectro vermelho (pino 4) + LED de alerta (pino 7)
1× LED azul — grow espectro azul (pino 5)
1× LED verde — indicador de status OK (pino 6)
1× LCD 16×2 com módulo I2C
4× Resistor 220Ω — um em série com cada LED
Cabos jumper (vermelho, preto, verde, amarelo, laranja, roxo, azul)
Protoboard (recomendada para organizar as conexões)

# Explicação do funcionamento:
O CosmoFarm é uma estufa autônoma controlada por Arduino. Três sensores monitoram o ambiente continuamente: o DHT22 lê temperatura e umidade, e o LDR mede a luminosidade.
Com base nessas leituras, o sistema age sozinho — se a temperatura passa de 26°C, liga o ventilador; se a umidade cai abaixo de 55%, aciona a bomba de irrigação; se a luz for insuficiente, acende os LEDs grow. Se tudo estiver dentro dos parâmetros, o LED verde fica aceso indicando que está OK. Qualquer desvio acende o LED de alerta.
O LCD exibe em tempo real temperatura, umidade, dia do ciclo, biomassa estimada e consumo de água — alternando entre três telas a cada 3 segundos.
Por baixo, dois modelos matemáticos rodam em paralelo: um exponencial que prevê o crescimento das plantas dia a dia, e um linear que controla o consumo hídrico. A cada 10 segundos de simulação passa um dia de cultivo. Quando chega ao dia 30, o sistema detecta a colheita e reinicia o ciclo automaticamente.

# Estrutura do circuito e instruções de execução:
O Arduino Mega é o centro de tudo. Os componentes se dividem em três grupos ligados a ele:
Sensores de entrada — o DHT22 conecta no pino digital 2 (dados), com VCC em 5V e GND. O LDR conecta no pino analógico A0, também com VCC em 5V e GND.
Atuadores de saída — os dois módulos relay conectam nos pinos 8 e 9, cada um com VCC em 5V e GND próprio. Os quatro LEDs conectam nos pinos 4, 5, 6 e 7, cada um com um resistor de 220Ω em série antes do anodo, e o catodo no GND.
Display — o LCD 16×2 com módulo I2C conecta via SDA no pino 20 e SCL no pino 21, com VCC em 5V e GND.
Instruções de execução no Wokwi

1 - Acesse wokwi.com e crie um novo projeto com Arduino Mega
2 - No arquivo diagram.json, cole o JSON do circuito
3 - No arquivo sketch.ino, cole o código Arduino
4 - Clique em "Add Library" e instale DHT sensor library e LiquidCrystal I2C
5 - Clique em ▶ para iniciar a simulação

# Integrantes:
LETÍCIA BRANDÃO COUTO - 569516
BEATRIZ CAMPOS MENDONÇA DE ALBUQUERQUE - 572055
NICOLE FIGUEIREDO COUTO - 569191 