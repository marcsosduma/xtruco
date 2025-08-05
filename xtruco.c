/*
//  Xtruco
//  Card Game
*/

#include "xbook.h"
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "truco.xbm"
#include "cards.h"
// xxd -i cards.bmp > cards.h  ---gerar o .h

#define       MAX_PIX      30
#define       TOTAL        42
#define	      BACK	   	   40
#define	      MAXIMO	   12
#define	      YES	   	   88
#define	      NO	       89
#define	      ERR_TRUCO	   666
#define 	  CARDS_WIDTH  80
#define  	  CARDS_HEIGHT 123
#define 	  WAIT_VIEW    15000
#define       NUM_CARDS    6

enum stp {STEP_OPEN_GAME,STEP_WAITING,STEP_NEW_GAME,STEP_DECK_CUTTING, STEP_INIT_GAME, STEP_PLAYER_GAME,STEP_PC_EVALUATE_GAME,STEP_PLAYER_ACCEPT_TRUCO,STEP_PC_ACCEPT_TRUCO,STEP_END_GAME,STEP_ANIMATE,STEP_END_APP};

typedef struct
    {
         Pixmap         bitmap;
         int            x, y;
         int            width, height;
    } BIT_STRUCT;

typedef struct
	{
		int	x, y;
		int	val_score;
	} TYPE_SCORE;

struct {
			int  what;
			int  card_state;
			int  val_score;
			int  X;
			int  Y;
		 } Table[9];

struct {
			int number_of_trucos; /* Not all of them are currently in use. */
			int first_truco;
			int second_truco;
			int third_truco;
			int lying;
			int time_seconds; 
			int analisys;
		 } info={ 0, 0, 0, 0, 0, 8, 1};

typedef struct {
    int x, y;
    int dx, dy;
} Card;


BIT_STRUCT    	the_bitmaps[ MAX_PIX ];
TYPE_SCORE	    score[ 2 ]={{0,0,0},{0,0,0}};	

int 			horiz, vert;
int 			State=STEP_OPEN_GAME, Message=0, Cards[40];
char   			Messa[80];
static unsigned long    black, white, green, blue, red, navy;
unsigned long green_back 		= 0x008000; // green
unsigned long green_highlight 	= 0x90EE90; // lightgreen
unsigned long green_shadow 		= 0x006400; // darkgreen
int font_height = 12;
Pixmap		bitmap1;
int			pc, you, Who_Play, You_Play, first, MyScore, YourScore;
int			alert, Sum_Val, ValGame, danger, Begining;
int			Play, Beginer, Who_Say_Truco,score1, score2;
int			SHOW=1;
int         pos_arrow_x=0, pos_arrow_y=0, i_arrow=0;
int 		St = -1000;
int			has_talk=0;
int         animating=0;

XPoint pcard[] = {{0,0}, {80,0}, {160,0}, {240,0},{320,0},{400,0},{480,0},{560,0},{640,0},{720,0},{800,0},{880,0},
		 		  {0,125}, {80,125}, {160,125}, {240,125},{320,125},{400,125},{480,125},{560,125},{640,125},{720,125},{800,125},{880,125}, 
		 		  {0,248}, {80,248}, {160,248}, {240,248},{320,248},{400,248},{480,248},{560,248},{640,248},{720,248},{800,248},{880,248}, 
		 		  {0,372}, {80,372}, {160,372}, {240,372},{320,372},{400,372}}; 
XPoint parrow[] = {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}};

XImage* bmdcards = NULL;
int q=2;

// 
void init_random();
int Button_New(Display	*display, Window window);
int QuitApplication( Display *display, Window	window);
void MakeButtons( Display * display, Window window, unsigned long fore, unsigned long back, Font font_id);
int Refresh( Display   *display, Window window, GC gc, Pixmap pixmap, int x, int y, int width, int height);
int ShowCards(Display *display,Window window, Pixmap pixmap, GC gc, int card);
int TableCards (Display *display, Window window, Pixmap pixmap, GC gc, int card);
int Cutting(Display *display, Window window, Pixmap pixmap, GC gc, int card, int pos);
int TalkMachine(Display *display, Window window, GC gc, char *text, int	type);
int EventLoop( Display*display, Window window, Pixmap pixmap, GC gc, int *width, int *height);
int DrawScore(Display *display, Window window, GC gc, 
              unsigned long c_back, unsigned long c_highlight, unsigned long c_shadow,
              int f, int horiz,
              TYPE_SCORE *draw_score);
int card1(Display *display, Window	window, int type);
int card2(Display *display, Window	window, int type);
int card3(Display *display, Window	window, int type);
int Button_Truco(Display *display, Window	window);
int Button_Yes(Display *display, Window	window);
int Button_No(Display *display, Window	window);
int Button_New(Display *display, Window	window);
int QuitApplication(Display *display, Window	window);
int FirstGame();
int SecondGame();
int ThirdGame();
int AcceptTruco(int val1);
int CanSayTruco(int val1, int val2);
int RANDOM( int val_score );

int main( argc, argv )
int argc;
char *argv[];
{
    Display    *display;
    int        screen;
    Window     rootwindow=0, window=0;
    int        x, y, width, height;
    XFontStruct *font;
    Pixmap     icon, pixmap;
    GC         pixgc, gc;
    int        font_height, position, fator;
    Colormap   colormap;
    int shmid, size, shmflg;
    key_t key; 
	(void) truco_bits;

#ifdef IPC_OK
    char * shmaddr=NULL;	
    size= 3600*sizeof( unsigned char *);
    key= IPC_PRIVATE;
    shmflg= IPC_CREAT;
    shmid= shmget( key, size, shmflg );
    shmflg |= 0x1ff;
    shmat( shmid, shmaddr, shmflg );
#endif
	for(x=0;x<40;x++) Cards[x]=x+1;
    x        = 10;
    y        = 10;
    width=640;
    height= 480;  	
    display   = SetUpDisplay( argc, argv, &screen );
    rootwindow= RootWindow( display, screen );
    black     = BlackPixel( display, screen );
    white     = WhitePixel( display, screen );
    colormap  = DefaultColormap( display, screen );  	
    blue	  = GetColor( display, "gray", colormap, white );
    green	  = GetColor( display, "#117e1f", colormap, 2L );
    red		  = GetColor( display, "red", colormap, white );
    navy	  = GetColor( display, "darkblue", colormap, black ); 

	Visual* visual = DefaultVisual(display, screen);
    bmdcards = loadBitmapFromMemory(display, visual, screen, cards_bmp, cards_bmp_len);

    if (!bmdcards) {
        fprintf(stderr, "Erro ao carregar a imagem BMP\n");
        XDestroyWindow(display, window);
        XCloseDisplay(display);
        return 1;
    }

	init_random();
    horiz  = width;
    vert = height;
    fator = horiz/6;
    for(position=0;position<3;position++)
    {
		Table[position].X= Table[position+3].X= fator*(position+1);
		Table[position].Y= 25;
		Table[position+3].Y= vert - 160;
		parrow[position].x = fator*(position+1);
		parrow[position].y = vert - 175;
    }
    Table[6].X= fator*5;
    Table[6].Y= (Table[0].Y+Table[3].Y)/2;
    Table[7].X= Table[0].X+36;
    Table[7].Y= (Table[0].Y+Table[3].Y)/2;
    Table[8].X= Table[2].X-36;
    Table[8].Y= (Table[2].Y+Table[3].Y)/2;
    pixmap = CreatePixmap( display, rootwindow, width,
              height, DefaultDepth( display, screen ),
		      black, green, &pixgc );
    XFreeGC( display, pixgc ); 
    CheckGeometry( argc, argv, DisplayWidth( display, screen ),
           DisplayHeight( display, screen ),
           &x, &y, &width, &height );
    window   = TopWindow( display, x, y, width, height,
			super_bits, super_width, super_height,
				   &icon, &gc );
    font = LoadFont(display, gc, argc, argv, "");
    font_height	= (font->ascent + font->descent);
	(void)font_height;
    SetNormalHints( display, window, x, y, width, height ); 
    SetWMHints( display, window, icon );
    NameWindow( display, window, "Super Truco", "Super Truco", "Super Truco" );
    MakeButtons( display, window, white, navy, font->fid );
	
    MapWindow( display, window );
    while( EventLoop( display, window, pixmap, gc,
			       &width, &height ) == True );
    XFreePixmap( display, pixmap );
    XFreePixmap( display, icon );
    CloseDisplay( display, window, gc );
#ifdef IPC_OK
    shmdt( shmaddr );
#endif
    exit( 0 );
}

