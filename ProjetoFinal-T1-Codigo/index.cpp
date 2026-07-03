#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ==========================================
// 1. PINOS DOS COMPONENTES
// ==========================================
#define PINO_POT          A0
#define PINO_ECHO         11
#define PINO_TRIG         12
#define PINO_BUZZER        8
#define PINO_LED_VERDE     6
#define PINO_LED_VERMELHO 7

#define BOTAO_AMARELO     2
#define BOTAO_AZUL        3
#define BOTAO_VERDE       4
#define BOTAO_VERMELHO    5 // Adaptado: Pino 5 agora é o Botão Vermelho

// ==========================================
// 2. CONFIGURAÇÕES DO JOGO
// ==========================================
#define TAMANHO_FILTRO    8
#define DISTANCIA_MAO     5   

#define TEMPO_INICIAL     7000  // 7 segundos para começar
#define TEMPO_LIMITE      2000  // Limite máximo de velocidade (2 segundos)
#define DESCONTO_TEMPO    250   // Quanto tempo diminui a cada acerto

enum Acoes { GIRAR, PASSAR_MAO, APERTAR_AMARELO, APERTAR_AZUL, APERTAR_VERDE, APERTAR_VERMELHO, TOTAL_ACOES };
const char* textosAcoes[] = { "Gire o Botao!", "Passe a Mao!", "Aperte Amarelo", "Aperte Azul", "Aperte Verde", "Aperte Vermelho" };

// Configurado para endereço 32 (0x20) do novo display I2C padrão do Tinkercad
LiquidCrystal_I2C lcd(0x20, 16, 2); 

int leiturasPot[TAMANHO_FILTRO];
int indiceFiltro = 0;
int pontos = 0;
unsigned long tempoJogo = TEMPO_INICIAL;
Acoes comandoAtual;

// ==========================================
// 3. FILTROS E TRATAMENTO DE SINAIS
// ==========================================

int lerPotFiltrado() {
    int leituraAtual = analogRead(PINO_POT);
    leiturasPot[indiceFiltro] = leituraAtual;
    indiceFiltro = (indiceFiltro + 1) % TAMANHO_FILTRO;
    
    long soma = 0;
    for(int i = 0; i < TAMANHO_FILTRO; i++) {
        soma += leiturasPot[i];
    }
    return (int)(soma / TAMANHO_FILTRO);
}

