# Ugradbeni Sustavi - Prekidi

## 1. Opis zadatka
Cilj zadatka je implementacija prekida u ugradbenim sustavima, konkretno koristeći Arduino Mega mikrokontroler. Projekt uključuje detekciju pritisaka tipkala s implementiranim prioritetima, generiranje prekida pomoću timer-a, te mjerenje udaljenosti.

## 2. Korištene komponente
- 1 Arduino Mega mikrokontroler
- 3 tipkala
- 5 LED dioda
- 5 otpornika
- 1 HC-SR04 senzor udaljenosti
- 1 logički analizator
  
## 3. Funkcionalnost
Projekt sadrži sljedeće prioritete, te viši prioriteti mogu prekinuti niže:
- a) Timer
- b) Tipkalo INT0
- c) Tipkalo INT1
- d) Tipkalo INT2
- e) Senzor udaljenosti

Tipkala INT0, INT1 i INT2 titraju svakih 100ms unutar jedne pune sekunde, dok LED dioda koja se odnosi na senzor svijetlosti titra svakih 200 ms. LED dioda koja se odnosi na timer se pali svaku jednu sekundu, te pošto ima najveći prioritet, na kratko vrijeme prekida sve druge LED diode.
