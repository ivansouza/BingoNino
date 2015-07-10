// BingoNino
// Máquina de Bingo baseada no Arduino
// Copyright (c) 2011, Ivan Tadeu dos Santos Souza
// O código é licenciado pela BSD New License.(Veja no final do arquivo). 
//
//  Verifique a pinagem de hardware!
//  LCD: A2, A0, 12, 11, 10 e 9
//  Botão: 5,  Buzzer: 8
//  Rotary Encoder: 2 e 3

#include <Wire.h>
#include <LiquidCrystal.h>
// Definição de frequencia das Notas de melodias
#include "pitches.h"

LiquidCrystal lcd(A2, A0, 12, 11, 10, 9); // Pinos do Arduino ligados ao LCD 16x2

// As oito Matrizes que formam os segmentos dos números grandes
byte LT[8] =  { B01111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };
byte UB[8] =  { B11111, B11111, B11111, B00000, B00000, B00000, B00000, B00000 };
byte RT[8] =  { B11110, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };
byte LL[8] =  { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B01111 };
byte LB[8] =  { B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111 };
byte LR[8] =  { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11110 };
byte UMB[8] = { B11111, B11111, B11111, B00000, B00000, B00000, B11111, B11111 };
byte LMB[8] = { B11111, B00000, B00000, B00000, B00000, B11111, B11111, B11111 };


volatile int number = 0;	// Variável de Controle do número sorteado
					                // também utilizada nas rotinas de interrução do encoder
int oldnumber = number;   // Variavel temporária de controle do número anterior
int buttonState = 0;		  // Variavel de Controle do Botão

volatile boolean halfleft = false;	// Usada nas interrupções do encoder
volatile boolean halfright = false; // Usada nas interrupções do encoder

// Variável de controle de posição de número grande
int x = 0;

// Melodia de Início do Bingo:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// Duração das notas da melodia de Início do Bingo:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

// Variável de controle do número randômico:
long randNumber;

// A Matriz dos números do Bingo (75 pedras):
int anArray[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75 };

// Tamanho da Matriz:
int elements = sizeof(anArray) / sizeof(int);

// Variável de Controle do número sorteado:
int numsort = 0;

// Variáveis de Controle das letras do Bingo...
int numletra, numletraant;

void setup()
{
  pinMode(9, OUTPUT);     // Incialização dos Pinos do LCD...
  digitalWrite(9, LOW);   //
  pinMode(10, OUTPUT);     //
  digitalWrite(10,LOW);    //
  pinMode(11, OUTPUT);     //
  digitalWrite(11,LOW);    //
  pinMode(12, OUTPUT);     //
  digitalWrite(12,LOW);    //
  pinMode(A0, OUTPUT);     //
  digitalWrite(A0,LOW);    //
  pinMode(A2, OUTPUT);     //
  digitalWrite(A2,LOW);    //
  
   
  pinMode(8,OUTPUT);       // Inicialização do Pino do Buzzer
  pinMode(8,LOW);
  
  lcd.begin(16,2);         // Inicialização o LCD

  pinMode(5, INPUT);       // Inicialização do Pino do Botão de Sorteio
  digitalWrite(5,HIGH);
    
  pinMode(2, INPUT);       // Inicialização dos Pinos do Encoder
  digitalWrite(2, HIGH);	 // Liga o resistor interno de pullup
  pinMode(3, INPUT);       // Inicialização dos Pinos do Encoder
  digitalWrite(3, HIGH);	 // Liga o resistor interno de pullup

  // Inicialização das interrupções do rotary encoder:
  attachInterrupt(0, isr_2, FALLING);	// Chama isr_2 quando o pino 2 vai para LOW
  attachInterrupt(1, isr_3, FALLING);	// Chama isr_3 quando o pino 3 vai para LOW

  // Controle da semente do randômico
  unsigned long seed = seedOut(31);
  randomSeed(seed);
  
  // Cria os segmentos dos números grandes:
  lcd.createChar(0, LT);
  lcd.createChar(1, UB);
  lcd.createChar(2, RT);
  lcd.createChar(3, LL);
  lcd.createChar(4, LB);
  lcd.createChar(5, LR);
  lcd.createChar(6, UMB);
  lcd.createChar(7, LMB);

  // Embaralha os números da Matriz do Bingo...
  shuffle(anArray, elements, sizeof(int));
  
  // Inicializa uma variável de controle:
  oldnumber = 1;

  // Mensagem de boas vindas...
  lcd.setCursor(3, 0);
  lcd.print("B I N G O");
  lcd.setCursor(1, 1);
  lcd.print("by Ivan Souza");

  // Toca a melodia de boas vindas...
  for (int thisNote = 0; thisNote < 8; thisNote++) {
	  int noteDuration = 1000 / noteDurations[thisNote];
	  tone(8, melody[thisNote], noteDuration);
  	int pauseBetweenNotes = noteDuration * 1.30;
	  delay(pauseBetweenNotes);
	noTone(8);
  }

  // Apresenta a mensagem de início do Bingo...
  delay(500);
  lcd.clear();
  lcd.print(" Aperte o botao");
  lcd.setCursor(2, 1);
  lcd.print("para Iniciar");

  // Aguarda o pressionamento do Botão para Iniciar...
  while (buttonState == LOW) {
	  buttonState = digitalRead(5);
  }

  // Toca o Tom de Início do Bingo / Pressionamento do botão...
  int noteDuration = 1000 / 16;
  tone(8, NOTE_G5, noteDuration);
  delay(500);
  lcd.clear();
}