void play_sound() {
    const char *sound = "/usr/share/sounds/freedesktop/stereo/bell.oga";
    if (access(sound, F_OK) == 0) {
        int rc = system("paplay /usr/share/sounds/freedesktop/stereo/bell.oga 2>/dev/null &");
        (void)rc;
    }
}

int Draw_Cards(Display *display, Window rootwindow,
               Pixmap pixmap, GC pixgc,
               int card, int posx, int posy)
{
    int src_x = pcard[card].x;
    int src_y = pcard[card].y;

	Pixmap cardsPixmap = XCreatePixmap(display, rootwindow,
                                       CARDS_WIDTH, CARDS_HEIGHT,
                                       DefaultDepth(display, DefaultScreen(display)));
    XPutImage(display, cardsPixmap, pixgc, bmdcards,
              src_x, src_y, 0, 0,
              CARDS_WIDTH, CARDS_HEIGHT);
    XCopyArea(display, cardsPixmap, pixmap, pixgc,
              0, 0,                          // origem da carta dentro do BMP
              CARDS_WIDTH, CARDS_HEIGHT,     // tamanho da carta
              posx, posy);                   // destino dentro do Pixmap
    XFreePixmap(display, cardsPixmap);
    return 0;
}

int Draw_Arrow(Display *display,Window window, Pixmap pixmap, GC gc, int pos_arrow_x, int pos_arrow_y, int clear, int type)
{ 
	int src_x = (clear)?1550:((type==0)?1500:1517);
    int src_y = 235;

	Pixmap cardsPixmap = XCreatePixmap(display, window, 12, 15,
                           DefaultDepth(display, DefaultScreen(display)));
    XPutImage(display, cardsPixmap, gc, bmdcards,
              src_x, src_y, 0, 0,
              12, 15);
    XCopyArea(display, cardsPixmap, pixmap, gc,
              0, 0,
              12, 15,
              pos_arrow_x, pos_arrow_y);                   

	Refresh( display, window, gc, pixmap, pos_arrow_x, pos_arrow_y,
			12, 15 );

	XFreePixmap(display, cardsPixmap);

	return 0;
} 

int Draw_Talk(Display *display,Window window, Pixmap pixmap, GC gc, int pos_x, int pos_y, int type)
{ 
	XPoint p[]={{1013,600}, {1097,600}, {1182,600}, {1268,600}, {1353,600},{1438,600},{1523,600}, {970,7} };
 	
	Pixmap cardsPixmap = XCreatePixmap(display, window,
                                       80, 100,
                                       DefaultDepth(display, DefaultScreen(display)));
    XPutImage(display, cardsPixmap, gc, bmdcards,
              p[type].x, p[type].y, 0, 0,
              80, 100);
    XCopyArea(display, cardsPixmap, pixmap, gc,
              0, 0,
              80, 100,
              pos_x, pos_y);                   // destino dentro do Pixmap

	Refresh( display, window, gc, pixmap, pos_x, pos_y,
			  80, 100 );

	XFreePixmap(display, cardsPixmap);

	has_talk=1;

	return 0;
} 

void Clear_Talk(Display *display, Window window, Pixmap pixmap, GC gc) {
	if(!has_talk) return;
	Draw_Talk(display, window, pixmap, gc, 450 , 380, 7);
	has_talk=0;
}


int First_Openning(Display *display, Window rootwindow, Pixmap pixmap, GC pixgc, int type)
{
    Pixmap	bitmap1;
	
    bitmap1  = XCreateBitmapFromData( display, rootwindow,
		super_bits, super_width, super_height );
    if(type!=0)
    	XSetBackground( display, pixgc, green );
    else
	XSetBackground( display, pixgc, red );	
    XSetForeground( display, pixgc, green );	
    XCopyPlane( display, bitmap1,
			     pixmap, pixgc, 0, 0,
			     super_width,
			     super_height,
			     (horiz/2)-(super_width/2),
			     (vert/2)-(super_height/2),
			     0x01 );
    XFreePixmap( display, bitmap1 );
    Refresh( display, rootwindow, pixgc, pixmap, (horiz/2)-(super_width/2),
		(vert/2)-(super_height/2), super_width, super_height );
	return 0;
}

int DrawPartCards(Display *display, Window rootwindow, Pixmap pixmap, GC pixgc, int card, int posx, int posy, int width)
{
	int src_x = pcard[card].x;
    int src_y = pcard[card].y;

	(void)width;

	Pixmap cardsPixmap = XCreatePixmap(display, rootwindow,
                                       CARDS_WIDTH, CARDS_HEIGHT,
                                       DefaultDepth(display, DefaultScreen(display)));
    XPutImage(display, cardsPixmap, pixgc, bmdcards,
              src_x, src_y, 0, 0,
              CARDS_WIDTH, CARDS_HEIGHT);
    XCopyArea(display, cardsPixmap, pixmap, pixgc,
              0, 0,                          // origem da carta dentro do BMP
              CARDS_WIDTH, CARDS_HEIGHT,     // tamanho da carta
              posx, posy);                   // destino dentro do Pixmap
    XFreePixmap(display, cardsPixmap);
    return 0;
}

int ClearCard(Display *display, Window rootwindow, Pixmap pixmap, GC pixgc, int posx, int posy, int width)
{

	int src_x = pcard[41].x;
    int src_y = pcard[41].y;

	(void) width;

	Pixmap cardsPixmap = XCreatePixmap(display, rootwindow,
                                       CARDS_WIDTH, CARDS_HEIGHT,
                                       DefaultDepth(display, DefaultScreen(display)));
    XPutImage(display, cardsPixmap, pixgc, bmdcards,
              src_x, src_y, 0, 0,
              CARDS_WIDTH, CARDS_HEIGHT);
    XCopyArea(display, cardsPixmap, pixmap, pixgc,
              0, 0,                         // origem da carta dentro do BMP
              CARDS_WIDTH, CARDS_HEIGHT,    // tamanho da carta
              posx, posy);                  // destino dentro do Pixmap
    XFreePixmap(display, cardsPixmap);
    return 0;
}


void draw_medal(Display *display, Window window, Pixmap pixmap, GC gc, int n,
                int pos_x, int pos_y) {
    int src_x = 1230;
    int src_y = (n==0)?250:385;

    Pixmap pix =  XCreatePixmap(display, window, 150, 130,
                            DefaultDepth(display, DefaultScreen(display)));
    XPutImage(display, pix, gc, bmdcards,
              src_x, src_y, 0, 0,
              150, 130);

	XCopyArea(display, pix, pixmap, gc,
				0, 0,
				150, 130,
				pos_x, pos_y);

    XFreePixmap(display, pix);
}

