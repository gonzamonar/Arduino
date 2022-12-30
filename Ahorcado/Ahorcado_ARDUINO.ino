// C++ code
//
#include <LiquidCrystal.h>
#include <string.h>
#define NO_BUTTON 0
#define BUTTON_PREV  1
#define BUTTON_TRY  2
#define BUTTON_NEXT  3


//Iniciación del LCD: LiquidCrystal(rs, rw, enable, d4, d5, d6, d7)
LiquidCrystal lcd(2,3,4,5,6,7); 


//Mapas binarios para crear caracteres
byte heart[8] = {B00000,B11011,B11111,B11111,B01110,B00100,};
byte cross[8] = {B00000,B10001,B01010,B00100,B01010,B10001,};
byte crownA[8] = {B00000,B10000,B11000,B11101,B11111,B11111,B11111,B11111,};
byte crownB[8] = {B00100,B01110,B11111,B11111,B11111,B11111,B11111,B11111,};
byte crownC[8] = {B00000,B00001,B00011,B10111,B11111,B11111,B11111,B11111,};


//Prototipos
int start();
void pickKeyword();
void maskKeyword();
void printUI();
void printHearts();
void calcRemainingChars();
void printMaskedKeyword();
void printCharSelected();
int readButton();
void executeButton();
void prevChar();
void nextChar();
void tryChar();
void crossOutLives();
void displayMatches();
void checkVictory();
void displayGameover();
void resetGame();
void printVictoryMessage();
void printDefeatMessage();


void setup()
{
  Serial.begin(9600);
  lcd.begin(16, 2);     //iniciación del lcd
  lcd.createChar(1,heart);  //crear caracter de corazón
  lcd.createChar(2,cross);  //crear caracter de cruz
  lcd.createChar(4,crownA); //crear caracter de corona parte 1/3
  lcd.createChar(5,crownB); //crear caracter de corona parte 2/3
  lcd.createChar(6,crownC); //crear caracter de corona parte 3/3
}


//Declaración de variables globales
const char* dictionary[50] = {"JUVENIL","ROTULA","INFANTIL","AMARILLO","ROJO","CIRUGIA","HIGIENE","TOXICO","ALACRAN","EPOPEYA","VERDE","MENTA","VIRUS","PLANTA","GANGLIO","MESA","ESTUPENDO","TECLADO","SISTEMA","DATOS","RIGIDO","GABINETE","CEBRA","XILOFONO","ZARIGUEYA","PARLANTE","CELULAR","MANTEL","SANSCRITO","CELESTIAL","NEGRUZCO","ARCANO","ATIGRADO","CAOBA","HERMANO","FUNCION","PLEBEYO","TRUHAN","APROBADO","PRISTINO","BAYO","ESTUDIANTE","DRAGON","VALIJA","PASAPORTE","ROTONDA","PASACALLE","ACERADO","PARED","ARBOL"};
char currentChar = 'A';
char keyword[11];
char maskedKeyword[11];
char shownKeyword[11];
bool started = false;
bool gameover = false;
bool victory;
unsigned long currentMillis;
unsigned long lastMillis;
int buttonStatus;
int lastButtonStatus = NO_BUTTON;
int lives = 3;
int gameoverMsgStage = 0;
int remainingChars;




//MAIN-LOOP dividido en tres secciones:
//1. Iniciar el juego (!started)     líneas 84-87
//2. Juego en curso   (!gameover)    líneas 89-99
//3. Juego terminado  (else: gameover) líneas 100-103
void loop()
{
  if(!started)
  {
    started = start();
  }
  
  if(!gameover)
  {
    printMaskedKeyword();
    printCharSelected();
    
    buttonStatus = readButton();
    if(buttonStatus != NO_BUTTON && buttonStatus != lastButtonStatus){
      executeButton();
    }
    lastButtonStatus = buttonStatus;
  }
  else
  {
    displayGameover();
  }
  delay(20);
}





/////MAIN-LOOP FUNCTIONS 1  --  START GAME/////

