#include <Base/AVR/ATMEGA8/GPIO.h>
#include <Base/AVR/ATMEGA8/Timer.h>

int min, sec, dynamic_pos;
enum State { Strike, Tick, Wait } state;
int on_button1, on_button2, on_button3, on_reset, dsec;

void get_input(void);
void set_output(void);
void update_time(int, int);
int choose_digit(void);
int seg_pat(int);

int main() {
  state = Wait;
  while (1) {
    get_input();
    if (state == Strike) {
      if (on_button3) {
        state = Wait;
        min = 0, sec = 0;
      }
    } else if (state == Tick && min == 0 && sec == 0) {
      state = Strike;
    } else if (state == Tick) {
      if (on_button3) {
        state = Wait;
      } else {
        int x = (min*60+sec)-dsec;
        update_time(x/60, x%60);
      }
    } else if (state == Wait) {
      if (on_button1) {
        update_time((min+1)%100, sec);
      } else if (on_button2) {
        update_time(min, (sec+1)%60);
      } else if (on_button3) {
        if (!(sec == 0 && min == 0)) {
          state = Tick;
        }
      } else if (on_reset) {
        min = 0, sec = 0;
      }
    }
    dynamic_pos = (dynamic_pos+1)%4;
    set_output();
  }
  return 0;
}

void get_input() {
  on_button1 = posEdgePB(0);
  on_button2 = posEdgePB(1);
  on_button3 = posEdgePB(2);
  on_reset = pinB(0) && pinB(1);
  dsec = get_dsec(1024);
}

void set_output() {
  portB(6, state != Strike);
  portCs(0b00001111);
  if (state == Strike) {
    portDs(seg_pat(0));
  } else if (state == Wait) {
    portDs(seg_pat(choose_digit()));
  } else if (state == Tick) {
    portDs(seg_pat(choose_digit()));
  }
  portCs((1 << dynamic_pos) ^ 0b00001111);
}

int seg_pat(int digit) {
  switch (digit) {
    case 0: return 0b01111110;
    case 1: return 0b00010010;
    case 2: return 0b01011101;
    case 3: return 0b01010111;
    case 4: return 0b00110011;
    case 5: return 0b01100111;
    case 6: return 0b01101111;
    case 7: return 0b01010010;
    case 8: return 0b01111111;
    case 9: return 0b01110111;
  }
  return 0;
}

int choose_digit() {
  switch (dynamic_pos) {
    case 0: return min/10;
    case 1: return min%10;
    case 2: return sec/10;
    case 3: return sec%10;
  }
  return 0;
}

void update_time(int new_min, int new_sec) {
  min = new_min, sec = new_sec;
}
