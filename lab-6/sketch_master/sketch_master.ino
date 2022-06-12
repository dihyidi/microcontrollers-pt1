bool isAddress = true;
bool isCommand = false;
byte command;

void setWriteModeRS485() {
  byte port = PORTD;
  PORTD |= 1 << PD1;
  delay(1);
}

// переривання по завершенню передачі
ISR(USART1_TX_vect)
{
  PORTD &= ~(1 << PD1); 
}

void setup() {
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT); // зазвичай 13й пін, який має вбудований ледик
  digitalWrite(LED_BUILTIN, HIGH);

  DDRD |= 1 << PD1;
  PORTD &= ~(1 << PD1);

  Serial.begin(19200); // в межах RS-485
  Serial1.begin(14000, SERIAL_8N1); // між МК і ПК
    // задає біт UCSZ02, який описаний тут: "Біти UCSZn2, поєднані з бітом UCSZn1: 0 в UCSRnC, встановлюють
  // кількість бітів даних (Символ SiZe) у кадрі. використання приймача та передавача ". Це в основному
  // дозволяє вам вибрати 9-бітовий серійний номер (8 біт є більш поширеним)
  // бит TXCIE0 регистра UCSR0B - разрешение прерывания при завершении передачи если установлен в 1.
  UCSR1B |= (1 << UCSZ12) | (1 << TXCIE1);
}

void loop() {  
  // маю тут ці дві змінні isAddress i isCommand, які мені дозволяють фактично не просто спамити даними
  // в форму, а надсилати їх тільки якщо буде така команда, тобто якщо я натискаю на якийсь зі слейвів
  // в формі, тоді в мене ці дані йдуть на форму, а так то я просто їх неперервно приймаю
  if (Serial.available()) { // якщо нам в буфер прийшли якісь дані зі слейвів
    byte inByte = Serial.read(); // то вичитуємо їх по рядочку
    if (isAddress) {
      setWriteModeRS485(); // включаємо режим запису на RS-485, тобто це ми дозволяємо з нашого мікроконтролера записати дані в форму
      UCSR1B |= 1 << TXB81; // 8й розряд даних
      Serial1.write(inByte);
      isAddress = false;
      isCommand = true;
    } else if (isCommand) {
      command = inByte;
      isCommand = false;
      setWriteModeRS485();
      UCSR1B &= ~(1 << TXB81); // після запису даних у буфер передачі необхідно
                               // надати дозвіл на переривання для події спорожнення буфера UDR.
      Serial1.write(inByte);
      if (command == 0xB1) {
        isAddress = true;
      }
    } else {
      isAddress = true;
      setWriteModeRS485();
      UCSR1B &= ~(1 << TXB81);
      Serial1.write(inByte);
    }
  }

  if (Serial1.available()) { // якщо нам шось надходить з форми, то ми це теж обробляємо.
    byte inByte1 = Serial1.read();
    Serial.write(inByte1);
  }
}
