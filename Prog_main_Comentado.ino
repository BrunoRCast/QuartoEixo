/*Inclusão das bibliotecas responsáveis pelo funcionamento do display */
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000
//------------------------------------------------------------------------------------

/*Declaração das variáveis e constantes, os valores de calibração que correspondem as variáveis TS_LEFT, TS_RT, TS_TOP, TS_BOT são obtidos utilizando o programa TouchScreen_Calibr_native.ino disponibilizado na própria biblioteca. */
const int XP = 6, XM = A2, YP = A1, YM = 7;
const int TS_LEFT=892,TS_RT=106,TS_TOP=107,TS_BOT=897;

bool On_Off = 0, mover = 0, md_rel = 1, md_abs = 0,  plsMover = 0, borda_pulso = 0;
float nPulsos = 0, iGraus = 0, ipss = 0, posic_atual, var_temp1, var_temp2, var_temp3;
int pls_i = 0;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


Adafruit_GFX_Button inc10, dec100, inc100, dec10, moverBT, inc1, dec1, md_Rel, md_Abs, zerar, d_H, d_AH;
int pixel_x, pixel_y;
//------------------------------------------------------------------------------------

/*função responsável por capturar as coordenadas quando a tela é pressionada*/
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH); 
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
    
        pixel_x = map(p.y, TS_TOP, TS_BOT, 0, tft.width());
        pixel_y = map(p.x, TS_LEFT, TS_RT, 0, tft.height());
   
    }
    return pressed;
}
//------------------------------------------------------------------------------------

/*Declaração de algumas constantes para facilitar a programação quando necessário se referir a cores */
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
//------------------------------------------------------------------------------------

/*Função criada para atualizar campo numérico que representa o valor do ângulo na tela*/
void Atualiza_i_n(void)
{
            tft.fillRect(200, 10, 100, 60, BLUE);
            tft.setTextSize(5);
            tft.setTextColor(YELLOW, BLUE);
            tft.setCursor(100, 22);
            tft.println(iGraus);
            tft.setTextSize(3);
            tft.setCursor(250, 22);
            tft.println("o");
}
//------------------------------------------------------------------------------------

/*Configuração das entradas e saídas físicas do arduino*/
void setup(void)
{
pinMode(45, INPUT); //SENSOR INDUTIVO LIBERA MOVIMENTO
pinMode(43, OUTPUT); //PULSO MOVIMENTO COMPLETADO
pinMode(49, OUTPUT); //SAIDA DE PULSO
pinMode(51, OUTPUT); //SAIDA DE DIREÇÃO
pinMode(53, OUTPUT); //SAIDA DE ENABLE
//------------------------------------------------------------------------------------  

/*Inicia a comunicação serial, foi utilizado para debug do programa*/
    Serial.begin(19200); 
//------------------------------------------------------------------------------------

/*Inicia comunicação com o display*/ 
   tft.begin(0x4532); 
//------------------------------------------------------------------------------------
 
/*Defini rotação de exibição do display*/ 
    tft.setRotation(1);  
//------------------------------------------------------------------------------------

/*Defini cor de fundo do display*/
    tft.fillScreen(BLACK); 
//------------------------------------------------------------------------------------

/* Declaração das coordenadas, cores e tamanho dos textos dos botões */
 inc1.initButton(&tft,  40, 93, 70, 30, WHITE, CYAN, BLACK, "+0.09", 2);
 inc100.initButton(&tft,  120, 93, 70, 30, WHITE, CYAN, BLACK, "+9", 2);
     
 dec100.initButton(&tft, 200, 93, 70, 30, WHITE, CYAN, BLACK, "-9", 2);
 dec1.initButton(&tft, 280, 93, 70, 30, WHITE, CYAN, BLACK, "-0.09", 2);
 
 inc10.initButton(&tft,  80, 130, 70, 30, WHITE, CYAN, BLACK, "+0.9", 2);
 dec10.initButton(&tft,  245, 130, 70, 30, WHITE, CYAN, BLACK, "-0.9", 2);
 
 zerar.initButton(&tft,  162, 130, 70, 30, BLUE, GREEN, BLACK, "ZERAR", 2);
  
 moverBT.initButton(&tft,  45, 205, 70, 50, BLUE, RED, BLACK, "MOVER", 2);
 md_Rel.initButton(&tft,  115, 205, 50, 30, BLUE, GREEN, BLACK, "REL", 2);
 md_Abs.initButton(&tft,  170, 205, 50, 30, BLUE, GREEN, BLACK, "ABS", 2);
 d_H.initButton(&tft,  225, 205, 50, 30, BLUE, GREEN, BLACK, "H", 2);
 d_AH.initButton(&tft,  280, 205, 50, 30, BLUE, GREEN, BLACK, "AH", 2);
//------------------------------------------------------------------------------------ 

/*Chamada de funções para desenhar os botões na tela */     
    inc1.drawButton(false);
    dec100.drawButton(false);
    inc100.drawButton(false);
    dec1.drawButton(false);
    moverBT.drawButton(false);
  
    inc10.drawButton(false);
    dec10.drawButton(false);
    zerar.drawButton(false);
    md_Rel.drawButton(false);
    md_Abs.drawButton(false);
    d_H.drawButton(false);
    d_AH.drawButton(false);
//------------------------------------------------------------------------------------       

/*Desenha um retângulo azul na tela e exibe o campo numérico que exibe o ângulo desejado*/ 
    
tft.fillRect(50, 10, 220, 60, BLUE);

Atualiza_i_n();
//------------------------------------------------------------------------------------    

/*Apaga parte do display e exibe o nome do projeto nas coordenadas declaradas*/ 
    tft.setTextSize(2);
    tft.setTextColor(GREEN, BLACK);
    tft.setCursor(0, 157);
    tft.print("                         ");
    tft.setCursor(0, 157);
    tft.print("-POSICIONADOR QUARTO EIXO-");
//------------------------------------------------------------------------------------    

}

