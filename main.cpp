#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <gl\freeglut.h>
#include <mmsystem.h>
#include "bass.h"
#include "code.h"

#pragma comment (lib, "bass.lib")


typedef struct
{
	char name[10];
	unsigned long score;
} HSCORE;

float WinWid=300;
float WinHei=600;

HSTREAM stream;

int field[10][22];

const int T=1,I=2,Z=3,S=4,J=5,L=6,O=7;
int shape,X,Y,sh_st;

int gameover=0, score=0, speed=1, score_lvl[9]={100,300,600,1000,1500,2100,2800,3600,4500};
char buf[4][30], sc[10]="Score: ", sp[10]="Speed: ", enter_name[20];

bool menu=1,music=0,sound=0,showscore=0;
int menu_choose=0;
unsigned char menu_items[6][10]={"NEW GAME","LEVEL: ","MUSIC: ","SOUND: ","SCORES","EXIT"};
char recor[10][20];

HSCORE high_score[10];

//char recor[10][20] = {"...................","...................","...................","...................","...................",
//"...................","...................","...................","...................","..................."};

void get_score()
{	
	Decode(recor,"cat.bmp");
	for(int i=0;i<10;i++)
	{		
		if (recor[i][0]!='.')
		{
			for(int j=0;recor[i][j]!='.';j++)
				high_score[i].name[strlen(high_score[i].name)] = recor[i][j];	
			int step=1;
			for(int q=18;recor[i][q]!='.';q--)
			{
				high_score[i].score = high_score[i].score + (recor[i][q]-48)*step;
				step*=10;
			}
		}
		else high_score[i].name[0]='.';
	}	
}

void push_score()
{
	for(int i=0;i<10;i++)
	{
		if (high_score[i].name[0]!='.')
		{
			char buffer[10];
			strcpy(recor[i],high_score[i].name);
			int name_len = strlen(recor[i]);
			itoa(high_score[i].score,buffer,10);
			for(int j=0;j<17-(name_len-strlen(buffer));j++)
				recor[i][strlen(recor[i])]='.';		
			strcat(recor[i],buffer);
		}
		else strcpy(recor[i],"...................");
	}
	Encode(recor,"cat.bmp");
}
	
void ExitGame()
{
	push_score();
	BASS_Free();
	exit(0);
}

void DrawQuad(int x,int y) 
{
	glTranslatef(30*x+1,30*(y-2)+1,0); 
	switch (field[x][y])
	{
	case T: glColor3f(0.5, 0.3, 1.0); break;
	case I: glColor3f(0.0, 1.0, 0.8); break;
	case Z: glColor3f(0.0, 0.0, 1.0); break;
	case S: glColor3f(1.0, 0.0, 0.0); break;
	case J: glColor3f(0.8, 0.8, 0.8); break;
	case L: glColor3f(0.4, 0.4, 0.4); break;
	case O: glColor3f(0.0, 1.0, 0.0); break;
	};
	glBegin(GL_QUADS);	
		glVertex2i(1, 1);			
		glVertex2i(30, 1);
		glVertex2i(30, 30);
		glVertex2i(1, 30);
	glEnd();
	glLoadIdentity(); 
}