void draw_animated_cards(Display *display, Window window, Pixmap pixmap, GC gc,
                Card cards[], int n,
                XImage *cardsImage, XPoint *cardPositions) {
    int src_x = cardPositions[40].x;
    int src_y = cardPositions[40].y;

    Pixmap cardsPixmap = XCreatePixmap(display, window, CARDS_WIDTH, CARDS_HEIGHT,
                                       DefaultDepth(display, DefaultScreen(display)));
    XPutImage(display, cardsPixmap, gc, cardsImage,
              src_x, src_y, 0, 0,
              CARDS_WIDTH, CARDS_HEIGHT);

    for (int i = 0; i < n; i++) {
        XCopyArea(display, cardsPixmap, pixmap, gc,
                  0, 0,
                  CARDS_WIDTH, CARDS_HEIGHT,
                  cards[i].x, cards[i].y);
    }

    XFreePixmap(display, cardsPixmap);
}

void animate(Display *display, Window window, Pixmap pixmap, GC gc,
             int width, int height, int who,
             XImage *cardsImage, XPoint *cardPositions) {
				
    Card cards[NUM_CARDS];
    int center_x = width / 2;
    int center_y = height / 2;
	animating = 1;

	XColor background;
    Colormap cmap = DefaultColormap(display, DefaultScreen(display));
    XParseColor(display, cmap, "#117e1f", &background);
    XAllocColor(display, cmap, &background);
    XSetForeground(display, gc, background.pixel);
	
    for (int i = 0; i < NUM_CARDS; i++) {
        cards[i].x = center_x;
        cards[i].y = center_y;
        cards[i].dx = (i - 2) * 4;
        cards[i].dy = ((i % 2 == 0) ? -1 : 1) * 4;
    }

    for (int step = 0; step < 50; step++) {
        XFillRectangle(display, pixmap, gc, 0, 0, width, height);
        draw_animated_cards(display, window, pixmap, gc, cards, NUM_CARDS, cardsImage, cardPositions);
        Refresh(display, window, gc, pixmap, 0, 0, width, height);
        for (int i = 0; i < NUM_CARDS; i++) {
            cards[i].x += cards[i].dx;
            cards[i].y += cards[i].dy;
        }
        usleep(10000);
    }
    XFillRectangle(display, pixmap, gc, 0, 0, width, height);
	draw_medal(display, window, pixmap, gc, who, width/2-75, height/2-65);
	Refresh(display, window, gc, pixmap, 0, 0, width, height);
	XFlush(display);
	usleep(5000000);
    XFillRectangle(display, pixmap, gc, 0, 0, width, height);
    Refresh(display, window, gc, pixmap, 0, 0, width, height);
	animating=0;
}


int SelectByKey(Display*display, Window window, Pixmap pixmap, GC gc){
	int min=(State==STEP_PLAYER_ACCEPT_TRUCO)? 3: 0;
	if(i_arrow<0){
		i_arrow= min;
		while(i_arrow<3 && Table[i_arrow+3].card_state<1){
			i_arrow=(i_arrow>6)?min:i_arrow+1;
		}
		Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, False, (i_arrow>2));
	}
	if(Message==2000){
		Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
		while(True){
			i_arrow=(i_arrow>min)?i_arrow-1:7;
			if(i_arrow<3 && Table[i_arrow+3].card_state<1) continue;
			break;
		}
		Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, False, (i_arrow>2));
		Message=0;
	}else if(Message==2010){
		Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
		while(True){
			i_arrow=(i_arrow>6)?min:i_arrow+1;
			if(i_arrow<3 && Table[i_arrow+3].card_state<1) continue;
			break;
		}
		Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, False, (i_arrow>2));
		Message=0;
	}else if(Message==2020){
		Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
		Message=0;
		switch (i_arrow)
		{
			case 0:
				card1(display, window, 1);
				break;
			case 1:
				card2(display, window, 1);
				break;
			case 2:
				card3(display, window, 1);
				break;
			case 3:
				Button_Truco(display, window);
				break;
			case 4:
				Button_Yes(display, window);
				break;
			case 5:
				Button_No(display, window);
				break;
			case 6:
				Button_New(display, window);
				break;
			case 7:
				QuitApplication( display, window);
				break;
			}
		}
	return 0;
}