bool verificarMaoPerto() {
    digitalWrite(PINO_TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(PINO_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PINO_TRIG, LOW);
    
    long tempoEco = pulseIn(PINO_ECHO, HIGH, 15000);
    if (tempoEco == 0) return false;
    
    int distancia = tempoEco * 0.034 / 2;
    return (distancia > 0 && distancia < DISTANCIA_MAO);
}

// ==========================================
// 4. MENSAGENS
// ==========================================

void somAcerto() {
    digitalWrite(PINO_LED_VERDE, HIGH);
    tone(PINO_BUZZER, 1000, 150);
    delay(150);
    tone(PINO_BUZZER, 1500, 200);
    delay(200);
    digitalWrite(PINO_LED_VERDE, LOW);
} 

void somErro() {
    digitalWrite(PINO_LED_VERMELHO, HIGH);
    tone(PINO_BUZZER, 300, 500);
    delay(500);
    digitalWrite(PINO_LED_VERMELHO, LOW);
}

void mostraNoLcd(const char* linha1, const char* linha2) {
    // Atualiza o display LCD físico
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(linha1);
    lcd.setCursor(0, 1);
    lcd.print(linha2);

    // Envia formatado para o Monitor Serial
    Serial.println("[LCD]----------------");
    Serial.print("  L1: ");
    Serial.println(linha1);
    if (linha2 && strlen(linha2) > 0) { 
        Serial.print("  L2: ");
        Serial.println(linha2);
    }
    Serial.println("---------------------");
}

// Função que trava o jogo até que qualquer botão seja pressionado
void esperarCliqueBotao() {
    unsigned long ultimoPisca = 0;
    bool estadoLed = LOW;
    
    while (digitalRead(BOTAO_AMARELO) == HIGH && 
           digitalRead(BOTAO_AZUL) == HIGH && 
           digitalRead(BOTAO_VERDE) == HIGH && 
           digitalRead(BOTAO_VERMELHO) == HIGH) {
        
        if (millis() - ultimoPisca > 500) {
            estadoLed = !estadoLed;
            digitalWrite(PINO_LED_VERDE, estadoLed);
            ultimoPisca = millis();
        }
        delay(10); 
    }
    digitalWrite(PINO_LED_VERDE, LOW); 
    
    tone(PINO_BUZZER, 1200, 100);
    delay(100);
    tone(PINO_BUZZER, 1800, 150);
    delay(200);
}

void enviarDadosSerial(int scoreAtual, unsigned long respostaMilis, bool acertou) {
    Serial.println("--- RODADA FINALIZADA ---");
    Serial.print("Resultado: ");
    Serial.println(acertou ? "ACERTOU" : "ERROU / TIMEOUT");
    Serial.print("Pontuacao: ");
    Serial.println(scoreAtual);
    Serial.print("Tempo: ");
    Serial.print(respostaMilis);
    Serial.println(" ms");
}

// ==========================================
// 5. FLUXO PRINCIPAL
// ==========================================

void setup() {
    Serial.begin(9600); 
    
    pinMode(PINO_TRIG, OUTPUT);
    pinMode(PINO_ECHO, INPUT);
    pinMode(PINO_BUZZER, OUTPUT);
    pinMode(PINO_LED_VERDE, OUTPUT);
    pinMode(PINO_LED_VERMELHO, OUTPUT);
    
    pinMode(BOTAO_AMARELO, INPUT_PULLUP);
    pinMode(BOTAO_AZUL, INPUT_PULLUP);
    pinMode(BOTAO_VERDE, INPUT_PULLUP);
    pinMode(BOTAO_VERMELHO, INPUT_PULLUP);
    
    lcd.init(); 
    lcd.backlight();
    
    for(int i = 0; i < TAMANHO_FILTRO; i++) {
        leiturasPot[i] = analogRead(PINO_POT);
    }
    randomSeed(analogRead(A1)); 
    
    mostraNoLcd(" Aperte um botao", "  para comecar!");
    esperarCliqueBotao();
    
    mostraNoLcd("Jogo Bop-It", "Prepare-se...");
    delay(2000);
}

void loop() {
    comandoAtual = (Acoes)random(0, TOTAL_ACOES);
    String stringPontos = "Pontos: " + String(pontos);
    mostraNoLcd(textosAcoes[comandoAtual], stringPontos.c_str());
    
    for(int i = 0; i < TAMANHO_FILTRO; i++) {
        lerPotFiltrado();
        delay(10);
    }
    int potInicial = lerPotFiltrado();
    
    while (digitalRead(BOTAO_AMARELO) == LOW || 
           digitalRead(BOTAO_AZUL) == LOW || 
           digitalRead(BOTAO_VERDE) == LOW || 
           digitalRead(BOTAO_VERMELHO) == LOW) {
        delay(10); 
    }

    unsigned long tempoInicial = millis();
    bool acaoCorreta = false;
    bool acaoErrada = false;
    unsigned long tempoResposta = 0;

    while ((millis() - tempoInicial) < tempoJogo) {
        tempoResposta = millis() - tempoInicial;
        
        // Verificação do Potenciômetro
        int potAtual = lerPotFiltrado();
        if (abs(potAtual - potInicial) > 150) {
            if (comandoAtual == GIRAR) acaoCorreta = true;
            else acaoErrada = true;
            break;
        }
        
        // Verificação do Ultrassônico
        if (verificarMaoPerto()) {
            if (comandoAtual == PASSAR_MAO) acaoCorreta = true;
            else acaoErrada = true;
            break;
        }
        
        // Verificação dos Botões
        if (digitalRead(BOTAO_AMARELO) == LOW) {
            if (comandoAtual == APERTAR_AMARELO) acaoCorreta = true;
            else acaoErrada = true;
            break;
        }
        if (digitalRead(BOTAO_AZUL) == LOW) {
            if (comandoAtual == APERTAR_AZUL) acaoCorreta = true;
            else acaoErrada = true;
            break;
        }
        if (digitalRead(BOTAO_VERDE) == LOW) {
            if (comandoAtual == APERTAR_VERDE) acaoCorreta = true;
            else acaoErrada = true;
            break;
        }
        if (digitalRead(BOTAO_VERMELHO) == LOW) {
            if (comandoAtual == APERTAR_VERMELHO) acaoCorreta = true;
            else acaoErrada = true;
            break;
        }
        
        delay(30);
    }

    if (acaoCorreta && !acaoErrada) {
        pontos++;
        somAcerto();
        enviarDadosSerial(pontos, tempoResposta, true);
        
        if (tempoJogo > TEMPO_LIMITE) {
            tempoJogo -= DESCONTO_TEMPO;
        }
        delay(800); 
    } 
    else {
        somErro();
        enviarDadosSerial(pontos, tempoResposta, false);
        
        String totalPontos = "Total: " + String(pontos) + " pts";
        mostraNoLcd("FIM DE JOGO!", totalPontos.c_str());
        delay(4000);
        
        mostraNoLcd("Aperte p/ Recomecar", totalPontos.c_str());
        
        delay(500); 
        esperarCliqueBotao();
        
        pontos = 0;
        tempoJogo = TEMPO_INICIAL;
        mostraNoLcd("Reiniciando...", "");
        delay(1500);
    }
}