int Figure(int figure,int state,int x,int y,int n)
{
	switch (figure)
	{
	case T: if ((state==0) && (!(field[x][y-1] || field[x][y] || field[x-1][y] || field[x+1][y] || (x<1) || (x>8) || (y>21)) || (n==0)))
			{
				field[x][y-1]=n;
				field[x-1][y]=n; //_|_
				field[x][y]=n;
				field[x+1][y]=n;
			}
			else if ((state==1) && (!(field[x][y-1] || field[x][y] || field[x][y+1] || field[x+1][y] || (x>8) || (x<0) || (y>20)) || (n==0)))
			{				
				field[x][y-1]=n;
				field[x][y]=n;   // |_ 
				field[x][y+1]=n; // |
				field[x+1][y]=n;
			}
			else if ((state==2) && (!(field[x-1][y] || field[x][y] || field[x][y+1] || field[x+1][y] || (x<1) || (x>8) || (y>20)) || (n==0)))
			{
				field[x][y]=n;
				field[x+1][y]=n; // ___
				field[x-1][y]=n; //  |
				field[x][y+1]=n;
			}
			else if ((state==3) && (!(field[x][y-1] || field[x][y] || field[x][y+1] || field[x-1][y] || (x<1) || (x>9) || (y>20)) || (n==0)))
			{
				field[x][y]=n;
				field[x][y-1]=n; // _|
				field[x][y+1]=n; //  |
				field[x-1][y]=n;
			}
			else return 1;
			break;
	case I: if ((state%2==0) && (!(field[x][y] || field[x][y+1] || field[x][y+2] || field[x][y+3] || (x<0) || (x>9) || (y>18)) || (n==0)))
			{
				field[x][y]=n;
				field[x][y+1]=n;
				field[x][y+2]=n; // |
				field[x][y+3]=n;
			}
			else if ((state%2==1) && (!(field[x][y] || field[x+1][y] || field[x+2][y] || field[x+3][y] || (x<0) || (x>6) || (y>21)) || (n==0)))
			{
				field[x][y]=n;
				field[x+1][y]=n;
				field[x+2][y]=n; // _
				field[x+3][y]=n;
			}
			else return 1;
			break;
	case Z: if ((state%2==0) && (!(field[x-1][y] || field[x][y] || field[x][y+1] || field[x+1][y+1] || (x<1) || (x>8) || (y>20)) || (n==0)))
			{
				field[x-1][y]=n;
				field[x][y]=n;
				field[x][y+1]=n;     // Z
				field[x+1][y+1]=n;
			}
			else if ((state%2==1) && (!(field[x][y-1] || field[x][y] || field[x-1][y] || field[x-1][y+1] || (x<1) || (x>9) || (y>20)) || (n==0)))
			{
				field[x][y-1]=n;
				field[x][y]=n;
				field[x-1][y]=n;     //  |\|
				field[x-1][y+1]=n;
			}
			else return 1;
			break;
	case S: if ((state%2==0) && (!(field[x+1][y] || field[x][y] || field[x][y+1] || field[x-1][y+1] || (x<1) || (x>8) || (y>20)) || (n==0)))
			{
				field[x+1][y]=n;
				field[x][y]=n;
				field[x][y+1]=n;     // S
				field[x-1][y+1]=n;
			}
			else if ((state%2==1) && (!(field[x][y-1] || field[x][y] || field[x+1][y] || field[x+1][y+1] || (x<0) || (x>8) || (y>20)) || (n==0)))
			{
				field[x][y-1]=n;
				field[x][y]=n;
				field[x+1][y]=n;     //  |/|
				field[x+1][y+1]=n;
			}
			else return 1;
			break;
	case J: if ((state==0) && (!(field[x-1][y] || field[x][y] || field[x][y-1] || field[x-1][y-2] || (x<1) || (x>9) || (y>21)) || (n==0)))
			{
				field[x-1][y]=n;
				field[x][y]=n;
				field[x][y-1]=n;     // j
				field[x][y-2]=n;
			}
			else if ((state==1) && (!(field[x][y-1] || field[x][y] || field[x+1][y] || field[x+2][y] || (x<0) || (x>7) || (y>21)) || (n==0)))
			{
				field[x][y-1]=n;
				field[x][y]=n;
				field[x+1][y]=n;     //  |__
				field[x+2][y]=n;
			}
			else if ((state==2) && (!(field[x+1][y] || field[x][y] || field[x][y+1] || field[x][y+2] || (x<0) || (x>8) || (y>19)) || (n==0)))
			{
				field[x+1][y]=n;
				field[x][y]=n;
				field[x][y+1]=n;     //  Г
				field[x][y+2]=n;
			}
			else if ((state==3) && (!(field[x][y] || field[x-1][y] || field[x-2][y] || field[x][y+1] || (x<2) || (x>9) || (y>20)) || (n==0)))
			{
				field[x][y]=n;
				field[x-1][y]=n;     // ___
				field[x-2][y]=n;     //    |
				field[x][y+1]=n;
			}
			else return 1;
			break;
	case L: if ((state==0) && (!(field[x+1][y] || field[x][y] || field[x][y-1] || field[x][y-2] || (x<0) || (x>8) || (y>21)) || (n==0)))
			{
				field[x+1][y]=n;
				field[x][y]=n;
				field[x][y-1]=n;     // L
				field[x][y-2]=n;
			}
			else if ((state==1) && (!(field[x][y+1] || field[x][y] || field[x+1][y] || field[x+2][y] || (x<0) || (x>7) || (y>20)) || (n==0)))
			{
				field[x][y+1]=n;
				field[x][y]=n;       //   ___
				field[x+1][y]=n;     //  |
				field[x+2][y]=n;
			}
			else if ((state==2) && (!(field[x-1][y] || field[x][y] || field[x][y+1] || field[x][y+2] || (x<1) || (x>9) || (y>19)) || (n==0)))
			{
				field[x-1][y]=n;
				field[x][y]=n;       //    _
				field[x][y+1]=n;     //     |
				field[x][y+2]=n;
			}
			else if ((state==3) && (!(field[x][y] || field[x-1][y] || field[x-2][y] || field[x][y-1] || (x<2) || (x>9) || (y>21)) || (n==0)))
			{
				field[x][y]=n;
				field[x-1][y]=n;     
				field[x-2][y]=n;     //  __|
				field[x][y-1]=n;
			}
			else return 1;
			break;
	case O: if (!(field[x+1][y] || field[x][y] || field[x][y+1] || field[x+1][y+1] || (x<0) || (x>8) || (y>20)) || (n==0))
			{
				field[x+1][y]=n;
				field[x][y]=n;
				field[x][y+1]=n;     // O_o
				field[x+1][y+1]=n;
			}
			else return 1;
			break;
	}
	return 0;
}