int EventLoop( Display*display, Window window, Pixmap pixmap, GC gc, int *width, int *height)
{
    XEvent      event;
    KeySym      keysym;
    static int	what=0;
    int		status=False;
    int  	Last_State=STEP_OPEN_GAME, position, resulters, aux;
    struct timeval timer1, timer2;
    struct timezone zone;

	/* Debug
	if(St != State){
		printf("State=%d\n", State);
		St = State;
	}
	*/

	if((status= CheckEvent( display, True, *width, *height, &event, &keysym ))==True){
		Last_State= State;
		if( ButtonEvent( display, &event ) == True ){
			if( State==STEP_END_APP)
				return( False );
		}else if( InvEvent( display, &event ) == True ){
			if( State==STEP_END_APP)
				return( False );
		}
    }	
    switch(State)
    {
	case STEP_OPEN_GAME:
    	First_Openning(display, window, pixmap, gc, 0 );
		TrucoButton(1);
		Clear_Talk(display, window, pixmap, gc);
		i_arrow= 6;
		Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, False, 1);
		State=STEP_WAITING;
		break;
	case STEP_WAITING:
		aux= RANDOM(40); 
		(void)aux;
		if(Message==2000){
			Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, 1);
			i_arrow=(i_arrow<7)?i_arrow+1:6;
			Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, False, 1);
			Message=0;
		}else if(Message==2010){
			Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, 1);
			i_arrow=(i_arrow>6)?6:i_arrow+1;
			Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, False, 1);
			Message=0;
		}else if(Message==2020){
			if (i_arrow==7){
				QuitApplication( display, window);
				return False;
			}else{
				Button_New(display, window);
			}
		}
		break;
	case STEP_NEW_GAME:
		Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, 1);
		TrucoButton(1);
		Clear_Talk(display, window, pixmap, gc);
		sprintf(Messa, "Starting a new game...");
		TalkMachine( display, window, gc, Messa, 0 );
		if(Last_State==STEP_INIT_GAME) 
			Cutting(display, window, pixmap, gc, BACK, 480 );
    	First_Openning(display, window, pixmap, gc, 1 );
		score[0].val_score=0; score[1].val_score=0; 
    	DrawScore(display, window, gc, green_back, 
		green_highlight, green_shadow, font_height, horiz, score);
		/* fall through */
	case STEP_DECK_CUTTING:
		TrucoButton(1);
		Clear_Talk(display, window, pixmap, gc);
		TalkMachine( display, window, gc, Messa, 1 );
		for(what=0;what<9;what++) Table[what].card_state=0;
		TableCards( display, window, pixmap, gc, BACK );
		strcpy(Messa, "Please, CUT...");
		TalkMachine( display, window, gc, Messa, 0 ); 
		ShowCards(display, window, pixmap, gc, BACK );
		Message=Last_State=0;
		pos_arrow_x= 20;
		pos_arrow_y= vert/2 - 48 - 17; 
		Draw_Arrow(display, window, pixmap, gc, horiz/2 -36- 20*10 + (10*pos_arrow_x), pos_arrow_y, False, 0);
		State=STEP_INIT_GAME;
		break;
	case STEP_INIT_GAME:
		aux= RANDOM(40);	
		if((Message==4) || (Message==40) || (Message==2020))
		{
			// ButtonCut
			ValGame=1;
			You_Play=100;
			Who_Say_Truco=Sum_Val=danger=0;
			pc=you=-1;
			if((++Begining)>2) Begining=1;
			Beginer=Begining;
			score1=score2=first=0;
			//

			pos_arrow_y= vert/2 - 48 - 17; 
			Draw_Arrow(display, window, pixmap, gc, horiz/2 -36- 20*10 + (10*pos_arrow_x), pos_arrow_y, True, 0);
			TalkMachine( display, window, gc, Messa, 1 );
			what=(Message==2020)?horiz/2 -36- 20*10 + (pos_arrow_x*10):event.xbutton.x;
			Cutting(display, window, pixmap, gc, BACK, what );
			TableCards( display, window, pixmap, gc, BACK );
			strcpy(Messa, "Let's go...");
			i_arrow=-1; // arrow start
			if(Beginer==1){
				State=STEP_PLAYER_GAME;
				i_arrow = -1;
			} else{
				State=STEP_PC_EVALUATE_GAME;
			}
			if((MyScore==MAXIMO-1) && (YourScore<11))
				State=STEP_PC_ACCEPT_TRUCO;
			else if((YourScore==MAXIMO-1) && (MyScore<11))
			{ 
				Sum_Val=2; State=STEP_PLAYER_ACCEPT_TRUCO; i_arrow=-1;
				strcpy(Messa, "Will You go play?...");
				Draw_Talk(display, window, pixmap, gc, 450 , 380, 6);
			}
			TalkMachine( display, window, gc, Messa, 0 ); 
			Message=0;
		}else if(Message==2000){
			pos_arrow_y= vert/2 - 48 - 17; 
			Draw_Arrow(display, window, pixmap, gc, horiz/2 -36- 20*10 + (10*pos_arrow_x), pos_arrow_y, True, 0);
			if(pos_arrow_x>0) pos_arrow_x--;
			Draw_Arrow(display, window, pixmap, gc, horiz/2 -36- 20*10 + (10*pos_arrow_x), pos_arrow_y, False, 0);
			Message=0;
		}else if(Message==2010){
			pos_arrow_y= vert/2 - 48 - 17; 
			Draw_Arrow(display, window, pixmap, gc, horiz/2 -36- 20*10 + (10*pos_arrow_x), pos_arrow_y, True, 0);
			if(pos_arrow_x<39) pos_arrow_x++;
			Draw_Arrow(display, window, pixmap, gc, horiz/2 -36- 20*10 + (10*pos_arrow_x), pos_arrow_y, False, 0);
			Message=0;
		}
		break;
	case STEP_PLAYER_GAME:
		SelectByKey(display, window, pixmap, gc);
		if((Message==5) && (Who_Say_Truco!=2))
		{
			Clear_Talk(display, window, pixmap, gc);
			if(i_arrow>=0 && i_arrow<8)
				Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
			State=STEP_PC_ACCEPT_TRUCO;
			Who_Say_Truco=2;
			ValGame+= Sum_Val;
			Sum_Val=0;
		}
		else if((Message==1) || (Message==2) || (Message==3)
			|| (Message==10) || (Message==20) || (Message==30))
		{
			Clear_Talk(display, window, pixmap, gc);
			if(i_arrow>=0 && i_arrow<8)
				Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
			ValGame+= Sum_Val;
			Sum_Val=0;
			if(Message<10)
			{
				you=Table[6+Play].val_score
					=Table[Message+2].val_score;
				Table[6+Play].what= Table[Message+2].what;
				Table[6+Play].card_state=2;
				Table[Message+2].card_state=0;
			}
			else
			{
				Table[Message/10+2].card_state=0;
				you=Table[6+Play].val_score =0;
				Table[6+Play].card_state=1;
			}
			TableCards( display, window, pixmap, gc, BACK );
			if((++Beginer)>2) Beginer=1;
			if(Beginer==1) {
				State=STEP_PLAYER_GAME;
				i_arrow = -1;
			} else{
				State=STEP_PC_EVALUATE_GAME;
			}
			if((++Play)>2){ Play=1; State=STEP_END_GAME; }
		}
		else if(Message==ERR_TRUCO)
		{
			Clear_Talk(display, window, pixmap, gc);
			if(i_arrow>=0 && i_arrow<8)
				Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
			ValGame += Sum_Val;
			Sum_Val = 0;
			pc=you+1;
			score1=0; score2=2;
			State=STEP_END_GAME;
		}			
		Message=0;
		break;
	case STEP_PC_EVALUATE_GAME:
		if(You_Play==100)
		switch(Who_Play)
		{
			case 1:
				You_Play= FirstGame();
				break;
			case 2:
				You_Play= SecondGame();
				break;
			case 3:
				You_Play= ThirdGame();
				break;
		}
		position= You_Play;
		if(position<0) position*=(-1);
		position--;
		resulters=0;
		if((Who_Play>1) && (Who_Say_Truco!=1) &&
		  (ValGame<12) && !((Who_Play==2) && (You_Play<0))
			&& (YourScore!=MAXIMO-1) && (MyScore!=MAXIMO-1) )
			resulters=CanSayTruco(position, you);
		if(resulters>=1)
		{
			play_sound();
			Who_Say_Truco=1;
			if(ValGame==1) Sum_Val=2; else Sum_Val=3;
			sprintf(Messa, "Truco (%2d). Do you accept?...", 
					Sum_Val+ValGame );
			Draw_Talk(display, window, pixmap, gc, 450 , 380, (ValGame+Sum_Val)/3-1);
			TrucoButton(ValGame+Sum_Val);			
			TalkMachine( display, window, gc, Messa, 0 );
		}
		if(Sum_Val==0)
		{
		   if(You_Play>0)
		   {
			You_Play--;
			Table[6+Play].what= Table[You_Play].what;
			pc=Table[6+Play].val_score=Table[You_Play].val_score;
			Table[6+Play].card_state= 2;
			Table[You_Play].card_state=0;
		   }
		   else
		   {
			You_Play*=(-1);You_Play--;
			Table[6+Play].what = 0;
			pc=Table[6+Play].val_score = 0;
			Table[6+Play].card_state= 1;
			Table[You_Play].card_state=0;
		   }
		   TableCards( display, window, pixmap, gc, BACK );
		   if((++Beginer)>2) Beginer=1;
		   if(Beginer==1) {
				State=STEP_PLAYER_GAME; 
				i_arrow=-1;
		   }else{
				 State=STEP_PC_EVALUATE_GAME;
		   }
		   if((++Play)>2) { Play=1; State=STEP_END_GAME; }
		}
		else{
			State=STEP_PLAYER_ACCEPT_TRUCO;
			i_arrow=-1;
		}
		break;
	case STEP_PLAYER_ACCEPT_TRUCO:
		SelectByKey(display, window, pixmap, gc);
		if((Message==5) && (Who_Say_Truco!=2))
		{
			Clear_Talk(display, window, pixmap, gc);
			if(i_arrow>=0 && i_arrow<8)
				Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
			State=STEP_PC_ACCEPT_TRUCO;
			Who_Say_Truco=2;
			TalkMachine( display, window, gc, Messa, 1 ); 
			ValGame+= Sum_Val;
			Sum_Val=0;
		}
		else if(Message==YES)
		{
			Clear_Talk(display, window, pixmap, gc);
			if(i_arrow>=0 && i_arrow<8)
				Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
			Message=0;
			TalkMachine( display, window, gc, Messa, 1 ); 
			if(Beginer==1) {
				State=STEP_PLAYER_GAME;
				i_arrow = -1;
			} else{
				State=STEP_PC_EVALUATE_GAME;
			}
		}
		else if(Message==NO)
		{
			Clear_Talk(display, window, pixmap, gc);
			if(i_arrow>=0 && i_arrow<8)
				Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
			strcpy(Messa, "You're running away again...");
			TalkMachine( display, window, gc, Messa, 0 ); 
			Message=0;
			State=STEP_END_GAME;
			Sum_Val=score1=0;
			score2=10;
			you=0;
			pc=1;
		}
		else if(Message==ERR_TRUCO)
		{
			if(i_arrow>=0 && i_arrow<8)
				Draw_Arrow(display, window, pixmap, gc, parrow[i_arrow].x, parrow[i_arrow].y, True, (i_arrow>2));
			ValGame += Sum_Val;
			Sum_Val = 0;
			pc=you+1;
			score1=0; score2=2;
			State=STEP_END_GAME;
			Message=0;
		}
		break;
	case STEP_PC_ACCEPT_TRUCO:
		if(Beginer==1) {
			State=STEP_PLAYER_GAME;
			i_arrow = -1;
		} else{
			State=STEP_PC_EVALUATE_GAME;
		}
		resulters=AcceptTruco(pc);
		if(ValGame==1) info.number_of_trucos++;
		if(resulters>=1)
		{
			if(ValGame==1) ValGame+=2; else ValGame+=3;
			if(( resulters>1) && (ValGame<12) && 
				((ValGame+MyScore)<MAXIMO))
			{
			  Sum_Val+=3;
			  Who_Say_Truco=1;
			  sprintf(Messa, "I Want %2d. Do you accept?...", 
					ValGame+Sum_Val );
			  TrucoButton(ValGame+Sum_Val);
			  Draw_Talk(display, window, pixmap, gc, 450 , 380, (ValGame+Sum_Val)/3-1);
			  State=STEP_PLAYER_ACCEPT_TRUCO;
			  i_arrow=-1;
			  Message=0;
			}
			else{
			  sprintf(Messa, "I accept. Let's go !!!");
			  Draw_Talk(display, window, pixmap, gc, 450 , 380, 5);
			}
			TalkMachine( display, window, gc, Messa, 0 );
		}
		else
		{
			sprintf(Messa, "Sorry!!! I don't accept...");
			TalkMachine( display, window, gc, Messa, 0 );
			Draw_Talk(display, window, pixmap, gc, 450 , 380, 4);
			State=STEP_END_GAME;score1=10; pc= score2=0; you=1;
		}
		break;
	case STEP_END_GAME:
		You_Play=100;
		if(you>pc)
		{
			score1++;
			Beginer=1;
			if(Who_Play==1) first=2;
		}
		else if(you<pc)
		{
			if(Who_Play==1) first=1;
			score2++;
			Beginer=2;
		}
		else if(you==pc)
		{
			if(Who_Play==1) first=3;
			if(!((score2==1) && (score1==1) && (Who_Play==2)))
				{ score2++;score1++;}
		}
		if(you>11)
			for(position=0;position<6;position++)
				if(Table[position].val_score<you)
					Table[position].val_score++;
		if(pc>11)
			for(position=0;position<6;position++)
				if(Table[position].val_score<pc)
					Table[position].val_score++;
		pc=you=-1;
		TableCards( display, window, pixmap, gc, BACK );
		gettimeofday(&timer1, &zone);
		while(1) { 
			gettimeofday(&timer2, &zone );
			if(timer2.tv_sec>(timer1.tv_sec+1L)) break; }
		if(Beginer==1){
			State=STEP_PLAYER_GAME;
			i_arrow = -1;
		 }else{
			State=STEP_PC_EVALUATE_GAME;
		 }
		Table[7].card_state=0; Table[8].card_state=0;
		if((score1>1) || (score2>1))
		{
			for(position=0;position<7;position++)
				Table[position].card_state=0;
			if((score1>1) && (score2>1))
				{ if(first==1)score1--; 
					else if(first==2) score2--; }
			if(score1>1)
			  YourScore+=ValGame;
			if(score2>1) MyScore+=ValGame;
			if((YourScore>=MAXIMO) || (MyScore>=MAXIMO))
				State=STEP_ANIMATE;
			else
				State=STEP_DECK_CUTTING;
			alert=score1=score2=0;
			Play=1;
			Who_Play=1;
		}
		else
			Who_Play++;
		score[0].val_score=MyScore; score[1].val_score= YourScore;
		TableCards( display, window, pixmap, gc, BACK );
		DrawScore(display, window, gc, green_back, green_highlight, 
			      green_shadow, font_height, horiz, score);
		break;	 
	case STEP_ANIMATE:
		if(YourScore>=MAXIMO){
			sprintf(Messa, "You Win !!!");
			animate(display, window, pixmap, gc, 640, 480, 0, bmdcards, pcard);
		}else if(MyScore>=MAXIMO){
			sprintf(Messa, "Ohhh! I win!!! Try again!");
			animate(display, window, pixmap, gc, 640, 480, 1, bmdcards, pcard);
		}
		if(YourScore>MAXIMO) YourScore=MAXIMO;
		TalkMachine( display, window, gc, Messa, 0 );
		State = 0;
		break;

    }	
    if(status==True)
    switch( event.type )
    {
        case Expose:
            Refresh( display, window, gc, pixmap,
				     event.xexpose.x,
				     event.xexpose.y,
				     event.xexpose.width,
				     event.xexpose.height );
					 
			if(!animating){
				DrawScore(display, window, gc, green_back, green_highlight, 
							green_shadow, font_height, horiz, score); 
				if(State==STEP_WAITING)
					strcpy(Messa, "This is the Xlib version of the Truco game...");
				TalkMachine( display, window, gc, Messa, 0 ); 
			}
        	XFlush( display );
            break;
        case KeyPress:
            if( ( keysym == XK_Q ) || ( keysym == XK_q ) )
            {
            	return( False );
            }else if( ( keysym == XK_A ) || ( keysym == XK_a ) ){
				if((++SHOW)>2) SHOW=1;
            }else if(keysym == XK_Left){
				Message = 2000;
			}else if(keysym == XK_Right){
				Message = 2010;
			}else if(keysym == XK_Return){
				Message = 2020;
			}
			break;
        case ConfigureNotify:
            *width  = event.xconfigure.width;
            *height = event.xconfigure.height;
            break;
    }
    return( True );
}

