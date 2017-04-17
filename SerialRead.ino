
//S,BBB.BBB,TTT.TTT,LLL.LLL

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define DEBUG  /// Habilita debug na serial
#define led 13

byte  Index = 0;                      //Indexador para o BufRX.
const int BUFFER_SIZE = 28;           //Tamanho do buffer serial.
unsigned char BufSerial[BUFFER_SIZE]; //Buffer para armazenar os dados recebidos da serial.
const int INTERVALO = 20;             //Tempo para receber os dados seriais
bool ChegouDados = false;
String flag, pesoBruto, tara, pesoLiquido;
static unsigned long TempoAnterior = 0;          //Inicializa em 0ms.
unsigned long TempoCorrente = millis();          //Retorna o tempo desde que o programa foi executado.

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT), digitalWrite(led, LOW);

  lcd.begin(16, 2);
  lcd.print("# Balanca serial #");
  lcd.setCursor(0, 1);
  lcd.print("iniciando");
#ifdef DEBUG
  Serial.println("iniciando Balança");
#endif
  for (int i = 0; i < 7; i++) {
    lcd.setCursor((i + 1), 1);
    lcd.print("*");
    delay(100);
  }
  delay(1000);
  lcd.clear();
#ifdef DEBUG
  Serial.println("Balança iniciada");
#endif
}
void loop() {
  if (ChegouDados)
  {
    ChegouDados = false;
#ifdef DEBUG
    Serial.println((char*)BufSerial);
#endif
    //S,BBB.BBB,TTT.TTT,LLL.LLL
    String streamData    = (char*)BufSerial;
    int commaIndex       = streamData.indexOf(',');//1
    int secondCommaIndex = streamData.indexOf(',', commaIndex + 1); //9
    int thirdCommaIndex  = streamData.indexOf(',', secondCommaIndex + 1);//17

    if (commaIndex > 0 and secondCommaIndex > 0 and thirdCommaIndex > 0) {
      flag        = streamData.substring(0, commaIndex);
      pesoBruto   = streamData.substring(commaIndex + 1, secondCommaIndex);
      tara        = streamData.substring(secondCommaIndex + 1, thirdCommaIndex);
      pesoLiquido = streamData.substring(thirdCommaIndex + 1);
#ifdef DEBUG
      Serial.println(flag);
      Serial.println(pesoBruto);
      Serial.println(tara);
      Serial.println(pesoLiquido);
#endif
      if (flag == "1")// Verifica se Peso esta estável para imprimir no LCD
      {
        digitalWrite(led, HIGH);
      }
      else
      {
        digitalWrite(led, LOW);
      }
      memset(BufSerial, 0, BUFFER_SIZE - 1); //Limpa o buffer.
      lcd.setCursor(0, 1);
      lcd.print("Peso L = ");
      lcd.print(pesoLiquido);
    }
  }///end ChegouDados
}//end loop

void serialEvent()  //
{
  TempoAnterior = TempoCorrente = millis();
  while (Serial.available()) //Le dados enquanto o buffer Serial tiver dados.
  {
    TempoCorrente = millis();
    //Timeout para limpar o BufRX e zerar o indexador (Index), quando (TempoCorrente - TempoAnterior) for maior que INTERVALO.
    if ( (TempoCorrente - TempoAnterior) > INTERVALO)  //Executa as intruções abiaxo após se pássarem 20ms.
    {
      //TempoAnterior = TempoCorrente;
      Index = 0;                             //Inicializa ixdexador do array.
      memset(BufSerial, 0, BUFFER_SIZE - 1); //Limpa o buffer.
      break;
    }
    
    unsigned char SerialByte = Serial.read(); // Guarda o byte do buffer serial;

    delay(2);

    switch (SerialByte)
    {
      case '\r':  //CR.
      case '\n':  //LF.

        if (Index == 0) //Se o primeiro byte for CR ou LF, despreza-os.
        {
          return;
        }
        Index = 0;           //Zera o indexador do buffer.
        ChegouDados = true;  //Avisa que chegou Dados na serial.
        break;

      default:
        if (Index < BUFFER_SIZE - 1) //Checa se há espaço no buffer.
        {
          BufSerial[Index++] = SerialByte; //Armazena o byte lido pela serial, no buffer.
          TempoAnterior = TempoCorrente;
        }
    }
  }
}