void PlayMusic()
{
		if (!BASS_ChannelPlay(stream,TRUE))
	{
		MessageBox(NULL, LPCWSTR("play channel error."), NULL, MB_OK);		
	}
}

void Game_Over()
{
	glTranslatef(0,0,0);			
	glColor3f(0.0,0.0,0.0);
	glBegin(GL_QUADS);
		glVertex2i(1, 1);			
		glVertex2i(300, 1);
		glVertex2i(300, 600);
		glVertex2i(1, 600);
	glEnd();
	glColor3f(1.0,0.0,0.0);
	glRasterPos2i(70, 100);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char *) "GAME OVER");			
	glRasterPos2i(70, 150);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char *) "Enter your name");			
	glBegin(GL_LINE_LOOP);
		glVertex2i(33,200);
		glVertex2i(267,200);
		glVertex2i(267,230);
		glVertex2i(33,230);
	glEnd();
	glRasterPos2i(35, 220);
	glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char *) enter_name);	
}

void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//отрисовка игры
	if (!menu) 
	if (!showscore)
	{
		if (gameover) Game_Over();
		else
		for(int i=0;i<10;i++)	
		for(int j=2;j<22;j++)
		{
			if (field[i][j])
				DrawQuad(i,j);
		}
		
	}
	else
		{
			glColor3f(1.0,0.0,0.0);
			glRasterPos2i(70, 50);
			glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,(unsigned char *)"HIGH SCORES:");
			for(int i=0;i<10;i++)
			{
				glRasterPos2i(60, 100+i*50);
				glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,(unsigned char *)high_score[i].name);
				char buffer[10];
				itoa(high_score[i].score,buffer,10);
				glRasterPos2i(250-(strlen(buffer))*12,100+i*50);
				glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,(unsigned char *)buffer);
			}
		}
	//отрисовка меню
	else
	{		

		strcpy(buf[0],(char *)menu_items[1]);
		strcat(buf[0],itoa(speed,buf[1],10));
		strcpy(buf[2],(char *)menu_items[2]);
		strcpy(buf[3],(char *)menu_items[3]);
		if (music) strcat(buf[2],"ON");
		else strcat(buf[2],"OFF");
		if (sound) strcat(buf[3],"ON");
		else strcat(buf[3],"OFF");
		for(int i=0;i<6;i++)
		{
			if (i==menu_choose) glColor3f(1.0,1.0,0.0);
			else glColor3f(1.0,0.0,0.0);
			glRasterPos2i(100, 150+i*70);
			if (i==1) glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,(unsigned char *)buf[0]);
			else if (i==2) glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,(unsigned char *)buf[2]);
			else if (i==3) glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,(unsigned char *)buf[3]);
			else glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24,menu_items[i]);
		}
	}
	glutSwapBuffers();
}

