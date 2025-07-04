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

#include "truco.xb"
#include "cards.xbm"

#define       MAX_PIX      30
#define       TOTAL        42
#define	      BACK	   40
#define	      MAXIMO	   12
#define	      YES	   88
#define	      NO	   89
#define	      NEW	   99
#define	      WAITING	   91
#define	      ERR_TRUCO	   666

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

BIT_STRUCT    		the_bitmaps[ MAX_PIX ];
TYPE_SCORE	      		score[ 2 ]={{0,0,0},{0,0,0}};	

int 			horiz, vert;
int 			State=0, Message=0, Cards[40];
char   			Messa[80];
static unsigned char    *CarBit[TOTAL];
static unsigned long    black, white, green, blue, red, navy;
Pixmap			bitmap1;
int			pc, you, Who_Play, You_Play, first, MyScore, YourScore;
int			alert, Sum_Val, ValGame, danger, Begining;
int			Play, Beginer, Who_Say_Truco,score1, score2;
int			SHOW=1;

main( argc, argv )
int argc;
char *argv[];
{
    Display    *display;
    int        screen;
    Window     rootwindow, window;
    int        x, y, width, height;
    XFontStruct *font;
    Pixmap     icon, pixmap;
    GC         pixgc, gc;
    int        num_bitmaps, font_height, position, fator;
    Colormap   colormap;
    int shmid, size, shmflg;
    key_t key; 
    char * shmaddr;	

#ifdef IPC_OK
    size= 3600*sizeof( unsigned char *);
    key= IPC_PRIVATE;
    shmflg= IPC_CREAT;
    shmid= shmget( key, size, shmflg );
    shmflg |= 0x1ff;
    shmat( shmid, shmaddr, shmflg );
#endif
    CarBit[0]= card1_bits;CarBit[1]= card2_bits;CarBit[2]= card3_bits;
    CarBit[3]= card4_bits;CarBit[4]= card5_bits;CarBit[5]= card6_bits;
    CarBit[6]= card7_bits;CarBit[7]= card8_bits;CarBit[8]= card9_bits;
    CarBit[9]= card10_bits;CarBit[10]= card11_bits;CarBit[11]= card12_bits;
    CarBit[12]= card13_bits;CarBit[13]= card14_bits;CarBit[14]= card15_bits;
    CarBit[15]= card16_bits;CarBit[16]= card17_bits;CarBit[17]= card18_bits;
    CarBit[18]= card19_bits;CarBit[19]= card20_bits;CarBit[20]= card21_bits;
    CarBit[21]= card22_bits;CarBit[22]= card23_bits;CarBit[23]= card24_bits;
    CarBit[24]= card25_bits;CarBit[25]= card26_bits;CarBit[26]= card27_bits;
    CarBit[27]= card28_bits;CarBit[28]= card29_bits;CarBit[29]= card30_bits;
    CarBit[30]= card31_bits;CarBit[31]= card32_bits;CarBit[32]= card33_bits;
    CarBit[33]= card34_bits;CarBit[34]= card35_bits;CarBit[35]= card36_bits;
    CarBit[36]= card37_bits;CarBit[37]= card38_bits;CarBit[38]= card39_bits;
    CarBit[39]= card40_bits;CarBit[40]= card41_bits;CarBit[41]= card42_bits;
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
    blue	= GetColor( display, "gray", colormap, white );
    green	= GetColor( display, "darkgreen", colormap, 2L );
    red	= GetColor( display, "red", colormap, white );
    navy	= GetColor( display, "darkblue", colormap, black ); 
    horiz  = width;
    vert = height;
    fator = horiz/6;
    for(position=0;position<3;position++)
    {
	Table[position].X= Table[position+3].X= fator*(position+1);
	Table[position].Y= 25;
	Table[position+3].Y= vert - 160;
    }
    Table[6].X= fator*5;
    Table[6].Y= (Table[0].Y+Table[3].Y)/2;
    Table[7].X= Table[0].X+36;
    Table[7].Y= (Table[0].Y+Table[3].Y)/2;
    Table[8].X= Table[2].X-36;
    Table[8].Y= (Table[2].Y+Table[3].Y)/2;
    num_bitmaps = 0;     
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
    font 	= LoadFont( display, gc, argc, argv, "variable" );
    font_height	= (font->ascent + font->descent);
    SetNormalHints( display, window, x, y, width, height );
    SetWMHints( display, window, icon );
    NameWindow( display, window, "Super Truco", "Super Truco", "Super Truco" );
    MakeButtons( display, window, gc, white, navy, font->fid );
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

Draw_Cards(display, rootwindow, pixmap, pixgc, card, posx, posy )
Display		*display;
Window		rootwindow;
Pixmap		pixmap;
GC		pixgc;
int		card, posx, posy;
{
    bitmap1  = XCreateBitmapFromData( display, rootwindow,
		CarBit[card], CARD_WIDTH, CARD_HEIGHT );
    if((card%2)==0)
    	XSetForeground( display, pixgc, red );
    else
	XSetForeground( display, pixgc, black );	
    XSetBackground( display, pixgc, white );	
    XCopyPlane( display, bitmap1,
			     pixmap, pixgc, 0, 0,
			     CARD_WIDTH,
			     CARD_HEIGHT,
			     posx,
			     posy,
			     0x01 );
    XFreePixmap( display, bitmap1 );
}

First_Openning(display, rootwindow, pixmap, pixgc, type )
Display		*display;
Window		rootwindow;
Pixmap		pixmap;
GC		pixgc;
int 		type;
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
}

