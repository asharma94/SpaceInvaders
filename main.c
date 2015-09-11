// Lab10 Arcade Style Game
// main.c
// Runs on LM4F120 or TM4C123
// Put your name here or look very silly
// Put the date here or look very silly

// Graphic display on Kentec EB-LM4F120-L35
// Sound output to DAC (Lab 7)
// Analog Input connected to PE2=ADC1, 1-D joystick (Lab 8)
// optional: UART link to second board for two-player mode
// Switch input

#include "../inc/tm4c123gh6pm.h"
#include "SSD2119.h"
#include "PLL.h"
#include "random.h"
#include "SpaceArt.h"     // space invader art
#include "sounds.h"       // audio waveforms

void EnableInterrupts(void);
void Timer2_Init(unsigned long period);
void Timer3_Init(unsigned long period);
unsigned long TimerCount;
unsigned long Semaphore;
void InitEnemy(void);
void InitBunker(void);
void InitPlayer(void);
void InitMissile(void);
void InitLaser(void);
void Update(void);
void DrawBuffer1(void);
void MoveWorld(void);
void EnemyCol(void);
void MissileStatus(void);

struct State {
  unsigned long x;      // x coordinate
  unsigned long y;      // y coordinate
  const unsigned char *image; // ptr->image
  long life;            // 0=dead, 1=alive
};         
typedef struct State STyp;
STyp Enemy[15];
STyp Bunker[4];
STyp Player;
STyp Missile;
STyp Laser;

void InitEnemy(void){ int i;
  for(i=0;i<5;i++){
    Enemy[i].x = 50*i;
    Enemy[i].y = 30;
    Enemy[i].image = ATMA;
    Enemy[i].life = 3;
		}
	for(i=5;i<10;i++){
    Enemy[i].x = 50*(i-5);
    Enemy[i].y = 55;
    Enemy[i].image = OUA;
    Enemy[i].life = 2;
		}
	for(i=10;i<15;i++){
    Enemy[i].x = 50*(i-10);
    Enemy[i].y = 80;
    Enemy[i].image = TTechA;
    Enemy[i].life = 1;
		} 
}

void InitBunker(void){ int i;
  for(i=0;i<4;i++){
    Bunker[i].x = (80*i)+20;
    Bunker[i].y = 190;
    Bunker[i].image = Bunker0;
    Bunker[i].life = 3;
}
}

void InitPlayer(void){
    Player.x = 0;
    Player.y = 210;
    Player.image = UTShip;
    Player.life = 3;
}

void InitMissile(void){
		Missile.x = 0;
		Missile.y = 10;
		Missile.image = missile0;
		Missile.life = 1;	
}

void InitLaser(void){
		Laser.x = 0;
		Laser.y = 100;
		Laser.image = laser0;
		Laser.life = 1;
}

char Fireflag;

int main(void){unsigned short score;
  PLL_Init();  // Set the clocking to run at 80MHz from the PLL.
  LCD_Init();  // Initialize LCD
  LCD_Goto(0,0);
	score =0;
	long v2;
  LCD_SetTextColor(255,0,0); // yellow= red+green, no blue
  printf("score: %d", score);
  LCD_DrawLine(10,16,310,16,BURNTORANGE);
  Timer2_Init(4000000); // 20 times a second
	Timer3_Init(7256);	// 11.025 KHz
	InitEnemy();
	InitBunker();
	InitPlayer();
	InitMissile();
	InitLaser();
	Random_Init(1);
  EnableInterrupts();
	Fireflag=1;
	
while(1){
	if(Semaphore){
		DrawBuffer1();		
		Semaphore = 0;
    }
  }
}

//Timer Handler, Most processing occurs here
void Timer2A_Handler(void){ 
  TIMER2_ICR_R = 0x00000001;   // acknowledge timer2A timeout
	MoveWorld();
	Update();
	MissileStatus();
  TimerCount++;
  Semaphore = 1; // trigger buffer
}