int NewShape()
{
	shape=rand()%7+1;
	sh_st=rand()%4;
	X=5;
	Y=2;
	return Figure(shape,sh_st,X,Y,shape);
}

void CheckLines()
{	
	int qline=0, i=21;
	while(i>0)
	{
		int q=0;
		for(int j=0;j<10;j++)
			if (field[j][i]) q++;
		if (q==10) 
		{
			for(int k=i;k>0;k--)
			for(int j=0;j<10;j++)			
				field[j][k]=field[j][k-1];
			for(int j=0;j<10;j++)
				field[j][0]=0;
			score+=10*(++qline);
			if (sound) PlaySound(TEXT("sounds/clear.wav"),NULL,SND_ASYNC|SND_FILENAME);
		}
		else i--;		
	}	
	if ((!qline) && (sound)) PlaySound(TEXT("sounds/click.wav"),NULL,SND_ASYNC|SND_FILENAME);
	if (score>=score_lvl[speed-1]) speed++;
	strcpy(buf[0],sc);
	strcat(buf[0],itoa(score,buf[2],10));
	strcpy(buf[1],sp);
	strcat(buf[0]," ");
	strcat(buf[1],itoa(speed,buf[2],10));
	strcat(buf[0],buf[1]);
	glutSetWindowTitle(buf[0]);
}

void Timer(int)
{
	glutPostRedisplay();
	if ((!menu) && (!showscore))
	{
		if (!gameover)
		{
 		Figure(shape,sh_st,X,Y,0);
		if (!Figure(shape,sh_st,X,Y+1,shape)) Y++;
		else
		{
			Figure(shape,sh_st,X,Y,shape);
			CheckLines();
			if (NewShape()) gameover=1;				
		}
		}
	}
	glutTimerFunc(1000-speed*100,Timer,0);
}

void Keyboard(unsigned char key,int x,int y)
{
	if (key==32)	
	if ((!menu) && (!gameover) && (!showscore))
	{
		Figure(shape,sh_st,X,Y,0);
		while(!Figure(shape,sh_st,X,Y+1,shape)) 
		{
			Draw();
			Figure(shape,sh_st,X,Y+1,0);
			Y++;
		}
		Figure(shape,sh_st,X,Y,shape);
 		CheckLines();
		if (NewShape()) gameover=1;	
		Draw();
	}	
	if (key==13) 
	{
	if (menu)
	{
		switch (menu_choose)
		{
		case 0: 
			{
				for(int i=0;i<10;i++)
				for(int j=0;j<22;j++)
					field[i][j]=0;
				menu=0; 
				score=0;	
				showscore=0;
				if (music) PlayMusic();
				NewShape();
			break;
			}
		case 4: 
			{
				showscore=1; 
				menu=0;
				break;
			}
		case 5: ExitGame(); break;
		}
	}
	if (gameover) 
	{
		int i=0;
		while((score<=high_score[i].score)&&(high_score[i].name[0]!='.'))
			i++;
		for(int j=9;j>i;j--)
			high_score[j]=high_score[j-1];
		strcpy(high_score[i].name,enter_name);
		high_score[i].score=score;
		//дописать добавление строки рекордов
		memset(enter_name,0,20);
		gameover=0;
		menu=0;
		showscore=1;
		Draw();
	}
	}
	if (key==27) 
	if (!menu) 
		{
			menu=1;				
			showscore=0;
			speed=0;
			PlaySound(NULL, 0, 0);
		}
		else ExitGame();
	if (gameover)
	{
		if ((key>47) && (key<127))	
 			if (strlen(enter_name)<10)
			{
				enter_name[strlen(enter_name)]=key;	
				Draw();
			}
		if (key==8) enter_name[strlen(enter_name)-1]=0;		
	}
	Draw();
}