//Función que inicia e imprime los recursos básicos del juego
int start()
{
  pickKeyword();
  maskKeyword();
  strcpy(shownKeyword, keyword);
  printUI();
  printHearts();
  calcRemainingChars();
  return 1;
}



          ///1. START SUB-FUNCTIONS///

//Función para seleccionar al azar una de las 50 palabras del diccionario.
//Emplea la lectura del pin 9 (sin conectar a nada) como seed de la función random.
void pickKeyword()
{
  long randNumber;
  randomSeed(analogRead(9));
  randNumber = (random() % 50);
  strcpy(keyword, dictionary[randNumber]);
}


//Función para enmascarar la palabra seleccionada al azar y guardarla
//en el array masked_keyword.
void maskKeyword()
{
  strcpy(maskedKeyword, keyword);
  int len = strlen(keyword);
  for(int i=0; i<len; i++)
  {
    maskedKeyword[i]='_';
  }
  /// *PARA PRUEBAS* ///
  Serial.println(keyword);//-> La palabra se imprime desenmascarada en Consola.
}


//Función que imprime parte estática de la UI en el LCD.
void printUI()
{
  lcd.setCursor(0, 0);
  lcd.print("Letra:");
  lcd.setCursor(12,0);
  lcd.print(":");
  
}


//Función que imprime las tres vidas (corazones) en el LCD.
void printHearts()
{
  lcd.setCursor(13, 0);
  lcd.write((byte)1);
  lcd.setCursor(14, 0);
  lcd.write((byte)1);
  lcd.setCursor(15, 0);
  lcd.write((byte)1);
}


//Función que calcula la cantidad de caracteres que va a tener
//que adivinar el usuario (= longitud de la palabra elegida).
void calcRemainingChars()
{
  remainingChars = strlen(keyword);
}


  


/////MAIN-LOOP FUNCTIONS 2  --  GAME RUNNING/////


//Función que imprime en el LCD la palabra clave enmascarada
void printMaskedKeyword()
{
  lcd.setCursor(3, 1);
  lcd.print(maskedKeyword);
}


//Función que imprime en el LCD el caracter actualmente seleccionado
void printCharSelected()
{
  lcd.setCursor(6, 0);
  lcd.print(currentChar);
}


//Función que evalúa el valor del divisor de tensión conectado
//a A0 y retorna cuál botón está siendo pulsado
int readButton()
{
  int valueA0 = analogRead(A0);

  if(valueA0>502 && valueA0<522)
    return BUTTON_PREV;

  if(valueA0>672 && valueA0<692)
    return BUTTON_TRY;

  if(valueA0>757 && valueA0<777)
    return BUTTON_NEXT;

  return NO_BUTTON;
}


//Función que llama a la ejecución de la función
//correspondiente a cada botón
void executeButton()
{
  switch(buttonStatus)
  {
    case BUTTON_PREV:
      prevChar();
      break;
    case BUTTON_TRY:
      tryChar();
      break;
    case BUTTON_NEXT:
      nextChar();
      break;
  }
}


  
      ///2A. BUTTON SUB-FUNCTIONS///

//Función que retrocede en uno el caracter seleccionado
//Caracter mínimo: A (65)
void prevChar()
{
  currentChar--;
  if(currentChar<65)
    currentChar += 26;
}


//Función que avanza en uno el caracter seleccionado
//Caracter máximo: Z (90)
void nextChar()
{
  currentChar++;
  if(currentChar>90)
    currentChar -= 26;
}


//Función que compara el caracter elegido con la palabra clave
//si no hay coincidencia (==NULL) se remueve una vida e imprime el caracter errado
//si hay coincidencia (!=NULL) se muestran los aciertos y verifica si hay victoria
void tryChar()
{
  if(strchr(keyword, currentChar)==NULL)
  {
  lives--;
    crossOutLives();
  }
  else
  {
    displayMatches();
    checkVictory();
  }
}


      ///2B. TRYCHAR SUB-FUNCTIONS///