DrawPartCards(display, rootwindow, pixmap, pixgc, card, posx, posy, width )
Display		*display;
Window		rootwindow;
Pixmap		pixmap;
GC		pixgc;
int		card, posx, posy, width;
{
    bitmap1  = XCreateBitmapFromData( display, rootwindow,
		CarBit[card], CARD_WIDTH, CARD_HEIGHT );
    if((card%2)==0)
    	XSetForeground( display, pixgc, red );
    else
	XSetForeground( display, pixgc, black );	
    XSetBackground( display, pixgc, white );	
    XCopyPlane( display, bitmap1,
			     pixmap, pixgc, 0, 0,
			     width,
			     CARD_HEIGHT,
			     posx,
			     posy,
			     0x01 );
    XFreePixmap( display, bitmap1 );
}

ClearCard(display, rootwindow, pixmap, pixgc, posx, posy, width )
Display		*display;
Window		rootwindow;
Pixmap		pixmap;
GC		pixgc;
int		width;
{
    bitmap1  = XCreateBitmapFromData( display, rootwindow,
		CarBit[0], CARD_WIDTH, CARD_HEIGHT );
 
    XSetForeground( display, pixgc, green );
    XSetBackground( display, pixgc, green );	
    XCopyPlane( display, bitmap1,
			     pixmap, pixgc, 0, 0,
			     width,
			     CARD_HEIGHT,
			     posx,
			     posy,
			     0x01 );
    XFreePixmap( display, bitmap1 );
}