void loop()
{
  // Inicializa variável de Controle e das letras do bingo
  int kk;
  String letra = "BINGO";

  // Verifica o Pressionamento do Botão...
  buttonState = digitalRead(5);
  if (buttonState == HIGH) {
    // Sorteia o Numero...
	  numsort++;
	  number = numsort;
    digitalWrite(5,LOW); // desliga o botão...

    // Toca o Tom de Início do Botão...
    int noteDuration = 1000 / 16;
	  tone(8, NOTE_G5, noteDuration);

    // Pausa de meio segundo...
	  delay(500);
  }

  // Controle de Fim e Início do número sorteado
  if (number < 0) {
	  number = 0;
	  tone(8, NOTE_G2, 1000 / 16);
  }
  if (numsort > 74) {
	  numsort = 74;
	  number = numsort;
  }
  if (number > numsort) {
	  number = numsort;
	  tone(8, NOTE_G2, 1000 / 16);
  }

  // Controle de novo número...
  if (number != oldnumber) {	
	  Serial.println(number);
	  kk = number;
	  oldnumber = number;
	  if (number < 0) {
	    number = 0;
	    tone(8, NOTE_G2, 1000 / 16);
	  }
	  if (number > 74) {
	    number = 74;
	    tone(8, NOTE_G2, 1000 / 16);
	  }

    // Apresenta a letra e o novo número sorteado...
	  lcd.setCursor(8, 0);
	  lcd.print(" ");
	  lcd.setCursor(8, 0);
	  numletra = map(anArray[kk], 0, 76, 0, 5);
	  lcd.print(letra.substring(numletra, numletra + 1));
	  lcd.setCursor(0, 0);
	  lcd.print("  /  ");
	  lcd.setCursor(0, 0);
	  lcd.print(kk + 1);
	  lcd.print("/75");
	  if (kk > 0) {
	    numletraant = map(anArray[kk - 1], 0, 76, 0, 5);
	    lcd.setCursor(5, 1);
	    lcd.print("   ");
	    lcd.setCursor(5, 1);
	    lcd.print(letra.substring(numletraant, numletraant + 1));
	    lcd.print(anArray[kk - 1]);
	  } else {
	    lcd.setCursor(5, 1);
	    lcd.print("   ");
	  }
	  x = 9;
	  numberf(anArray[kk]);
  }
}

// Função para gerar randomico de um valor: 0 <= x < n, onde n é o número de possíveis números
int rand_range(int n)
{
    int r, ul;
    ul = RAND_MAX - RAND_MAX % n;
    while ((r = random(RAND_MAX + 1)) >= ul);
    //r = random(ul);
    return r % n;
}

// Função para realizar a troca dos números da matriz...
void shuffle_swap(int index_a, int index_b, int *array, int size)
{
    char *x, *y, tmp[size];

    if (index_a == index_b)
	return;

    x = (char *) array + index_a * size;
    y = (char *) array + index_b * size;

    memcpy(tmp, x, size);
    memcpy(x, y, size);
    memcpy(y, tmp, size);
}

// Função para misturar todos os itens da matriz...
void shuffle(int *array, int nmemb, int size)
{
    int r;

    while (nmemb > 1) {
	r = rand_range(nmemb--);
	shuffle_swap(nmemb, r, array, size);
    }
}

// Função de controle de número grande menor que 10 (zero a esquerda)...
void numberf(int n)
{
    custom(n / 10);
    x = x + 4;
    custom(n % 10);
}