void SKeyboard(int button,int x,int y)
{
	switch(button)
	{
	case (GLUT_KEY_DOWN): 
		{
			if (menu)
			{
				menu_choose++;
				if (sound) PlaySound(TEXT("sounds/select.wav"), NULL, SND_ASYNC|SND_FILENAME);
				menu_choose%=6;
			}
			else 
			{
				Figure(shape,sh_st,X,Y,0);
				if (!Figure(shape,sh_st,X,Y+1,shape)) Y++;
				else Figure(shape,sh_st,X,Y,shape);
			}
			break;
		}
	case (GLUT_KEY_LEFT): 
		{
			if (menu)
			{
				if ((menu_choose==1) && (speed>1)) speed--;
				if (menu_choose==2) music=!music;
				if (menu_choose==3) sound=!sound;
			}
			else
			{
				Figure(shape,sh_st,X,Y,0);
				if (!Figure(shape,sh_st,X-1,Y,shape)) X--;			
				else Figure(shape,sh_st,X,Y,shape);
			}
			break;}
	case (GLUT_KEY_RIGHT): 
		{
			if (menu)
			{
				if ((menu_choose==1) && (speed<9)) speed++;
				if (menu_choose==2) music=!music;
				if (menu_choose==3) sound=!sound;
			}
			else
			{
				Figure(shape,sh_st,X,Y,0);
				if (!Figure(shape,sh_st,X+1,Y,shape)) X++;			
				else Figure(shape,sh_st,X,Y,shape);
			}
			break;
		}
	case (GLUT_KEY_UP):
		{
			if (menu)
			{
				if (--menu_choose==-1) menu_choose=5;
				if (sound) PlaySound(TEXT("sounds/select.wav"), NULL, SND_ASYNC|SND_FILENAME);
				menu_choose%=6;
			}
			Figure(shape,sh_st,X,Y,0);
			if (!Figure(shape,(sh_st+1)%4,X,Y,shape))
			{
				sh_st++;
				sh_st%=4;
			}
			else Figure(shape,sh_st,X,Y,shape);
			break;
		}
		}
	Draw();
}

void GraphInit(int &argc,char** argv)
{
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WinWid, WinHei);
    glutInitWindowPosition(100, 100);
    int hwnd=glutCreateWindow("Tetris");
    glutDisplayFunc(Draw);
	glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,WinWid,WinHei,0,-15.0,15.0);
    glMatrixMode(GL_MODELVIEW);	
	glutTimerFunc(1000,Timer,0);
	glutSpecialFunc(SKeyboard);
	glutKeyboardFunc(Keyboard);
    glutMainLoop();
}

int SoundInit()
{
	if (HIWORD(BASS_GetVersion())!=BASSVERSION) 
	{
		MessageBox(0,LPCWSTR("Sound init error"),0,MB_OK);
		return 1;
	}
	if (!BASS_Init (-1, 44100, BASS_DEVICE_3D , 0, NULL)) 
	{
		MessageBox(NULL, LPCWSTR("BASS initialization error"), NULL, MB_OK);
		return 1;
	}
	char fname[]="test3.mp3";
	stream =BASS_StreamCreateFile(FALSE,fname, 0,  0,  0 );
	if (!stream) 
	{
		MessageBox(NULL, LPCWSTR("load error."), NULL, MB_OK);
		return 1;
	}
	return 0;
}

int main(int argc, char** argv)
{		
	//Decode(recor,"cat.bmp");
	srand(time(0));
	get_score();
	if (SoundInit()) exit(1);
	GraphInit(argc,	argv);
}