EventLoop( display, window, pixmap, gc, width, height )
Display     *display;
Window      window;
Pixmap      pixmap;
GC          gc;
int         *width, *height;
{
    XEvent      event;
    KeySym      keysym;
    static int	what=0;
    int		status=False;
    int  	Last_State=0, position, resulters;
    struct timeval timer1, timer2;
    struct timezone zone;

    if((status= CheckEvent( display, True, *width, *height,
			&event, &keysym ))==True)
    {
	Last_State= State;
    	if( ButtonEvent( display, &event ) == True )
	{
		if( State==200)
			return( False );
	}
    	else if( InvEvent( display, &event ) == True )
	{
		if( State==200)
			return( False );
	}
    }	
    switch(State)
    {
	case 0:
    		First_Openning(display, window, pixmap, gc, 0 );
		State=WAITING;
		break;
	case NEW:
		sprintf(Messa, "Starting a new game...");
		TalkMachine( display, window, gc, Messa, 0 );
		if(Last_State==2) 
			Cuting(display, window, pixmap, gc, BACK, 480 );
    		First_Openning(display, window, pixmap, gc, 1 );
		score[0].val_score=0; score[1].val_score=0; 
    		DrawScore( display, window, pixmap, gc, blue,
		     white, white, 10 , horiz, vert, score );
	case 1:
		TalkMachine( display, window, gc, Messa, 1 );
		for(what=0;what<9;what++) Table[what].card_state=0;
		TableCards( display, window, pixmap, gc, BACK );
		strcpy(Messa, "Please, CUT...");
		TalkMachine( display, window, gc, Messa, 0 ); 
		ShowCards(display, window, pixmap, gc, BACK );
		Message=Last_State=0;
		State=2;
		break;
	case 2:
		if((Message==4) || (Message==40))
		{
			TalkMachine( display, window, gc, Messa, 1 ); 
			what=event.xbutton.x;
			Cuting(display, window, pixmap, gc, BACK, what );
			TableCards( display, window, pixmap, gc, BACK );
			strcpy(Messa, "Let's go...");
			if(Beginer==1) State=3; else State=4;
			if((MyScore==MAXIMO-1) && (YourScore<11))
				State=6;
			else if((YourScore==MAXIMO-1) && (MyScore<11))
			{ 
				Sum_Val=2; State=5;
				strcpy(Messa, "Will You go play?...");
			}
			TalkMachine( display, window, gc, Messa, 0 ); 
			Message=0;
		}
		break;
	case 3:
		if((Message==5) && (Who_Say_Truco!=2))
		{
			State=6;
			Who_Say_Truco=2;
			ValGame+= Sum_Val;
			Sum_Val=0;
		}
		else if((Message==1) || (Message==2) || (Message==3)
			|| (Message==10) || (Message==20) || (Message==30))
		{
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
			if(Beginer==1) State=3 ; else State=4;
			if((++Play)>2){ Play=1; State=13; }
		}
		else if(Message==ERR_TRUCO)
		{
			ValGame += Sum_Val;
			Sum_Val = 0;
			pc=you+1;
			score1=0; score2=2;
			State=13;
		}
			
		Message=0;
		break;
	case 4:
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
			Who_Say_Truco=1;
			if(ValGame==1) Sum_Val=2; else Sum_Val=3;
			sprintf(Messa, "Truco (%2d). Do you accept?...", 
					Sum_Val+ValGame );
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
		   if(Beginer==1) State=3; else State=4;
		   if((++Play)>2) { Play=1; State=13; }
		}
		else State=5;
		break;
	case 5:
		if((Message==5) && (Who_Say_Truco!=2))
		{
			State=6;
			Who_Say_Truco=2;
			TalkMachine( display, window, gc, Messa, 1 ); 
			ValGame+= Sum_Val;
			Sum_Val=0;
		}
		else if(Message==YES)
		{
			Message=0;
			TalkMachine( display, window, gc, Messa, 1 ); 
			if(Beginer==1) State=3; else State=4;
		}
		else if(Message==NO)
		{
			strcpy(Messa, "You're running away again...");
			TalkMachine( display, window, gc, Messa, 0 ); 
			Message=0;
			State=13;
			Sum_Val=score1=0;
			score2=10;
			you=0;
			pc=1;
		}
		else if(Message==ERR_TRUCO)
		{
			ValGame += Sum_Val;
			Sum_Val = 0;
			pc=you+1;
			score1=0; score2=2;
			State=13;
			Message=0;
		}
		break;
	case 6:
		if(Beginer==1) State=3; else State=4;
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
			  State=5;
			}
			else
			  sprintf(Messa, "I accept. Let's go !!!");
			TalkMachine( display, window, gc, Messa, 0 );
		}
		else
		{
			sprintf(Messa, "Sorry!!! I don't accept...");
			TalkMachine( display, window, gc, Messa, 0 );
			State=13;score1=10; pc= score2=0; you=1;
		}
		break;
	case 13:
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
		if(Beginer==1) State=3; else State=4;
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
			State=1;
			if(YourScore>=MAXIMO)
			  sprintf(Messa, "You Win !!!");
			else if(MyScore>=MAXIMO) 
			  sprintf(Messa, "Ohhh! I win!!! Try again!");
			if((YourScore>=MAXIMO) || (MyScore>=MAXIMO))
			{
				TalkMachine( display, window, gc, Messa, 0 );
				if(YourScore>MAXIMO) YourScore=MAXIMO;
				if(MyScore>MAXIMO) MyScore=MAXIMO;
				State=0;
			}
			alert=score1=score2=0;
			Play=1;
			Who_Play=1;
		}
		else
			Who_Play++;
		score[0].val_score=MyScore; score[1].val_score= YourScore;
		TableCards( display, window, pixmap, gc, BACK );
		DrawScore( display, window, pixmap, gc, blue,
			 white, white, 10 , horiz, vert, score );
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
    		   DrawScore( display, window, pixmap, gc, blue,
			 white, white, 10 , horiz, vert, score ); 
		   if(State==WAITING)
			strcpy(Messa, 
			   "This is the first version of the Xtruco...");
		   TalkMachine( display, window, gc, Messa, 0 ); 
                   XFlush( display );
                   break;
        case KeyPress:
                   if( ( keysym == XK_Q ) || ( keysym == XK_q ) )
                   {
                           return( False );
                   }
                   else if( ( keysym == XK_A ) || ( keysym == XK_a ) )
                   {
			if((++SHOW)>2) SHOW=1;
                   }
        case ConfigureNotify:
                   *width  = event.xconfigure.width;
                   *height = event.xconfigure.height;
                   break;
    }
    return( True );
}