// Função para imprimir o número grande...
void custom(int n)
{
    switch (n) {
    case 0:{			          // Segmentos usados para fazer o número 0
	    lcd.setCursor(x, 0);	
	    lcd.write((byte)0);	
	    lcd.write(1);	
	    lcd.write(2);
	    lcd.setCursor(x, 1);	
	    lcd.write(3);	
	    lcd.write(4);	
	    lcd.write(5);
	    break;
	}

    case 1:{
	    lcd.setCursor(x, 0);
	    lcd.write(32);
	    lcd.write(32);
	    lcd.write(2);
	    lcd.setCursor(x, 1);
	    lcd.write(32);
	    lcd.write(32);
	    lcd.write(5);
	    break;
	}
    case 2:{
	    lcd.setCursor(x, 0);
	    lcd.write(6);
	    lcd.write(6);
	    lcd.write(2);
	    lcd.setCursor(x, 1);
	    lcd.write(3);
	    lcd.write(7);
	    lcd.write(7);
	    break;
	}

    case 3:{
	    lcd.setCursor(x, 0);
	    lcd.write(6);
	    lcd.write(6);
	    lcd.write(2);
	    lcd.setCursor(x, 1);
	    lcd.write(7);
	    lcd.write(7);
	    lcd.write(5);
	    break;
	}

    case 4:{
	    lcd.setCursor(x, 0);
	    lcd.write(3);
	    lcd.write(4);
	    lcd.write(2);
	    lcd.setCursor(x, 1);
	    lcd.write(32);
	    lcd.write(32);
	    lcd.write(5);
	    break;
	}

    case 5:{
	    lcd.setCursor(x, 0);
	    lcd.write((byte)0);
	    lcd.write(6);
	    lcd.write(6);
	    lcd.setCursor(x, 1);
	    lcd.write(7);
	    lcd.write(7);
	    lcd.write(5);
	    break;
	}

    case 6:{
	    lcd.setCursor(x, 0);
	    lcd.write((byte)0);
	    lcd.write(6);
	    lcd.write(6);
	    lcd.setCursor(x, 1);
	    lcd.write(3);
	    lcd.write(7);
	    lcd.write(5);
	    break;
	}

    case 7:{
	    lcd.setCursor(x, 0);
	    lcd.write(1);
	    lcd.write(1);
	    lcd.write(2);
	    lcd.setCursor(x, 1);
	    lcd.write(32);
	    lcd.write((byte)0);
	    lcd.write(32);
	    break;
	}

    case 8:{
	    lcd.setCursor(x, 0);
	    lcd.write((byte)0);
	    lcd.write(6);
	    lcd.write(2);
	    lcd.setCursor(x, 1);
	    lcd.write(3);
	    lcd.write(7);
	    lcd.write(5);
	    break;
	}

    case 9:{
	    lcd.setCursor(x, 0);
	    lcd.write((byte)0);
	    lcd.write(6);
	    lcd.write(2);
	    lcd.setCursor(x, 1);
	    lcd.write(32);
	    lcd.write(7);
	    lcd.write(5);
	    break;
	}

    }
}


// Rotina da Interrução do Pino 2 em Baixa para o Rotary Encoder...
void isr_2()
{				// Pin2 foi para LOW
    delay(1);			// Tempo para Debounce
    if (digitalRead(2) == LOW) {	// Pin2 continua LOW ?
	if (digitalRead(3) == HIGH && halfright == false) {	// -->
	    halfright = true;	// Meio ciclo clockwise
	}
	if (digitalRead(3) == LOW && halfleft == true) {	// <--
	    halfleft = false;	// Ciclo Inteiro counter-clockwise
	    number--;		
      // if(number<0) {number=0;} // (Testar)...
	}
    }
}

// Rotina da Interrução do Pino 3 em Baixa para o Rotary Encoder...
void isr_3()
{				// Pin3 foi para LOW
    delay(1);			// Tempo para Debounce
    if (digitalRead(3) == LOW) {	// Pin3 continua LOW ?
	if (digitalRead(2) == HIGH && halfleft == false) {	// <--
	    halfleft = true;	// Meio ciclo counter-clockwise
	}			
	if (digitalRead(2) == LOW && halfright == true) {	// -->
	    halfright = false;	// Ciclo Inteiro clockwise
	    number++;
	}
    }
}


// Função da Randomização da semente do randômico (kkk)

unsigned long seedOut(unsigned int noOfBits)
{
    unsigned long seed = 0, limit = 99;
    int bit0 = 0, bit1 = 0;
    while (noOfBits--) {
	for (int i = 0; i < limit; ++i) {
	    bit0 = analogRead(4) & 1;
	    bit1 = analogRead(4) & 1;
	    if (bit1 != bit0)
		break;
	}
	seed = (seed << 1) | bit1;
    }
    return seed;
}

// BingoNino
// Máquina de Bingo baseada no Arduino
// Copyright (c) 2015, Ivan Tadeu dos Santos Souza
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this 
// list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this 
// list of conditions and the following disclaimer in the documentation and/or other 
// materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may 
// be used to endorse or promote products derived from this software without 
// specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