int Refresh( Display   *display, Window window, GC gc, Pixmap pixmap, int x, int y, int width, int height)
{
    if( ( x > horiz ) || ( y > vert ) )
    {
        return 0;
    }
    if( ( x + width ) > horiz )
    {
        width = horiz - x;
        if( width < 0 )
        {
             return 0;
        }
    }
    if( ( y + height ) > vert )
    {
        height = vert - y;
        if( height < 0 )
	{
	    return 0;
	}
    }
    XCopyArea( display, pixmap, window, gc, x, y, width,
			height, x, y );
	return 0;
}

void MakeButtons( Display * display, Window window, unsigned long fore, unsigned long back, Font font_id)
{
	int	QuitApplication();
	int	Button_Truco();
	int	Button_No();
	int Button_Yes();
	int	Button_New();
	int	card1();
	int	card2();
	int	card3();
	int	Button_Cut();
	int	x, y, pos1, pos2;
	int n_arrow=7;

	pos1 = horiz/2 -36- 20*10; 
	pos2 = vert/2 - 48;
	x = 390+CARDS_WIDTH;
	InvButton( display, window, pos1, pos2, x, CARDS_HEIGHT, Button_Cut ); 
	x = horiz- (BUTTON_WIDTH + 5);
	y = vert-BUTTON_HEIGHT-5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "QUIT",QuitApplication );
	parrow[n_arrow].x = x - 20;
	parrow[n_arrow--].y = y + BUTTON_HEIGHT/2 - 7; 
	y -= BUTTON_HEIGHT + 5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "NEW",Button_New );
	parrow[n_arrow].x = x - 20;
	parrow[n_arrow--].y = y + BUTTON_HEIGHT/2 - 7; 
	y -= BUTTON_HEIGHT + 5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "NO", Button_No );
	parrow[n_arrow].x = x - 20;
	parrow[n_arrow--].y = y + BUTTON_HEIGHT/2 - 7; 
	y -= BUTTON_HEIGHT + 5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "YES", Button_Yes );
	parrow[n_arrow].x = x - 20;
	parrow[n_arrow--].y = y + BUTTON_HEIGHT/2 - 7; 
	y -= BUTTON_HEIGHT + 5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "TRUCO", Button_Truco );
	parrow[n_arrow].x = x - 20;
	parrow[n_arrow--].y = y + BUTTON_HEIGHT/2 - 7; 
	InvButton( display, window, Table[3].X, Table[3].Y, CARDS_WIDTH, 
		CARDS_HEIGHT, card1 );
	InvButton( display, window, Table[4].X, Table[4].Y, CARDS_WIDTH, 
		CARDS_HEIGHT, card2 );
	InvButton( display, window, Table[5].X, Table[5].Y, CARDS_WIDTH, 
		CARDS_HEIGHT, card3 ); 
}