Refresh( display, window, gc, pixmap, x, y, width, height )
Display   *display;
Window    window;
GC        gc;
Pixmap    pixmap;
int       x, y, width, height;
{
    if( ( x > horiz ) || ( y > vert ) )
    {
        return;
    }
    if( ( x + width ) > horiz )
    {
        width = horiz - x;
        if( width < 0 )
        {
             return;
        }
    }
    if( ( y + height ) > vert )
    {
        height = vert - y;
        if( height < 0 )
	{
	    return;
	}
    }
    XCopyArea( display, pixmap, window, gc, x, y, width,
			height, x, y );
}

MakeButtons( display, window, gc, fore, back, font_id, h, v )
Display		*display;
Window		window;
GC		gc;
unsigned long	fore, back;
Font		font_id;
{
	int	QuitApplication();
	int	Button_Truco();
	int	Button_No();
	int     Button_Yes();
	int	Button_New();
	int	card1();
	int	card2();
	int	card3();
	int	Button_Cut();
	int	x, y, pos1, pos2;

	pos1 = horiz/2 -36- 20*10; 
	pos2 = vert/2 - 48;
	x = 390+CARD_WIDTH;
	InvButton( display, window, pos1, pos2, x, CARD_HEIGHT, Button_Cut ); 
	x = horiz- (BUTTON_WIDTH + 5);
	y = vert-BUTTON_HEIGHT-5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "QUIT",QuitApplication );
	y -= BUTTON_HEIGHT + 5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "NEW",Button_New );
	y -= BUTTON_HEIGHT + 5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "NO", Button_No );
	y -= BUTTON_HEIGHT + 5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "YES", Button_Yes );
	y -= BUTTON_HEIGHT + 5;
	CreateButton( display, window, x, y, fore, back,
		font_id, "TRUCO", Button_Truco );
	InvButton( display, window, Table[3].X, Table[3].Y, CARD_WIDTH, 
		CARD_HEIGHT, card1 );
	InvButton( display, window, Table[4].X, Table[4].Y, CARD_WIDTH, 
		CARD_HEIGHT, card2 );
	InvButton( display, window, Table[5].X, Table[5].Y, CARD_WIDTH, 
		CARD_HEIGHT, card3 ); 
}

Button_Truco(display, window)
Display		*display;
Window		window;
{
	if((YourScore==MAXIMO-1) || (MyScore==MAXIMO-1))
		Message=ERR_TRUCO;
	else
		Message=5;
}

Button_Yes(display, window)
Display		*display;
Window		window;
{
	ValGame+= Sum_Val;
	Sum_Val=0;
	Message=YES;
}

Button_No(display, window)
Display		*display;
Window		window;
{
	if(State>2)
	{
		State=5;
		Message=NO;	
	}
}

Button_New( display, window )
Display	*display;
Window	window;
{
	State=NEW;
	Begining= RANDOM(2)+1;
	Beginer=Begining;
	alert=YourScore=MyScore=0;
	Play=1; Who_Play=1; You_Play=100;
	info.number_of_trucos=info.first_truco=0;
}