//Función que sobre-escribe con una cruz los corazones según cuántas vidas
//le quedan al jugador e imprime en la pantalla el caracter errado.
//Si las vidas son 0, determina el fin del juego por derrota.
void crossOutLives()
{
  switch(lives){
    case 2:
        lcd.setCursor(15, 0);
        lcd.write((byte)2);
        lcd.setCursor(11, 0);
        break;
    case 1:
        lcd.setCursor(14, 0);
        lcd.write((byte)2);
        lcd.setCursor(10, 0);
        break;
    case 0:
        lcd.setCursor(13, 0);
        lcd.write((byte)2);
        lcd.setCursor(9, 0);
        gameover = true;
        victory = false;
        break;
    default:
        lcd.setCursor(9, 0);
        break;
  }
  lcd.write(currentChar);
}


//Función que desenmascara los aciertos del caracter elegido
//y los resta a la cantidad de letras restantes por acertar.
void displayMatches()
{
  int len = strlen(keyword);
  for(int i=0; i<len; i++)
  {
    if(keyword[i]==currentChar)
    {
      maskedKeyword[i]=currentChar;
      keyword[i]='_';
      remainingChars--;
    }
  }
}


//Función que verifica si quedan caracteres restantes por adivinar.
//Si los restantes son 0, determina el fin del juego por victoria.
void checkVictory()
{
  if(remainingChars==0)
  {
    gameover = true;
    victory = true;
  }
}





/////MAIN-LOOP FUNCTIONS 3  --  GAME OVER/////

//Función que organiza el cierre del juego según una victoria o derrota.
//Se muestran tres mensajes en cada caso y se concluye con el reset del juego.
void displayGameover()
{
  currentMillis = millis();
  if(currentMillis - lastMillis > 1500)
  {
    lastMillis = currentMillis;
    gameoverMsgStage++;
    lcd.clear();

    if(victory)
    {
      printVictoryMessage();
    }
    else
    {
      printDefeatMessage();
    }
  }

  if(gameoverMsgStage==4)
  {
    resetGame();
  }
}



/// 3. GAMEOVER SUB-FUNCTIONS ///

//Función que reinicia las variables globales que regresan
//el juego a la carga inicial y generan un nuevo start
void resetGame()
{
  lcd.clear();
  started = false;
  gameover = false;
  currentChar = 'A';
  maskedKeyword[0] = '\0';
  lives = 3;
  gameoverMsgStage = 0;
}


//Función para imprimir en el LCD el mensaje de VICTORIA
//en tres etapas, cada una con un mensaje diferente.
void printVictoryMessage()
{
  switch(gameoverMsgStage)
  {
    case 1:
    lcd.setCursor(1, 0);
    lcd.write("** Ganaste! **");
    break;
    case 2:
    lcd.setCursor(1, 1);
    lcd.write("** GANASTE! **");
    break;
    case 3:
    lcd.setCursor(11, 0);
    lcd.write((byte)4);
    lcd.setCursor(12, 0);
    lcd.write((byte)5);
    lcd.setCursor(13, 0);
    lcd.write((byte)6);
    lcd.setCursor(1, 0);
    lcd.write("Toma se");
    lcd.setCursor(1, 1);
    lcd.write("te cayo: CHAMP");
    break;
  }
}


//Función para imprimir en el LCD el mensaje de DERROTA
//en tres etapas, cada una con un mensaje diferente.
void printDefeatMessage()
{
  switch(gameoverMsgStage)
  {
    case 1:
    lcd.setCursor(1, 0);
    lcd.write("Perdiste...");
    break;
    case 2:
    lcd.setCursor(1, 0);
    lcd.write("Perdiste");
    lcd.setCursor(1, 1);
    lcd.write("y era facil.");
    break;
    case 3:
  lcd.setCursor(1, 0);
    lcd.write("La palabra era:");
    lcd.setCursor(1, 3);
    lcd.write(shownKeyword);
    break;
  }
}
