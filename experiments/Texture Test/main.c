#include <stdio.h>
#include <GL/glut.h>
#include <windows.h>
#include <math.h>

float ex=0, ey=0, ez=0, cx=0, cy=0, cz=0;
float angle=0, anglex=0;
int Rotation=0, UpDown=0;
int height, width;
GLuint Texture;

void render(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	gluLookAt(ex, ey, ez,	0,0,0, 	0,1,0);
	
	glRotatef(anglex*50, 1,0,0);
	glRotatef(angle*50, 0,1,0);
	glScalef(3,3,3);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, Texture);
	int i;
	for(i=0; i<4; i++){
		glPushMatrix();
		glRotatef(90*i, 0,1,0);
		glTranslatef(0,0,-1);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(-1,-1,0);
			
			glTexCoord2f(1,0);
			glVertex3f(1,-1,0);
			
			glTexCoord2f(1,1);
			glVertex3f(1,1,0);
			
			glTexCoord2f(0,1);
			glVertex3f(1,-1,0);
		glEnd();
		glPopMatrix();
	}
	for(i=0; i<2; i++){
		glPushMatrix();
		glRotatef(100*i+90, 1,0,0);
		glTranslatef(0,0,-1);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(-1,-1,0);
			
			glTexCoord2f(1,0);
			glVertex3f(1,-1,0);
			
			glTexCoord2f(1,1);
			glVertex3f(1,1,0);
			
			glTexCoord2f(0,1);
			glVertex3f(1,-1,0);
		glEnd();
		glPopMatrix();
	}
	glutSwapBuffers();
	glutPostRedisplay();
}

void screenResize(int w, int h) {
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	gluPerspective(45, ratio, 0.1f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
}

int move=0;
int prev_x=0;
int prev_y=0;

void mouseButton(int button, int state, int x, int y) {
	Rotation = (button-GLUT_LEFT_BUTTON) || (state-GLUT_DOWN);
	Rotation = !Rotation;
	if(Rotation){prev_x = x;}
	
	UpDown = (button-GLUT_RIGHT_BUTTON) || (state-GLUT_DOWN);
	UpDown = !UpDown;
	if(UpDown){prev_y = y;}
	
	move = (button-GLUT_MIDDLE_BUTTON) || (state-GLUT_DOWN);
	move = !move;
	if(move){prev_y = y;}
}

void motion(int x, int y){
	if(Rotation){angle += 0.015*(x-prev_x);}
	if(UpDown){anglex += 0.015*(y-prev_y);}
	if(move){cz += 0.1*(prev_y-y);}
	prev_x = x;
	prev_y = y;
}




int height, width;
unsigned char *img;

void BitmapReader(char *filename){
	img = NULL;
	
	tagBITMAPFILEHEADER fh;
	tagBITMAPINFOHEADER infoh;
	FILE *fp = fopen(filename, "r");
	if(!fp){
		printf("\aFailed to read file\n");
		return;
	}
	
	fread(&fh, sizeof(fh), 1, fp);
	fread(&finfoh, sizeof(finfoh), 1, fp);
	
	if(finfoh.biBitCount != 24){
		printf("\aEnter a 24bit BMP file\n");
		return;
	}
	
	width = infoh.biWidth;
	height = abs(infoh.biHeight);
	img = infoh.biSizeImage;
	
	fread(img, infoh.biSizeImage, 1, fp);
	unsigned char temp;
	int i;
	for(i=0; i<infoh.biSizeImage; i++){
		temp = img[i];
		img[i] = img[i+2];
		img[i+2] = temp;
		i+=2;
	}
}

GLuint LoadTexture(char *filename){
	GLuint ID;
	BitmapReader(filename);
	
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return ID;
}

void keyboard(unsigned char key, int x, int y){
	char file[80];
	if(key == 'a'){
		printf("Enter texture location : ");
		scanf("%s", &file);
		Texture = LoadTexture(file);
		fclose(stdin);
	}
}

int main(int argc, char **argv) {
	// init GLUT and create window
	printf("initializing");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(820,100);
	glutInitWindowSize(500,400);
	glutCreateWindow("render");

	// register callbacks
	glutDisplayFunc(render);
	glutReshapeFunc(screenResize);
	glutMouseFunc(mouseButton);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);

	// OpenGL init
	glEnable(GL_DEPTH_TEST);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 0;
}
