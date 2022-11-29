//Definição das macros utilizadas ao longo de todo o programa

//Estados que serão usados na implementação da maquina de estados
#define IDLE 0
#define CONT 1
#define SPLIT 2
#define RESET 3
#define END 4


//Tempo maximo de contagem
#define MAXTIME 43200 //12 horas = 720 minutos = 43200 segundos
#define ARRAYSIZE 0x0F

/*Mascaras para manipulação de bits 
SFR stands for special function register e p stands for position*/
#define SET(SFR, p) (SFR |= 1 << p) 
#define CLEAR(SFR, p) (SFR &= ~(1 << p))
#define TOOGLE(SFR, p) (SFR ^= 1 << p)
#define IS_SET(SFR, p) (SFR & 1 << p)
#define SHIFTLEFT(SFR, p) (SFR << p)
#define SHIFTRIGTH(SFR, p) (SFR >> p)
#define FULL(head, tail) ((head & ARRAYSIZE) == (tail & ARRAYSIZE) && head > tail)
#define EMPTY(head, tail) ((head & ARRAYSIZE) == (tail & ARRAYSIZE) && head == tail)
