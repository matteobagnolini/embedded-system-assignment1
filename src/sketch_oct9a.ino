#define SLEEP 10000
#define MIN_VISIBLE 3000
#define T1 10000

void setup() {
  // put your setup code here, to run once:
  level = PREPARATION;
  game_num = rand() % 16;
}

float F = 0;
int t = T1;
int game_num;
int score;
bool correct = false;
int start_time;
int scored = false;

enum FASE {
  SLEEP,
  PREPARATION,
  GAME,
  POINT_CHANGE,
  GAME_OVER
} level;

void loop() {
  // put your main code here, to run repeatedly:
  switch(level):
  case GAME: {
    elapsed = time_updater(elapsed);
    correct = number_check();
    if(correct){
      next_turn(/* &game_num, &score*/); 
    }
    if(!correct && elapsed > t)
      game_over();
  }
  break;
  case POINT_CHANGE:{
    if (millis() - start_time > MIN_VISIBLE !scored){
      scored = true;
      display_score();
    }
    if(millis() - start_time > 2 * MIN_VISIBLE){
      scored = false;
      level = GAME;
    }
  }
  break;
}

void next_turn(){
  display_good();
  correct = false;
  start_time = millis();
  t = t * (1 - F);
  game_num = rand() % 16;
  score++;
  led_off();
  display_number();
}
