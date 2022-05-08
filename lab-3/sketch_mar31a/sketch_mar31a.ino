#include <Keypad.h>
#include <LiquidCrystal.h>

char KP_BUTTONS[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte kp_row_pins[4] = {37, 36, 35, 34};
byte kp_col_pins[4] = {33, 32, 31, 30};
Keypad my_keypad = Keypad(makeKeymap(KP_BUTTONS), kp_row_pins, kp_col_pins, 4, 4);

const int rs = 41, rw = 40, en = 39, d0 = 22, d1 = 23, d2 = 24, d3 = 25, d4 = 26, d5 = 27, d6 = 28, d7 = 29;
LiquidCrystal lcd(rs, rw, en, d0, d1, d2, d3, d4, d5, d6, d7);

const int BUZZER_PIN = 8;

bool is_running = true;
bool is_clear = false;
bool show_T4 = true;
bool is_A_mode = false;
bool is_B_input_mode = false;

int number_from_input = 0;

const PROGMEM char sixty[60][3] =
    {
        {"00"}, {"01"}, {"02"}, {"03"}, {"04"}, {"05"}, {"06"}, {"07"}, {"08"}, {"09"}, {"10"}, {"11"}, {"12"}, {"13"}, {"14"}, {"15"}, {"16"}, {"17"}, {"18"}, {"19"}, {"20"}, {"21"}, {"22"}, {"23"}, {"24"}, {"25"}, {"26"}, {"27"}, {"28"}, {"29"}, {"30"}, {"31"}, {"32"}, {"33"}, {"34"}, {"35"}, {"36"}, {"37"}, {"38"}, {"39"}, {"40"}, {"41"}, {"42"}, {"43"}, {"44"}, {"45"}, {"46"}, {"47"}, {"48"}, {"49"}, {"50"}, {"51"}, {"52"}, {"53"}, {"54"}, {"55"}, {"56"}, {"57"}, {"58"}, {"59"}};

struct Time
{
  unsigned char second, minute, hour;
};

Time time_buffer[60];
int curr_pos = 0;
int millis_start = 0;
int curr_memory_pos = 0;
Time T4 = {0, 0, 0};

void show_next_from_memory()
{
  if (millis() - millis_start > 1000)
  {
    if (show_T4)
    {
      curr_memory_pos = 0;
    }
    else
    {
      if (curr_memory_pos >= curr_pos - 1)
      {
        curr_memory_pos = 0;
      }
      else
        ++curr_memory_pos;
      lcd.setCursor(8, 0);
      LCD_WriteStrPROGMEM(sixty[time_buffer[curr_memory_pos].hour], 2);
      lcd.write(':');
      LCD_WriteStrPROGMEM(sixty[time_buffer[curr_memory_pos].minute], 2);
      lcd.write(':');
      LCD_WriteStrPROGMEM(sixty[time_buffer[curr_memory_pos].second], 2);
      lcd.write(" ");
      millis_start = millis();
    }
  }
}

void show_next_from_memory_with_idx()
{
  if (millis() - millis_start > 1000)
  {
    if (show_T4)
    {
      curr_memory_pos = 0;
    }
    else
    {
      if (curr_memory_pos >= curr_pos - 1)
      {
        curr_memory_pos = 0;
      }
      else
        ++curr_memory_pos;
      lcd.setCursor(14, 1);
      LCD_WriteStrPROGMEM(sixty[curr_memory_pos + 1], 2);
      lcd.write(" ");
      lcd.setCursor(8, 0);
      LCD_WriteStrPROGMEM(sixty[time_buffer[curr_memory_pos].hour], 2);
      lcd.write(':');
      LCD_WriteStrPROGMEM(sixty[time_buffer[curr_memory_pos].minute], 2);
      lcd.write(':');
      LCD_WriteStrPROGMEM(sixty[time_buffer[curr_memory_pos].second], 2);
      lcd.write(" ");
      millis_start = millis();
    }
  }
}

void LCD_WriteStrPROGMEM(char *str, int n)
{
  for (int i = 0; i < n; i++)
    lcd.print((char)pgm_read_byte(&(str[i])));
}

ISR(TIMER4_COMPA_vect)
{
  if (is_clear)
  {
    T4.hour = 0;
    T4.minute = 0;
    T4.second = 0;
    is_clear = false;
  }
  if (is_running)
  {
    if (++T4.second == 60)
    {
      T4.second = 0;
      if (++T4.minute == 60)
      {
        T4.minute = 0;
        if (++T4.hour == 24)
          T4.hour = 0;
      }
    }
  }
  if (show_T4)
  {
    lcd.setCursor(8, 0);
    LCD_WriteStrPROGMEM(sixty[T4.hour], 2);
    lcd.write(':');
    LCD_WriteStrPROGMEM(sixty[T4.minute], 2);
    lcd.write(':');
    LCD_WriteStrPROGMEM(sixty[T4.second], 2);
    lcd.write(" ");
  }
}

void react(KeypadEvent e)
{
  if (my_keypad.getState() == PRESSED)
  {
    lcd.clear();
    if (e == 'D')
    {
      is_running = !is_running;
      lcd.setCursor(0, 1);
      digitalWrite(BUZZER_PIN, HIGH);
      lcd.write("D");
      delay(300);
      digitalWrite(BUZZER_PIN, LOW);
      lcd.setCursor(0, 1);
      lcd.write(" ");
    }
    else if (e == 'C')
    {
      is_clear = true;
      is_running = false;
      lcd.setCursor(0, 1);
      digitalWrite(BUZZER_PIN, HIGH);
      lcd.write("C");
      delay(300);
      digitalWrite(BUZZER_PIN, LOW);
      lcd.setCursor(0, 1);
      lcd.write(" ");
    }
    else if (e == 'A' && !is_A_mode && !is_B_input_mode)
    {
      show_T4 = false;
      is_A_mode = true;
      millis_start = millis();
      curr_memory_pos = 0;
      show_next_from_memory();
      lcd.setCursor(0, 1);
      digitalWrite(BUZZER_PIN, HIGH);
      lcd.write("A");
      delay(300);
      digitalWrite(BUZZER_PIN, LOW);
      lcd.setCursor(0, 1);
      lcd.write(" ");
    }
    else if (e == 'A' && is_A_mode)
    {
      show_T4 = true;
      is_A_mode = false;
      lcd.setCursor(0, 1);
      lcd.write(" ");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(300);
      digitalWrite(BUZZER_PIN, LOW);
    }
    else if (e == 'B' && !is_B_input_mode && !is_A_mode)
    {
      show_T4 = false;
      is_B_input_mode = true;
      millis_start = millis();
      curr_memory_pos = 0;
      show_next_from_memory_with_idx();
      lcd.setCursor(0, 1);
      lcd.write("B");
      digitalWrite(BUZZER_PIN, HIGH);
      delay(300);
      digitalWrite(BUZZER_PIN, LOW);
      lcd.setCursor(0, 1);
      lcd.write(" ");
    }
    else if (e == '#' && !is_B_input_mode)
    {
      if (curr_pos >= 60)
      {
        lcd.setCursor(0, 1);
        lcd.write("IDXERROR");
        digitalWrite(BUZZER_PIN, HIGH);
        delay(300);
        digitalWrite(BUZZER_PIN, LOW);
        delay(100);
        digitalWrite(BUZZER_PIN, HIGH);
        delay(300);
        digitalWrite(BUZZER_PIN, LOW);
        lcd.setCursor(0, 1);
        lcd.write("        ");
      }
      else
      {
        time_buffer[curr_pos] = T4;
        curr_pos++;
        lcd.setCursor(0, 1);
        digitalWrite(BUZZER_PIN, HIGH);
        lcd.write("#");
        delay(300);
        digitalWrite(BUZZER_PIN, LOW);
        lcd.setCursor(0, 1);
        lcd.write(" ");
      }
    }
    else if (e == '*')
    {
      memset(time_buffer, '\0', curr_pos * sizeof(Time));
      curr_pos = 0;
      lcd.setCursor(0, 1);
      digitalWrite(BUZZER_PIN, HIGH);
      lcd.write("*");
      delay(600);
      digitalWrite(BUZZER_PIN, LOW);
      lcd.setCursor(0, 1);
      lcd.write(" ");
    }
    else
    {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(300);
      digitalWrite(BUZZER_PIN, LOW);
      switch (e)
      {
      case 'A':
        break;
      case 'B':
        break;
      case '#':
      {
        if (number_from_input > 0 && number_from_input < curr_pos + 1)
        {
          digitalWrite(BUZZER_PIN, HIGH);
          delay(300);
          digitalWrite(BUZZER_PIN, LOW);
          delay(100);
          digitalWrite(BUZZER_PIN, HIGH);
          delay(300);
          digitalWrite(BUZZER_PIN, LOW);
          T4.hour = time_buffer[number_from_input - 1].hour;
          T4.minute = time_buffer[number_from_input - 1].minute;
          T4.second = time_buffer[number_from_input - 1].second;
          is_B_input_mode = false;
          show_T4 = true;
          number_from_input = 0;
          lcd.setCursor(0, 1);
          lcd.write("UPDATED");
          delay(300);
          lcd.setCursor(0, 1);
          lcd.write("       ");
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.write("INVALID");
          delay(100);
          lcd.clear();
          number_from_input = 0;
        }
        break;
      }
      default:
      {
        number_from_input *= 10;
        number_from_input += (int(e) - 48);
        lcd.setCursor(0, 1);
        LCD_WriteStrPROGMEM(sixty[number_from_input], 2);
        break;
      }
      }
    }
  }
}

void setup()
{
  noInterrupts();
  TCCR4A = 0x00;
  TCCR4B = (1 << WGM42) | (1 << CS42) | (1 << CS40);
  TIMSK4 = (1 << OCIE4A);
  OCR4A = 0x3D08; // compare value = 1 sec (16MHz AVR)

  // LCD's number of columns and row
  lcd.begin(16, 2);
  interrupts();

  // Buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  my_keypad.addEventListener(react);
}

void loop()
{
  lcd.setCursor(0, 0);
  if (curr_pos > 0)
  {
    lcd.write("M");
    LCD_WriteStrPROGMEM(sixty[curr_pos], 2);
  }
  if (T4.minute > 0 && T4.second == 0)
  {
    lcd.setCursor(0, 1);
    lcd.write("MIN");
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
    lcd.setCursor(0, 1);
    lcd.write("   ");
  }
  if (is_A_mode)
  {
    show_next_from_memory();
  }
  else if (is_B_input_mode)
  {
    show_next_from_memory_with_idx();
  }
  lcd.setCursor(0, 1);
  char a = my_keypad.getKey();
}