QuitApplication( display, window )
Display	*display;
Window	window;
{
	State=200;
	fprintf(stderr,"\nSuper Truco (XTruco)\n"\
		"By Marcos Martins Duma\n\n" );
}

card1( display, window, type )
Display	*display;
Window	window;
int	type;
{
	if(Table[3].card_state!=0)
		Message=1*type;
}

card2( display, window, type )
Display	*display;
Window	window;
int	type;
{
	if(Table[4].card_state!=0)
		Message=2*type;
}

card3( display, window, type )
Display	*display;
Window	window;
int 	type;
{
	if(Table[5].card_state!=0)
		Message=3*type;
}

Button_Cut( display, window, type )
Display	*display;
Window	window;
int 	type;
{
	Message=4*type;
	ValGame=1;
	You_Play=100;
	Who_Say_Truco=Sum_Val=danger=0;
	pc=you=-1;
	if((++Begining)>2) Begining=1;
	Beginer=Begining;
	score1=score2=first=0;
}

ShowCards(display, window, pixmap, gc, card )
Display		*display;
Window		window;
Pixmap		pixmap;
GC		gc;
int		card;
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
			CARD_WIDTH, CARD_HEIGHT );
	XFlush( display );
	Shuffle(); 
 	pos1= horiz/2 -36- 20*10; 
	pos2= vert/2 - 48; 
 	for(a=1; a<41; a++) 
	{ 
		Draw_Cards( display, window, pixmap, gc, what,
				 pos1, pos2 );
		Refresh( display, window, gc, pixmap, pos1, pos2, 
				CARD_WIDTH, CARD_HEIGHT );
		pos1+= 10;
 	} 
} 

Shuffle() 
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

Cuting(display, window, pixmap, gc, card, pos) 
Display		*display;
Window		window;
Pixmap		pixmap;
GC		gc;
int		card, pos;
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
		while(x<CARD_WIDTH)
		{ 
			x+=b;
			if(x>CARD_WIDTH) { b-=(x-CARD_WIDTH);x=CARD_WIDTH; }  
			pos-=b; 
			DrawPartCards( display, window, pixmap, gc, what,
				 pos, pos2, x );
			Refresh( display, window, gc, pixmap, pos, pos2, 
				x, CARD_HEIGHT );
		}
		pos1-=b;
		for(a=pos-10; a>=pos1; a-=10) 
		{ 
			DrawPartCards( display, window, pixmap, gc, what,
				 a, pos2, CARD_WIDTH );
			ClearCard( display, window, pixmap, gc,
				 a+CARD_WIDTH, pos2, 10 );
			Refresh( display, window, gc, pixmap, a, pos2, 
				CARD_WIDTH+10, CARD_HEIGHT );
 		}
		pos=a;
	}
	else
		corta2=0;
	for(a=400; a>=(10*corta2); a-=10) 
	{ 
		DrawPartCards( display, window, pixmap, gc, what,
				 pos1+a, pos2, CARD_WIDTH );
		ClearCard( display, window, pixmap, gc,
				 pos1+a+CARD_WIDTH, pos2, 10 );
		Refresh( display, window, gc, pixmap, pos1+a, pos2, 
				CARD_WIDTH+10, CARD_HEIGHT );
	} 
	corta2=pos1+a+10;
	for(a=pos;a<corta2; a+=10) 
	{ 
		ClearCard( display, window, pixmap, gc,
				 a, pos2, 10 );
		DrawPartCards( display, window, pixmap, gc, what,
				 a+10, pos2, CARD_WIDTH );
		Refresh( display, window, gc, pixmap, a, pos2, 
				CARD_WIDTH+10, CARD_HEIGHT );
	} 
	ClearCard( display, window, pixmap, gc,
			 corta2, pos2, CARD_WIDTH );
 	Refresh( display, window, gc, pixmap, corta2, pos2, 
				CARD_WIDTH, CARD_HEIGHT );
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
	for(a=6; a>=0; a--)
	{
		b=(Table[a].what-1)/4+1;
		if((a<6) && (( b==Table[6].val_score+1)
			|| ((b==1) && (Table[6].val_score==10))))
		Table[a].val_score=11 + (Table[a].what-1)%4;
		else Table[a].val_score=b;
	}
} 
 