int Button_Truco(Display	*display, Window window)
{
	(void) display;
	(void) window;
	if((YourScore==MAXIMO-1) || (MyScore==MAXIMO-1))
		Message=ERR_TRUCO;
	else
		Message=5;
	return 0;
}

int Button_Yes(Display	*display, Window window)
{
	(void) display;
	(void) window;
	ValGame+= Sum_Val;
	Sum_Val=0;
	Message=YES;
	return 0;
}

int Button_No(Display	*display, Window window)
{
	(void) display;
	(void) window;
	if(State>2)
	{
		State=STEP_PLAYER_ACCEPT_TRUCO;
		Message=NO;	
	}
	return 0;
}

int Button_New(Display	*display, Window window)
{
	(void) display;
	(void) window;
	State=STEP_NEW_GAME;
	Begining= RANDOM(2)+1;
	Beginer=Begining;
	alert=YourScore=MyScore=0;
	Play=1; Who_Play=1; You_Play=100;
	info.number_of_trucos=info.first_truco=0;
	return 0;
}

int QuitApplication( Display *display, Window	window)
{
	(void) display;
	(void) window;
	State=STEP_END_APP;
	fprintf(stderr,"\nSuper Truco (XTruco)\n"\
				   "By Marcos Martins Duma\n\n" );
	return 0;
}

int card1(Display *display, Window	window, int type)
{
	(void) display;
	(void) window;
	if(Table[3].card_state!=0)
		Message=1*type;
	return 0;
}

int card2(Display *display, Window	window, int type)
{
	(void) display;
	(void) window;
	if(Table[4].card_state!=0)
		Message=2*type;
	return 0;
}

int card3(Display *display, Window	window, int type)
{
	(void) display;
	(void) window;
	if(Table[5].card_state!=0)
		Message=3*type;
	return 0; 
}

int Button_Cut(Display *display, Window	window, int type)
{
	(void) display;
	(void) window;
	Message=4*type;
	return 0;
}

void Shuffle() 
{ 
	int auxs[40]; 
	int count=40,v; 
 
 	for(v=0;v<40;v++) auxs[v]=Cards[v]; 
	while(count) 
	{ 
		v= RANDOM(40); 
		if(auxs[v]>=0) 
		{ 
			Cards[--count]= auxs[v]; 
			auxs[v]= -1; 
		} 
	} 
} 

int ShowCards(Display *display,Window window, Pixmap pixmap, GC gc, int card)
{ 
	int what;
  	int a=0; 
	int pos1, pos2; 
 	
	if(card>39) what=card; else what=40; 
	pos1 = horiz/2 -36- 20*10; 
	pos2 = vert/2 - 48; 
	Draw_Cards( display, window, pixmap, gc, what,
				 pos1, pos2 );
	Refresh( display, window, gc, pixmap, pos1, pos2, 
			CARDS_WIDTH, CARDS_HEIGHT );
	XFlush( display );
	Shuffle(); 
 	pos1= horiz/2 -36- 20*10; 
	pos2= vert/2 - 48; 
 	for(a=1; a<41; a++) 
	{ 
		Draw_Cards( display, window, pixmap, gc, what,
				 pos1, pos2 );
		Refresh( display, window, gc, pixmap, pos1, pos2, 
				CARDS_WIDTH+10, CARDS_HEIGHT );
		XFlush(display);
		usleep(WAIT_VIEW);
		pos1+= 10;
 	} 
	return 0;
} 

int Cutting(Display *display, Window window, Pixmap pixmap, GC gc, int card, int pos)
{ 
	int a=0, b, corta, corta2; 
	int pos1, pos2, x; 
	int c[40], what; 
 
	if(card>39) what=card; else what=40; 
	for(a=0;a<40;a++) c[a]=Cards[a]; 
	pos1= horiz/2 -36- 20*10; 
	pos2= vert/2 - 48;
	if(pos>480) pos=480;
	corta=pos-pos1+10;
	if(corta<0) corta=0;
	corta=corta/10;
 	corta2=corta; pos=pos1+10*corta;  
	if(corta<40) 
	{ 
		x=0; b=10; 
		while(x<CARDS_WIDTH)
		{ 
			x+=b;
			if(x>CARDS_WIDTH) { b-=(x-CARDS_WIDTH);x=CARDS_WIDTH; }  
			pos-=b; 
			DrawPartCards( display, window, pixmap, gc, what,
				 pos, pos2, x );
			Refresh( display, window, gc, pixmap, pos, pos2, 
				x, CARDS_HEIGHT );
			//XFlush(display);
			usleep(WAIT_VIEW); 
		}
		pos1-=b;
		for(a=pos-10; a>=pos1; a-=10) 
		{ 
			DrawPartCards( display, window, pixmap, gc, what,
				 a, pos2, CARDS_WIDTH );
			ClearCard( display, window, pixmap, gc,
				 a+CARDS_WIDTH, pos2, 10 );
			Refresh( display, window, gc, pixmap, a, pos2, 
				CARDS_WIDTH+10, CARDS_HEIGHT );
			//XFlush(display);
			usleep(WAIT_VIEW); 
 		}
		pos=a;
	}
	else
		corta2=0;
	for(a=400; a>=(10*corta2); a-=10) 
	{ 
		DrawPartCards( display, window, pixmap, gc, what,
				 pos1+a, pos2, CARDS_WIDTH );
		ClearCard( display, window, pixmap, gc,
				 pos1+a+CARDS_WIDTH, pos2, 10 );
		Refresh( display, window, gc, pixmap, pos1+a, pos2, 
				CARDS_WIDTH+10, CARDS_HEIGHT );
		//XFlush(display);
		usleep(WAIT_VIEW); 
		
	} 
	corta2=pos1+a+10;
	for(a=pos;a<corta2; a+=10) 

	{ 
		ClearCard( display, window, pixmap, gc,
				 a, pos2, 10 );
		DrawPartCards( display, window, pixmap, gc, what,
				 a+10, pos2, CARDS_WIDTH );
		Refresh( display, window, gc, pixmap, a, pos2, 
				CARDS_WIDTH+20, CARDS_HEIGHT );
		//XFlush(display);
		usleep(WAIT_VIEW); 
	} 
	ClearCard( display, window, pixmap, gc,
			 corta2, pos2, CARDS_WIDTH );
 	Refresh( display, window, gc, pixmap, corta2, pos2, 
				CARDS_WIDTH, CARDS_HEIGHT );
	//XFlush(display);
	usleep(WAIT_VIEW); 
	b=0; 
	for(a=corta; a<40; a++) Cards[(b++)]=c[a]; 
	for(a=0; a<corta; a++) Cards[(b++)]= c[a];
	b=0; 
	for(a=0; a<9; a++)
	{
		Table[b].what= Cards[a];
		if(b<3) Table[b].card_state=SHOW;
		else if(b<7) Table[b].card_state=2;
		else Table[b].card_state=0;
		b++;
	}
	/*
	Table[0].what=21;Table[1].what=22;Table[2].what=23;
	Table[3].what=24;Table[4].what=25;Table[5].what=20;
	Table[6].what=1;
	*/
	for(a=6; a>=0; a--)
	{
		b=(Table[a].what-1)/4+1;
		if((a<6) && (( b==Table[6].val_score+1)
			|| ((b==1) && (Table[6].val_score==10))))
		Table[a].val_score=11 + (Table[a].what-1)%4;
		else Table[a].val_score=b;
	}
	return 0;
} 
 