/*Início da função prncipal loop*/ 
void loop(void)
{
//------------------------------------------------------------------------------------    

/*Testa três vezes o sensor indutivo com intervalos de cinquenta milisegundos com a finalidade de inibir acionamentos provindos de ruídos*/ 
plsMover = digitalRead(45);
if (plsMover == 0){
delay(50);
if (plsMover == 0) 
{
delay(50);
if (plsMover == 0) 	
{mover = 1;}
}
}
//------------------------------------------------------------------------------------    

/*Trata movimento relativo quando botão mover for pressionado ou sensor indutivo for acionado*/
while ((mover == 1) && (md_rel == 1))
{
//------------------------------------------------------------------------------------    

/*Converte e armazena Grau em número de pulsos, multiplica pela redução (4000/360 == 11.1111111111) erro total de 32 pulsos em uma volta de 320k pulsos devido arredondamento para 11.11, 360 == 320k então ((360 / 320k) * 32) ERRO == 0.036° */
nPulsos = iGraus * 11.11 * 80; 

//------------------------------------------------------------------------------------    

/*Envia pulso de um passo para o drive de motor de passo e incrementa variável ipss(controle de quantidade de passos), realiza esse procedimento enquanto a variável ipss for menor que a variável nPulsos*/
  for (ipss = 0; ipss < nPulsos; ipss++) {
  digitalWrite(49, HIGH);
  delayMicroseconds(100);
  digitalWrite(49, LOW); 
  delayMicroseconds(100);
  }
//------------------------------------------------------------------------------------    

/*Trata final do movimento zerando variáveis, apagando a mensagem "MOVENDO" do display e acionando o indicador sonoro durante 400 milisegundos*/
    if (ipss >= nPulsos)
 {
    mover = 0; 
    ipss = 0;
    tft.setTextSize(2);
    tft.setTextColor(GREEN, BLACK);
    tft.setCursor(0, 157);
    tft.print("                          ");
    }
  digitalWrite(43, HIGH);   
  delay(400);                       
  digitalWrite(43, LOW);   
  
}
//------------------------------------------------------------------------------------    

/*Trata movimento absoluto quando botão mover for pressionado ou sensor indutivo for acionado*/
while ((mover == 1) && (md_abs == 1))
{
if (posic_atual == iGraus)
{
    mover = 0;  
    tft.setCursor(0, 157);
    tft.print("                         ");
}

if (posic_atual > iGraus)
{
digitalWrite(51, HIGH);
var_temp1 = posic_atual - iGraus;
var_temp2 = var_temp1 * 11.11 * 80;
  Serial.print("var2 = ");
  Serial.print(var_temp2);
  Serial.print("\r\n");
for (ipss = 0; ipss <= var_temp2; ipss++)
{
  digitalWrite(49, HIGH);
  delayMicroseconds(100);
  digitalWrite(49, LOW); 
  delayMicroseconds(100);	
}
    if (ipss > var_temp2)
 {
    mover = 0; 
    ipss = 0;
	posic_atual = iGraus;
    digitalWrite(51, LOW);
	tft.setTextSize(2);
    tft.setTextColor(GREEN, BLACK);
    tft.setCursor(0, 157);
    tft.print("                         ");
  digitalWrite(43, HIGH);   
  delay(400);                       
  digitalWrite(43, LOW);   
  
    }
}	
if (posic_atual < iGraus)
{
var_temp1 =iGraus - posic_atual;
var_temp2 = var_temp1 * 11.11 * 80;
  Serial.print("var2 = ");
  Serial.print(var_temp2);
  Serial.print("\r\n");
for (ipss = 0; ipss < var_temp2; ipss++)
{
  digitalWrite(49, HIGH);
  delayMicroseconds(100);
  digitalWrite(49, LOW); 
  delayMicroseconds(100);	
}
    if (ipss >= var_temp2)
 {
    mover = 0; 
    ipss = 0;
	posic_atual = iGraus;
    digitalWrite(51, LOW);
	tft.setTextSize(2);
    tft.setTextColor(GREEN, BLACK);
    tft.setCursor(0, 157);
    tft.print("                         ");
    
  digitalWrite(43, HIGH);   
  delay(400);                       
  digitalWrite(43, LOW);

  }
	
}	
	
}	
//------------------------------------------------------------------------------------    
/* Faz comparação entre coordenadas pressionadas na tela e coordenadas dos botões para determinar qual botão foi pressionado*/
    bool down = Touch_getXY();
    inc1.press(down && inc1.contains(pixel_x, pixel_y));
    dec1.press(down && dec1.contains(pixel_x, pixel_y));
    inc10.press(down && inc10.contains(pixel_x, pixel_y));
    dec10.press(down && dec10.contains(pixel_x, pixel_y));
    inc100.press(down && inc100.contains(pixel_x, pixel_y));
    dec100.press(down && dec100.contains(pixel_x, pixel_y));
    
    moverBT.press(down && moverBT.contains(pixel_x, pixel_y));
    zerar.press(down && zerar.contains(pixel_x, pixel_y));
    md_Rel.press(down && md_Rel.contains(pixel_x, pixel_y));
    md_Abs.press(down && md_Abs.contains(pixel_x, pixel_y));
    d_H.press(down && d_H.contains(pixel_x, pixel_y));
    d_AH.press(down && d_AH.contains(pixel_x, pixel_y));

//------------------------------------------------------------------------------------      
/*Faz animação de botão sendo pressionado*/
 
    if (inc1.justReleased())
        inc1.drawButton();
   
    if (inc10.justReleased())
        inc10.drawButton();
   
    if (inc100.justReleased())
        inc100.drawButton();
   
    if (dec1.justReleased())
        dec1.drawButton();

    if (dec10.justReleased())
        dec10.drawButton();

    if (dec100.justReleased())
        dec100.drawButton();

    if (zerar.justReleased())
        zerar.drawButton();

    if (md_Rel.justReleased())
        md_Rel.drawButton();

    if (md_Abs.justReleased())
        md_Abs.drawButton();

    if (d_H.justReleased())
        d_H.drawButton();

    if (d_AH.justReleased())
        d_AH.drawButton();
//------------------------------------------------------------------------------------ 

/*Trata botão MOVER, se for pressionado, exibe mensagem "MOVENDO" na tela e realiza o movimento*/
if ((moverBT.justReleased()) && (mover == 0))
{

    tft.setTextSize(2);
    tft.setTextColor(RED, BLACK);
    tft.setCursor(0, 157);
    tft.print("                          ");
	tft.setCursor(0, 157);
	tft.print(">>>>>>>> MOVENDO <<<<<<<<");
    mover = 1;
}       
//------------------------------------------------------------------------------------ 

/*Trata botões de incremento e decremento do valor em grau*/
    if ((inc1.justPressed()) && (mover == 0)) {
        inc1.drawButton(true);
                iGraus = iGraus + 0.09;
Atualiza_i_n();
    }
    if ((inc10.justPressed()) && (mover == 0)) {
        inc10.drawButton(true);
                iGraus = iGraus + 0.9;
Atualiza_i_n();
    }
    if ((inc100.justPressed()) && (mover == 0)) {
        inc100.drawButton(true);
        iGraus = iGraus + 9;
Atualiza_i_n();
    }

    if ((dec1.justPressed()) && (mover == 0)) {
        dec1.drawButton(true);
               iGraus = iGraus - 0.09;     
Atualiza_i_n();    
}
    if ((dec10.justPressed()) && (mover == 0)) {
        dec10.drawButton(true);
               iGraus = iGraus - 0.9;     
Atualiza_i_n();    
}
    if ((dec100.justPressed() && (mover == 0))) {
        //if (iGraus < 0) iGraus = 0;
        dec100.drawButton(true);
        iGraus = iGraus - 9;     
Atualiza_i_n();    
}
//------------------------------------------------------------------------------------ 

/*Trata botão de Zerar a poição, este botão é utilizado após o posicionamento do eixo no grau desejado a ser o ponto 0 tanto em movimento relativo quanto em movimento absoluto*/
    if ((zerar.justPressed()) && (mover == 0)) {
        zerar.drawButton(true);
               iGraus = 0;
               posic_atual = 0;                     
Atualiza_i_n();    
}
//------------------------------------------------------------------------------------ 

/*Seta o modo de funcionamento para modo absoluto quando o botão para este fim for pressionado e exibe mensagem do modo selecionado na tela*/
 if ((md_Abs.justPressed()) && (mover == 0)) {
        md_Abs.drawButton(true);
               md_abs = 1;
               md_rel = 0;
			   iGraus = 0;
			   posic_atual = 0;
    tft.setTextSize(2);
    tft.setTextColor(GREEN, BLACK);
	tft.setCursor(0, 157);
    tft.print("                         ");
    tft.setCursor(0, 157);
    tft.print("MODO ABSOLUTO SELECIONADO ");
}
//------------------------------------------------------------------------------------ 

/*Seta o modo de funcionamento para modo relativo quando o botão para este fim for pressionado e exibe mensagem do modo selecionado na tela*/
    if ((md_Rel.justPressed()) && (mover == 0)) {
        md_Rel.drawButton(true);
               md_abs = 0;
               md_rel = 1;			   
    tft.setTextSize(2);
    tft.setTextColor(GREEN, BLACK);
	tft.setCursor(0, 157);
    tft.print("                         ");
    tft.setCursor(0, 157);
    tft.print("MODO RELATIVO SELECIONADO ");
}
//------------------------------------------------------------------------------------ 

/*Seta a direção do movimento para sentido horário, e testa o modo de funcionamento para permitir esta opção apenas no modo relativo*/
    if ((d_H.justPressed()) && (mover == 0) && (md_rel == 1)) 
	{
        d_H.drawButton(true);
		digitalWrite(51, HIGH);
			   
    tft.setTextSize(2);
    tft.setTextColor(GREEN, BLACK);
	tft.setCursor(0, 157);
    tft.print("                         ");
    tft.setCursor(0, 157);
    tft.print("SENTIDO HORARIO");
}
//------------------------------------------------------------------------------------ 

/*Seta a direção do movimento para sentido anti-horário, e testa o modo de funcionamento para permitir esta opção apenas no modo relativo*/
    if ((d_AH.justPressed()) && (mover == 0) && (md_rel == 1)) 
	{
        d_AH.drawButton(true);
		digitalWrite(51, LOW);
			   
    tft.setTextSize(2);
    tft.setTextColor(GREEN, BLACK);
	tft.setCursor(0, 157);
    tft.print("                         ");
    tft.setCursor(0, 157);
    tft.print("SENTIDO ANTI-HORARIO");
}
//------------------------------------------------------------------------------------ 

/*Controle de saturação de variável para impedir que o valor desejado para o movimento seja maior que uma volta*/   
   if (iGraus < 0){
    iGraus = 0;
    Atualiza_i_n();   
   }
   if (iGraus > 360){
    iGraus = 360;
   Atualiza_i_n();
   }
//------------------------------------------------------------------------------------ 
  
}