TableCards (display, window, pixmap, gc, card ) 
Display		*display;
Window		window;
Pixmap		pixmap;
GC		gc;
int		card;
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
				 Table[a].Y, CARD_WIDTH, CARD_HEIGHT );
		}
		else
		{
			ClearCard( display, window, pixmap, gc,
				 Table[a].X, Table[a].Y, CARD_WIDTH);
			Refresh( display, window, gc, pixmap, Table[a].X,
				 Table[a].Y, CARD_WIDTH, CARD_HEIGHT );
		}
	}
}

#define	SC_WIDTH	150


DrawScore( display, window, pixmap, gc, c1, c2, c3, f, h, v, draw_score )
Display			*display;
Window			window;
Pixmap			pixmap;
GC			gc;
unsigned long		c1, c2, c3; 	/* colors */
int			f, h, v;    	/* width_fonte, horiz, vert */ 
TYPE_SCORE		*draw_score;    /* Struct for Score (val_scores) */ 
{
	int	p1, p2, l, a, m, n, SC_HEIGHT;
	char	Scores[10];

	SC_HEIGHT=8*f;
	m= p1= horiz- SC_WIDTH;
	n= p2= 0;
	l= SC_WIDTH-1;
	a= SC_HEIGHT-1;
	Scores[9]='\0';
    	XSetForeground( display, gc, c1 );
    	XSetBackground( display, gc, c1 );
	XFillRectangle( display, window, gc, p1, p2, l, a );
	XSetForeground( display, gc, c3 );
	XDrawRectangle( display, window, gc, p1, p2, l, a );
	XDrawLine(display, window, gc, p1+l/2, p2, p1+l/2, p2+a );
	p2 += 3*f;
	a -= (3*f);
	XDrawLine(display, window, gc, p1, p2, p1+l, p2 );
	XSetForeground( display, gc, c2 );
	ShowText( display, window, gc, "Machine", p1+5, n+2*f );
	ShowText( display, window, gc, "You", p1+5+l/2, n+2*f );
	sprintf(Scores," %2d", score[0].val_score );
	ShowText( display, window, gc, Scores, p1+5, p2+3*f );
	sprintf(Scores," %2d", score[1].val_score );
	ShowText( display, window, gc, Scores, p1+5+l/2, p2+3*f );
}	
	
ShowText( display, window, gc, text, pos1, pos2 )
Display	*display;
Window	window;
GC	gc;
char	text[];
int	pos1, pos2;
{
	int tamanho;

	tamanho= strlen( text );
	XDrawImageString( display, window, gc,
		pos1, pos2,
		text, tamanho );
}

TalkMachine( display, window, gc, text, type )
Display		*display;
Window		window;
GC		gc;
char		*text;
int		type;
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
	XFlush( display );
}


FirstGame()
{
	struct {
			int v;
			int q;
			} c[3], t;
	int a,b,c1,c2,c3,ret, aleat;

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

SecondGame()
{
	struct {
			int v;
			int q;
			} c[2], t;
	int a,b,c1,c2,ret,aleat;

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

ThirdGame()
{
	int a;
	int ret;

	for(a=0;a<3;a++)
	{
		if(Table[a].card_state==0) continue;
		ret=a+1;
	}
	if((Play==2) && (Table[ret-1].val_score<you)) ret*=(-1);
	return (ret);
}

CanSayTruco(val1, val2)
int val1, val2;
{
	int a,c1,c2,ret,aleat;

	ret=0;
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

AcceptTruco(val1)
int	val1;
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
		if((ValGame+YourScore)>=MAXIMO) return(ret=2);
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
		if((ValGame+YourScore)>=MAXIMO) return (ret=2);
		ret=0;
				if(val1>0)
		{
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

RANDOM( val_score )
int	val_score;
{
	int ret;

#ifndef RAND_MAX
	ret=(int) rand()%val_score;
#else
	ret=((int) (50.0*rand()/(RAND_MAX+1.0)))%val_score;
#endif
	return( ret );
}