int TableCards (Display *display, Window window, Pixmap pixmap, GC gc, int card)
{ 
	int a, what;

	if(card<40) card=BACK;
	for(a=0; a<9; a++)
	{
		if(Table[a].card_state!=0)
		{
			if(Table[a].card_state==1) what=card;
				else what=Table[a].what-1;
			Draw_Cards( display, window, pixmap, gc, what,
				 Table[a].X, Table[a].Y);
			Refresh( display, window, gc, pixmap, Table[a].X,
				 Table[a].Y, CARDS_WIDTH, CARDS_HEIGHT );
		}
		else
		{
			ClearCard( display, window, pixmap, gc,
				 Table[a].X, Table[a].Y, CARDS_WIDTH);
			Refresh( display, window, gc, pixmap, Table[a].X,
				 Table[a].Y, CARDS_WIDTH, CARDS_HEIGHT );
		}
	}
	return 0;
}

#define	SC_WIDTH	150

void ShowText( Display *display, Window window, GC gc, char text[], int pos1, int pos2)
{
	int tamanho;

	tamanho= strlen( text );
	XDrawImageString( display, window, gc,
		pos1, pos2,
		text, tamanho );
}


int DrawScore(Display *display, Window window, GC gc, 
              unsigned long c_back, unsigned long c_highlight, unsigned long c_shadow,
              int f, int horiz, 
              TYPE_SCORE *draw_score) {

    int SC_HEIGHT = 8 * f;

    int x = horiz - SC_WIDTH;
    int y = 0;
    int width = SC_WIDTH - 1;
    int height = SC_HEIGHT - 1;

    char Scores[10];
    Scores[9] = '\0';

    XSetForeground(display, gc, c_back);
    XFillRectangle(display, window, gc, x, y, width, height);

    XSetForeground(display, gc, c_highlight);
    XDrawLine(display, window, gc, x, y, x + width, y);       // Topo
    XDrawLine(display, window, gc, x, y, x, y + height);      // Esquerda

    XSetForeground(display, gc, c_shadow);
    XDrawLine(display, window, gc, x, y + height, x + width, y + height);    // Base
    XDrawLine(display, window, gc, x + width, y, x + width, y + height);     // Direita

    XSetForeground(display, gc, c_shadow);
    XDrawLine(display, window, gc, x + width / 2, y, x + width / 2, y + height);

    int header_height = 3 * f;
    XDrawLine(display, window, gc, x, y + header_height, x + width, y + header_height);

    XSetForeground(display, gc, WhitePixel(display, 0)); // texto branco
    ShowText(display, window, gc, "Machine", x + 5, y + 2 * f);
    ShowText(display, window, gc, "You", x + 5 + width / 2, y + 2 * f);

    // Scores
    sprintf(Scores, " %2d", draw_score[0].val_score);
    ShowText(display, window, gc, Scores, x + 5, y + header_height + 3 * f);

    sprintf(Scores, " %2d", draw_score[1].val_score);
    ShowText(display, window, gc, Scores, x + 5 + width / 2, y + header_height + 3 * f);
    return 0;
}
	

int TalkMachine(Display *display, Window window, GC gc, char *text, int	type)
{
	int	tam, pos1, pos2, a, b;
	char	buffer[80];

	if(type==1) strcpy(text," " );
	b= 30;
	a= 400;
	pos1=10;
	pos2=vert - b; /* -b/3; */
    XSetForeground( display, gc, green );
    XSetBackground( display, gc, green );
	XFillRectangle( display, window, gc, pos1, pos2, a, b );
	XSetForeground( display, gc, black );
 	XSetForeground( display, gc, white );
	strcpy( buffer, "Machine -> ");
	strcat( buffer, text );
	tam= strlen( buffer );	
	pos1+= 10;
	pos2+= (b/2);	
	XDrawImageString( display, window, gc, pos1, pos2,
		buffer, tam );
	return 0;
}


int FirstGame()
{
	struct {
			int v;
			int q;
			} c[3], t;
	int a,b,c1,c2,c3,ret=-1,aleat;

	for(a=0;a<3;a++)
	{
		c[a].v=Table[a].val_score;
		c[a].q=a+1;
	}
	for(a=0;a<3;a++)
	for(b=a;b<3;b++)
	{
		if(c[a].v>c[b].v){
		  t=c[a]; c[a]=c[b]; c[b]=t; }
	}
	c1=c[0].v; c2=c[1].v; c3=c[2].v;
	switch(Play)
	{
	case 1:
		if((c3==14) && (c2==13)) ret=c[0].q;
		else
		{
			aleat= RANDOM(10);
			ret=-1*(c[0].q);
			switch(aleat)
			{
			case 0:
				for(a=2;a>=0;a--)
					if(c[a].v<13) {ret=c[a].q; break; }
				break;
			case 1:
				ret=c[0].q;
				break;
			default:
				for(a=2;a>=0;a--)
				if(c[a].v<11) {ret=c[a].q; break; }
				if(ret>0)
					break;
				for(a=0;a<=2;a++)
					if(c[a].v<13) {ret=c[a].q; break; }
				break;
			}
		}
		break;
	case 2:
		if((RANDOM(3))==1) aleat=-1; else aleat=1;
		ret=aleat*c[0].q;
		if((c3==14) && (c2==13) && (c1<you)) ret=c[0].q*aleat;
		else if((c3==14) && (c2==13)) ret=c[0].q*(-1);
		else
		{
			for(a=0;a<=2;a++)
			{
				if(c[a].v==you) ret=c[a].q;
				if((c[a].v>you) && (c[a].v<12))
					{ ret=c[a].q; break; }
			}
			for(a=2;a>=1;a--)
				if((c[a].v==you) && (c3>8))
			ret=c[a].q;
		}
		break;
	}
	return (ret);
}

int SecondGame()
{
	struct {
			int v;
			int q;
			} c[2], t;
	int a,b,c1,c2,ret=-1,aleat;

	b=0;
	for(a=0;a<3;a++)
	{
		if(!Table[a].card_state) continue;
		c[b].v=Table[a].val_score;
		c[b].q=a+1;
		b++;
	}
	if(c[0].v>c[1].v) { t=c[0]; c[0]=c[1]; c[1]=t; }
	c1=c[0].v; c2=c[1].v;
	
	aleat=RANDOM(4);
	switch(Play)
	{
	case 1:
		ret=c[1].q;
		if(c2==14)
		{
			if(RANDOM(3)==0) aleat=-1; else aleat=1;
			if(c1<6) ret=c[0].q*aleat;
			else if(c1>5) ret=-1*c[0].q;
		}
		else if(c2==13)
		{
			if(RANDOM(2)==1)
			{
				if((RANDOM(3))==1) aleat=-1; else aleat=1;
				if((c1>7) && (YourScore<MyScore+2)) ret=-1*c[0].q;
				else if(c1<=6) ret=c[0].q*aleat;
			}
			else
				ret=c[1].q;
		}
		else if((c2>11) && (aleat==1) && (YourScore<MyScore+2))
		{
			if(RANDOM(4)==3) aleat=-1; else aleat=1;
			if(c1<6) ret=c[0].q*aleat;
			if(c1>6) ret=-1*c[0].q;
		}
		else if(c2>9)
		{
			if((RANDOM(10)==6) && (YourScore<MyScore+2))
				ret=c[0].q;
			else
				ret=c[1].q;
		}
		else if((c2>=4) && (c1<4)) ret=c[1].q;
		else if((c1<4) && (aleat==2)) ret=-1*c[0].q;
		else
		{
			ret=c[RANDOM(2)].q;
		}
		break;
	case 2:
		ret=-1*c[0].q;
		for(a=0;a<2;a++) if(c[a].v>you) {ret=c[a].q; break; }
		if((first==1) || ((ret<0) && (first==3)))
		   for(a=0;a<2;a++) if(c[a].v>=you) {ret=c[a].q; break; }
		break;
	}
	return (ret);
}

