#define F_CPU = 12000000
#define DIGIT PORTA
#define SEGMENT PORTC
#define SEC_ARR_MAX 10
#define BUZZER 38
#define BEEP_DELAY 400

unsigned long savedTimeArr[SEC_ARR_MAX];
unsigned int currentSizeOfSavedTimeArr;
double seconds;
unsigned char printSegment = 0;
boolean isStartTimer1;
unsigned char isButtonPressed;

void printTime(unsigned long paramSeconds);
void buttonPressed();
void firstButtonTrigger();
void secondButtonTrigger();
void thirdButtonTrigger();
void fourthButtonTrigger();
inline void shortBeep();
inline void longBeep();

unsigned int numbers[10] = {
    0x3f,
    0x6,
    0x5b,
    0x4f,
    0x66,
    0x6d,
    0x7d,
    0x7,
    0x7f,
    0x6f};
unsigned int seg[6] = {
    22,
    23,
    24,
    25,
    26,
    27};

void printTime(unsigned long paramSeconds)
{
  unsigned long hr = (unsigned int)(paramSeconds / 3600);
  unsigned long mn = ((unsigned int)(paramSeconds / 60)) % 60;
  unsigned long sc = (unsigned int)(paramSeconds % 60);

  if (printSegment == 5)
  {
    printSegment = 0;
  }
  else
  {
    printSegment++;
  }

  switch (printSegment)
  {
  case 0:
    SEGMENT = 0b00000001;
    DIGIT = numbers[hr / 10];
    break;
  case 1:
    SEGMENT = 0b00000010;
    DIGIT = numbers[hr % 10];
    DIGIT |= (1 << 7);
    break;
  case 2:
    SEGMENT = 0b00000100;
    DIGIT = numbers[mn / 10];
    break;
  case 3:
    SEGMENT = 0b00001000;
    DIGIT = numbers[mn % 10];
    DIGIT |= (1 << 7);
    break;
  case 4:
    SEGMENT = 0b00010000;
    DIGIT = numbers[sc / 10];
    break;
  case 5:
    SEGMENT = 0b00100000;
    DIGIT = numbers[sc % 10];
    break;
  }
}

ISR(TIMER1_COMPA_vect)
{
  if (isStartTimer1)
  {
    seconds += 1;
  }
}

ISR(TIMER2_COMPA_vect)
{
  printTime(seconds);
}

void buttonPressed()
{
  if (PINB != 0b00001111)
  {
    if (isButtonPressed == 1)
    {
      return;
    }

    isButtonPressed = 1;

    switch (PINB)
    {
    case 0b00001110:
      firstButtonTrigger();
      break;

    case 0b00001101:
      secondButtonTrigger();
      break;

    case 0b00001011:
      thirdButtonTrigger();
      break;

    case 0b00000111:
      fourthButtonTrigger();
      break;

    default:
      break;
    }
  }
  else
  {
    isButtonPressed = 0;
  }
}

void firstButtonTrigger()
{
  if (isStartTimer1 == 0 && seconds == 0)
  {
    isStartTimer1 = 1;
  }
  else if (isStartTimer1 == 1 && seconds != 0)
  {
    isStartTimer1 = 0;
  }
  else
  {
    seconds = 0;
  }
}

void secondButtonTrigger()
{
  if (currentSizeOfSavedTimeArr < SEC_ARR_MAX)
  {
    savedTimeArr[currentSizeOfSavedTimeArr] = seconds;
    currentSizeOfSavedTimeArr++;
  }
}

void thirdButtonTrigger()
{
  for (int i = 0; i < SEC_ARR_MAX; i++)
  {
    savedTimeArr[i] = 0;
  }
  currentSizeOfSavedTimeArr = 0;
}

void fourthButtonTrigger()
{
  if (isStartTimer1 == 0)
  {
    unsigned long tempSeconds = seconds;
    for (int i = 0; i < currentSizeOfSavedTimeArr; i++)
    {
      seconds = savedTimeArr[i];
      _delay_ms(350);
    }
    seconds = tempSeconds;
  }
}

inline void shortBeep()
{
  digitalWrite(BUZZER, HIGH);
  delay(BEEP_DELAY);
  digitalWrite(BUZZER, LOW);
}

inline void longBeep()
{
  digitalWrite(BUZZER, HIGH);
  delay(BEEP_DELAY * 2);
  digitalWrite(BUZZER, LOW);
}

void setup()
{
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  // digit
  DDRC = 0b11111111;
  // segment
  DDRA = 0b00111111;
  // buttons
  DDRB = DDRB & ~((1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3));
  // Enable the pullups
  PORTB = PORTB | ((1 << PORTB0) | (1 << PORTB1) | (1 << PORTB2) | (1 << PORTB3));
  // initialize variables
  seconds = 0;
  isStartTimer1 = true;
  printSegment = 0;
  isButtonPressed = 0;
  // initialize Timers
  // Timer2 CTC & /1024 at Asynchronous mode with compare interrupt
  noInterrupts();

  TCCR1A = 0x00;
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC mode & Prescaler @ 1024
  TIMSK1 = (1 << OCIE1A);
  OCR1A = 0xD239; // compare value = 1 sec (12MHz AVR)

  TCCR2A = 0x00;
  TCCR2B = (1 << WGM22) | (1 << CS22) | (1 << CS20); // CTC mode & Prescaler @ 1024
  TIMSK2 = (1 << OCIE2A);
  OCR2A = 0x0014; // compare value for 500 Hz (12MHz AVR)

  interrupts();
}

void loop()
{
  buttonPressed();
  //   if ((int)seconds % 60 == 0)
  //   {
  //     longBeep();
  //   }
}