//Draw Screen Status
void DrawBuffer1(void){int j;
	for(j=0; j<15; j++){
		if(Enemy[j].life != 0){
			LCD_DrawBMP(Enemy[j].image, Enemy[j].x, Enemy[j].y);
		}
	}	
	for(j=0; j<4; j++){
		LCD_DrawBMP(Bunker[j].image, Bunker[j].x, Bunker[j].y);
	}
	if(Missile.life){		
		LCD_DrawBMP(Missile.image, Missile.x, Missile.y);
	}	
	LCD_DrawBMP(Player.image, Player.x, Player.y);
}

// Update sprite images
void Update(void){int j;
	EnemyCol();	
	 for(j=0;j<5;j++){
		if(Enemy[j].life == 0){
			switch(Enemy[j].life){
				case 0:
					Enemy[j].image = missile1;
				case 1:
					Enemy[j].image = ATMB;
			}
		}
	}
	for(j=5;j<10;j++){
			switch(Enemy[j].life){
				case 0:
					Enemy[j].image = missile1;
				case 1:
					Enemy[j].image = OUB;			
			}
	}		
	for(j=10;j<15;j++){
			switch(Enemy[j].life){
				case 0:
					Enemy[j].image = missile1;
				case 1:
					Enemy[j].image = TTechB;
			}
	}		
	for(j=0; j<4; j++){
			switch(Bunker[j].life){
				case 0:
					Bunker[j].image = missile1;
				case 1:
					Bunker[j].image = Bunker2;
				case 2:
					Bunker[j].image = Bunker1;
		}
	}
	if(Missile.y <= 20 || Missile.life==0){
		Missile.life = 0;
		LCD_DrawBMP(missile1, Missile.x, Missile.y);
	}
}

//Move Sprites
void MoveWorld(void){ int j;
	for(j=0; j<15; j++){
		Enemy[j].x = (Enemy[j].x + 3)%320;
	}
	if (Missile.life){
		Missile.y -= 4;
	}
	if(Laser.y < Player.y){
		
	}		
}
//Checks if a missile needs to be created
void MissileStatus(void){
	if(Fireflag && (!Missile.life)){
		Missile.x = (Player.x+16);
		Missile.y = Player.y;
		Missile.life=1;
	}
}
//Collision Check
void EnemyCol(void){int j;
	for(j=0;j<15;j++){
	if(Missile.life){
		if ((Missile.x - Enemy[j].x) < 35){
			if ((Missile.y - Enemy[j].y) < 30){
				if(Enemy[j].life>0){
					Missile.life = 0;
					LCD_DrawBMP(missile1, Missile.x, Missile.y);;
					Enemy[j].life--;
					if(Enemy[j].life==0){
						LCD_DrawFilledRect(Enemy[j].x,Enemy[j].y,32,20,BLACK);
					}	
				}	
			}	
		}
	}
	}
		for(j=0;j<4;j++){
	if(Missile.life){
		if ((Missile.x - Bunker[j].x) < 32){
			if ((Missile.y - Bunker[j].y) < 5){
				if(Bunker[j].life>0){
					Missile.life = 0;
					LCD_DrawBMP(missile1, Missile.x, Missile.y);;
					Bunker[j].life--;
					if(Bunker[j].life==0){
						LCD_DrawFilledRect(Bunker[j].x,Bunker[j].y,32,5,BLACK);
					}	
				}	
			}	
		}
	}
	}
}

void Timer2_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void Timer3_Init(unsigned long period){ 
  unsigned long volatile delay;
  SYSCTL_RCGCTIMER_R |= 0x08;   // 0) activate timer3
  delay = SYSCTL_RCGCTIMER_R;
  TimerCount = 0;
  Semaphore = 0;
  TIMER3_CTL_R = 0x00000000;    // 1) disable timer3A during setup
  TIMER3_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER3_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER3_TAILR_R = period-1;    // 4) reload value
  TIMER3_TAPR_R = 0;            // 5) bus clock resolution
  TIMER3_ICR_R = 0x00000001;    // 6) clear timer3A timeout flag
  TIMER3_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI8_R = (NVIC_PRI8_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN1_R = 1<<3;           // 9) enable IRQ 3 in NVIC
  TIMER3_CTL_R = 0x00000001;    // 10) enable timer3A
}