int ThirdGame()
{
	int a;
	int ret=-1;

	for(a=0;a<3;a++)
	{
		if(Table[a].card_state==0) continue;
		ret=a+1;
	}
	if((Play==2) && (Table[ret-1].val_score<you)) ret*=(-1);
	return (ret);
}

int CanSayTruco(int val1, int val2)
{
	int a,c1,c2,ret,aleat;

	ret=0;c1=0;c2=0;
	if(MyScore+ValGame>=MAXIMO) return( ret );
	for(a=0;a<3;a++)
	{
		if(!Table[a].card_state) continue;
		if(Table[val1].val_score==Table[a].val_score) 
			c1=Table[a].val_score;
		else 
			c2=Table[a].val_score;
	}
	switch(Who_Play)
	{
	case 2:
		ret=0;
		if((first==1) && (ValGame==1) && (You_Play>0))
		{
			if(c1>9) ret=1;
			else if((c1>8) && (c2>5)) ret=1;
			else if((c1>8) && (YourScore<MyScore+2)) ret=1;
			else if((c1>=6) && (info.number_of_trucos>3)
				&& (RANDOM(2)==0)) { danger=1; ret=1; }
			else if((c1>=6) && (Begining==1) && (YourScore<=MyScore))
				{ danger=1; ret=1; }
			else if((RANDOM(3)==0) && ((YourScore<=MyScore)
				|| (YourScore>=(MyScore/2)))) { danger=1; ret=1; }
		}
		else if(first==2)
		{
			if((ValGame+YourScore)>=MAXIMO) return(ret=1);
			if(val2==0) alert++;
			if((ValGame==1) && (c1>val2) && 
					(c2>11) && (val2!=0)) ret=1;
		}
		else if((first==3) && (val2<0))
		{
			if((ValGame+YourScore)>=MAXIMO) return(ret=1);
			else if((c1>9) && (ValGame==1)) ret=1;
			else if(c1>13) ret=1;
		}
		else if((first==3) && (val2>0) && (c1>val2)) ret=1;
		else if((first==3) && ((ValGame+YourScore)>=MAXIMO)) ret=1;
		break;
	case 3:
		if((ValGame+YourScore)>=MAXIMO) return(ret=1);
		ret=0;
		aleat=RANDOM(3);
		if(val2>=0)
		{
			
			if(c1>val2) ret=1;
			if((first==1) &&	(c1==val2)) ret=1;
			else if((val2<7)&& (aleat<=1)) { ret=1; danger=1; }
			else if(val2<=3) { ret=1; danger=1; }
			else if((RANDOM(4)==2) && (ValGame==1)
			&& (val2<11) && (YourScore<MyScore+4)) {ret=1; danger=1;}
		}
		else if(val2<0)
		{
			if((ValGame+YourScore)>=MAXIMO) return(ret=1);
			else if(c1==14) ret=1;
			else if((c1>10) && (ValGame==1)) ret=1;
			else if((c1>8) && !(alert) && (ValGame==1)) 
				ret=1;
		}
		break;
	}
	return(ret);
}

int AcceptTruco(int val1)
{
	int a,b,c1,c2,c3,ret,t;
	int c[3];

	c1=c2=c3=ret=0;
	if(val1>=0) c[ret++]=val1;
	for(a=0;a<3;a++)
	{
		if(!Table[a].card_state) continue;
		c[ret]=Table[a].val_score;
		ret++;
	}
	for(a=ret;a<3;a++) c[a]=0;
	for(a=0;a<3;a++)
	 for(b=a+1;b<3;b++)
		if((c[a]>c[b]) && (c[b]!=0)) {t=c[a];c[a]=c[b];c[b]=t;}
	c1=c[0];
	ret=0; while((ret<3) && (Table[ret].card_state>0)) ret++;
	if(c[1]!=0) c2= c[1]; else {c2= Table[ret].val_score; ret++; }
	while((ret<3) && (Table[ret].card_state>0)) ret++;
	if(c[2]!=0) c3= c[2]; else {c3= Table[ret].val_score; ret++; }
	ret=0;
	switch(Who_Play)
	{
	case 1:
		info.first_truco++;
		if((ValGame+YourScore)>=MAXIMO) return (ret=2);
		else if((c3>9) && (c2>8)) ret=1;
		else if((c3>10) && (c1>5) && (c2>6)) ret=1;
		else if((info.first_truco>1) 
			&& (c3>7) && (c2>6)) {ret=1;danger=1;}
		break;
	case 2:
		if((ValGame+YourScore)>=MAXIMO) 
			return(ret=2);
		if(first==1)
		{
			if(c2==14) ret=2;
			else if((c1>7) && (c2>6)) ret=1;
			else if((c2>8) && !(alert)) ret=1;
			else if(c2>10) ret=1;
		}
		else if(first==2)
		{
			if((c1>5) && (c2>8) && (c3>6)) ret=1;
			else if((c1>9) && (c2>10)) ret=1;
		}
		else if((first==3) && (val1>0))
		{
			if(val1==14) ret=2;
			else if(val1>9) ret=1;
			else if((ValGame==1) && (val1>8)
				&& (RANDOM(2)==0)) ret=1;
		}
		else if(first==3)
		{
			if(c2==14) ret=2;
			else if(c2>11) ret=1;
			else if((ValGame==1) && (c2>8)) ret=1;
			else if((ValGame==1) &&
				(c2>6) && (RANDOM(4)==1)) {ret=1;danger=1;}
		}
		break;
	case 3:
		if((ValGame+YourScore)>=MAXIMO) 
			return (ret=2);
		ret=0;
		if(you>=0){
			if(c1>you) ret=2;
			else if(c1>=you && first==1) ret=2;
			else if(you<7 && RANDOM(2)==1) ret=2;
			else ret=0;					
		}else if(val1>0){
			if(c1==14) ret=2;
			else if(c1>10) ret=1;
			else if((c1>6) && (first==3)) ret=1;
			else if((c1>7) && (ValGame==1)) ret=1;
			else if((c1>6) && (!alert) && (RANDOM(2)==1))
					 { ret=1; danger=1; }
		}
		else if(val1<0)
		{
			if(c1>11) ret=1;
			else if((c1>7) && !(alert)) ret=1;
			else if((first==1) && (Begining==1) && (c1>6) 
				&& (c2>6) && (c3>6)) ret=1;
		}
		break;
	}
	return(ret);
}

int RANDOM(int val_score)
{
    if (val_score <= 0) return 0;

    int r, limit = RAND_MAX - (RAND_MAX % val_score);
    do {
        r = rand();
    } while (r >= limit);

    return r % val_score;
}

void init_random()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    srand(ts.tv_nsec ^ ts.tv_sec);
}