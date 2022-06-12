#define K 0x01
#define O 0x02
#define V 0x03
#define A 0x04
#define L 0x05
#define D 0x6A
#define I 0x07
#define N 0x08
#define M 0x09
#define Y 0x0A

#define _ 0x00

byte res;
bool isAddress = true;
byte address;
bool isCommand = false;
byte command;
byte data;

void setWriteModeRS485()
{
  PORTD |= 1 << PD2;
  delay(1);
}

// переривання по завершенню передачі
ISR(USART_TX_vect)
{
  PORTD &= ~(1 << PD2);
}

// функція шоб віддавати дані на мій майстер
int writeData()
{
  byte message[24] = {
      K, O, V, A, L, _, // Koval
      D, I, A, N, A, _, // Diana
      M, Y, K, O, L, A, I, V, N, A, // Mykolaivna
      _, _};

  byte reflected[22];
//  for (int i = 0; i < 22; i++)
//  {
//    reflected[i] = ReverseByte(message[i]);
//  }

  unsigned short checkSum = Compute_CRC16(reflected);
  byte firstByteOfCheckSum = (checkSum >> 8) & 0xFF;
  byte secondByteOfCheckSum = checkSum & 0xFF;

  message[22] = firstByteOfCheckSum;
  message[23] = secondByteOfCheckSum;

  // спотворення - в нас є п'ять різних записів.
  // перший слейв передає прізвище, ім'я, по батькові
  // і в мене там має бути спотворення в другому і п'ятому кейсі
  // фактично, я просто виловлюю той момент і виловлюю конкретний біт і виконую побітовий зсув вліво
  for (int k = 0; k < 5; k++)
  {
    for (int i = 0; i < 24; i++)
    {
      if (k == 1 && i == 2)
      {
        res = message[i];
        Serial.write((1 << res));
      }
      else if (k == 4 && i == 6)
      {
        res = message[i];
        Serial.write((0 << res) | (2 << res) | (5 << res));
      }
      else
      {
        Serial.write(message[i]);
      }
    }
  }
}

void setup()
{
  delay(1000);
  DDRD = 0b00000111;
  PORTD = 0b11111000;

  Serial.begin(9600, SERIAL_8N1);
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0);

  delay(1);

  address = 0x2B;
}

void loop()
{
  if (Serial.available())
  {
    byte inByte = Serial.read();
    if (isAddress)
    {
      if (address == inByte)
      {
        isAddress = false;
        isCommand = true;
        UCSR0A &= ~(1 << MPCM0);
      }
    }
    else if (isCommand)
    {
      command = inByte;
      isCommand = false;
      if (command = 0xB1)
      {
        isAddress = true;
        setWriteModeRS485();
        writeData();
      }
    }
  }
}

///////////////////////////////////////////////////////
unsigned short Compute_CRC16(byte* bytes)
{
    const unsigned short generator = 0x1021; /* divisor is 16bit */
    unsigned short crc = 0x89EC; /* CRC value is 16bit */

    for (int i=0; i<sizeof bytes/sizeof bytes[0]; i++)
    {
        unsigned short b = Reflect16(bytes[i]);
        crc ^= (unsigned short) (b << 8); /* move byte into MSB of 16bit CRC */

        for (int i = 0; i < 8; i++)
        {
            if ((crc & 0x8000) != 0) /* test for MSB = bit 15 */
            {
                crc = (unsigned short)((crc << 1) ^ generator);
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    crc = Reflect16(crc);
    return crc;
} 

unsigned short Reflect16(unsigned short val)
{
    unsigned short resVal = 0;

    for (int i = 0; i < 16; i++)
    {
        if ((val & (1 << i)) != 0)
        {
            resVal |= (unsigned short)(1 << (15 - i));
        }
    }

    return resVal;
} 

byte ReverseByte(byte b)
{
  int a = 0;
  for (int i = 0; i < 8; i++)
  {
    if ((b & (1 << i)) != 0)
    {
      a |= 1 << (7 - i);
    }
  }
  return (byte)a;
}
