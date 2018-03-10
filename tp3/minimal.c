#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>


#define NB_SEGMENTS 100
/* Dimensions de la fenêtre */
static unsigned int WINDOW_WIDTH = 800;
static unsigned int WINDOW_HEIGHT = 600;

/* Nombre de bits par pixel de la fenêtre */
static const unsigned int BIT_PER_PIXEL = 32;

/* Nombre minimal de millisecondes separant le rendu de deux images */
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

typedef struct Point {
	float x, y;
	unsigned char r, g, b;
	struct Point* next;
} Point, *PointList;

typedef struct Primitive{
	GLenum primitiveType;
	PointList points;
	struct Primitive* next;
} Primitive, *PrimitiveList;


Point* allocPoint(float x, float y, unsigned char r, unsigned char g, unsigned char b) {
    /*
    On alloue un espace mémoire suffisant pour pouvoir stocker un point
    Attention : la fonction malloc() renvoie un void* qu'il faut impérativement caster en Point*.
    */
    Point* point = (Point*) malloc(sizeof(Point));
    if (!point) {
    	return NULL;
    }
    point->x = x;
    point->y = y;
    point->r = r;
    point->g = g;
    point->b = b;
    point->next = NULL;
    return point;
}

void addPointToList(Point* point, PointList* list) {
	assert(point);
	assert(list);
	if (*list == NULL) {
        /* Si le pointeur sur la liste est vide, on le modifie pour le faire pointer sur le point donné en paramètre */
		*list = point;
	} else {
        /* Sinon, on réapplique la même fonction mais sur le point suivant */
		addPointToList(point, &(*list)->next);
	}
}

void rotation(float a){
	glRotatef(a,0.0,0.0,1.0);
}

void taille(float x, float y, float z){
	glScalef(x,y,z);
}


void drawPoints(PointList list) {
	while(list) {
		glColor3ub(list->r, list->g, list->b);
		glVertex2f(list->x, list->y);
		list = list->next;
	}
}

void deletePoints(PointList* list) {
	assert(list);
	while (*list) {
		Point* next = (*list)->next;
		free(*list);
		*list = next;
	}
}

Primitive* allocPrimitive(GLenum primitiveType) {
    /*
    On alloue un espace mémoire suffisant pour pouvoir stocker une primitive
    Attention : la fonction malloc() renvoie un void* qu'il faut impérativement caster en Primitive*.
    */
    Primitive* primitive = (Primitive*) malloc(sizeof(Primitive));
    if (!primitive) {
    	return NULL;
    }
    primitive->primitiveType = primitiveType;
    primitive->points = NULL;
    primitive->next = NULL;
    return primitive;
}

void addPrimitive(Primitive* primitive, PrimitiveList* list) {
	assert(primitive);
	assert(list);
	primitive->next = (*list);
	(*list) = primitive;
}

void drawPrimitives(PrimitiveList list) {
	while(list) {
		glBegin(list->primitiveType);
		drawPoints(list->points);
		glEnd();
		list = list->next;
	}
}

void deletePrimitive(PrimitiveList* list) {
	assert(list);
	while(*list) {
		Primitive* next = (*list)->next;
		deletePoints(&(*list)->points);
		free(*list);
		*list = next;
	}
}

void drawSquare(){
	glColor3ub(255,0,0);
	glBegin(GL_QUADS);
	glVertex2f(-0.5, 0.5);
	glVertex2f(0.5,0.5);
	glVertex2f(0.5,-0.5);
	glVertex2f(-0.5,-0.5);
	glEnd();
}


void drawLandmarks(){
	float i;
	glBegin(GL_LINES);
	for(i=-4; i<4; i+=1){
		glColor3ub(255,255,255);
		glVertex2f(i,-0.05);
		glVertex2f(i,0.05);
		glVertex2f(-0.05,i);
		glVertex2f(0.05,i);
	}

	glEnd();
}

void drawLandmark(){
	glBegin(GL_LINES);
	glColor3ub(255,0,0);
	glVertex2f(-4, 0);
	glVertex2f(4,0);
	glColor3ub(0,255,0);
	glVertex2f(0,-3);
	glVertex2f(0,3);
	glEnd();
}


void drawCircle(){
	int i;
	glBegin(GL_POLYGON);
	float delta = 2 * M_PI / (float) NB_SEGMENTS;
	for(i=0; i<100; i++){
        glColor3ub(255,255,255);
		float x = (cos(i * delta));
		float y = (sin(i * delta));
		glVertex2f(x,y);
	}
	glEnd();
}

void drawCarre(float x, float y,float largeur, float longueur, int r, int v, int b){
	float x2 = x + longueur;
	float y2 = y + largeur;
    glBegin(GL_QUADS);
        glColor3ub(r,v,b);
        glVertex2f(x, y);
        glVertex2f(x2, y);
        glVertex2f(x2, y2);
        glVertex2f(x, y2);
    glEnd();
}

/* carré arrondis */ 
void drawRoundedSquare(float x, float y){
   glPushMatrix();
        glScalef(0.25,0.25,0);
        glTranslatef(-2*x,2*y,0);
        drawCircle();
   glPopMatrix();

   glPushMatrix();
        glScalef(0.25,0.25,0);
        glTranslatef(2*x,2*y,0);
        drawCircle();
   glPopMatrix();

   glPushMatrix();
        glScalef(0.25,0.25,0);
        glTranslatef(-2*x,-2*y,0);
        drawCircle();
   glPopMatrix();

   glPushMatrix();
        glScalef(0.25,0.25,0);
        glTranslatef(2*x,-2*y,0);
        drawCircle();
   glPopMatrix();

  glPushMatrix();
      glScalef(0.5,0.5,0);
    drawCarre(-1*x,-1.5*y,(3*x),(2*y),255,255,255);
  glPopMatrix();

   glPushMatrix();
    glScalef(0.5,0.5,0);
    drawCarre(-1.5*x,-1*y,(2*x),(3*y),255,255,255);
   glPopMatrix();
}



GLuint createdrawFirstArm(){
GLuint first;
first = glGenLists(1);

glNewList(first, GL_COMPILE);
/* 2 cercles et parralélépipède */ 
glPushMatrix();
    glScalef(2,2,0);
   glPushMatrix();
        glTranslatef(-0.3,0,0);
        glScalef(0.2,0.2,0);
        drawCircle();
   glPopMatrix();

   glPushMatrix();
        glTranslatef(0.3,0,0);
        glScalef(0.1,0.1,0);
        drawCircle();
   glPopMatrix();

/* quadrilapède central   */
    glPushMatrix();
    glBegin(GL_POLYGON);
        glVertex2f(-0.3,-0.2);
        glVertex2f(0.3, -0.1);
        glVertex2f(0.3,0.1);
        glVertex2f(-0.3,0.2);
    glEnd();
    glPopMatrix();
  glPopMatrix();

glEndList();
return first;
}

GLuint createdrawSecondArm(){
GLuint second;
second = glGenLists(1);
glNewList(second, GL_COMPILE);

 glPushMatrix();
    glScalef(0.20,0.20,0);
    
 glPushMatrix();  
    glTranslatef(3.,0,0);
    drawRoundedSquare(1,1);
 glPopMatrix();
	
 glPushMatrix();
    glTranslatef(8,0,0);
    drawRoundedSquare(1,1);
 glPopMatrix();

 glPushMatrix();
    glTranslatef(2,-1.25,0);
    drawCarre(1,1,0.6,4.6,255,255,255);
 glPopMatrix();

 glPopMatrix();

glEndList();
return second;
}

GLuint createdrawThirdArm(){
GLuint third;
third = glGenLists(1);
glNewList(third, GL_COMPILE);

glPushMatrix();
    glScalef(0.20,0.20,0);

glPushMatrix();
    glTranslatef(8,0,0); 
    glScalef(0.5,0.5,0);
    drawRoundedSquare(1,1);
glPopMatrix();

 glPushMatrix();
    glTranslatef(7,-1.25,0);
    drawCarre(1,1,0.4,4,255,255,255);
 glPopMatrix();

 glPushMatrix();
        glTranslatef(12,0,0);
        glScalef(0.4,0.4,0);
        drawCircle();
 glPopMatrix();

glPopMatrix();
glEndList();
return third;
}


void Arm(){

glPushMatrix();
glPushMatrix();
       glRotatef(45,0.0,0.0,1.0);
       createdrawFirstArm();
glPopMatrix();

glPushMatrix();
       glTranslatef(-0.1,0.4,0);
       createdrawSecondArm();
glPopMatrix();

glPushMatrix();
       glTranslatef(0.1,-0.5,0);
       glRotatef(35,0.0,0.0,1.0);
       createdrawThirdArm();
glPopMatrix();

glPushMatrix();
       glTranslatef(0.2,1.3,0);
       glRotatef(-35,0.0,0.0,1.0);
       createdrawThirdArm();
glPopMatrix();

glPushMatrix();
       glTranslatef(0,-.05,0);
       glRotatef(15,0.0,0.0,1.0);
       createdrawThirdArm();
glPopMatrix();

glPushMatrix();
       glTranslatef(0,0.8,0);
       glRotatef(-15,0.0,0.0,1.0);
       createdrawThirdArm();
glPopMatrix();

glPopMatrix();

}

void resizeViewport() {
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    //glMatrixMode(GL_PROJECTION);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(-4., 4., -3., 3.);
	SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE);
}


static const unsigned char COLORS[] = {
	255, 255, 255,
	0, 0, 0,
	255, 0, 0,
	0, 255, 0,
	0, 0, 255,
	255, 255, 0,
	0, 255, 255,
	255, 0, 255
};

static const unsigned int NB_COLORS = sizeof(COLORS) / (3 * sizeof(unsigned char));

void drawColorPalette() {
	int i;
    float dx = 2.f / NB_COLORS; // dx = "delta x"
    glBegin(GL_QUADS);
    for(i = 0; i < NB_COLORS; ++i) {
    	glColor3ubv(COLORS + i * 3);
    	glVertex2f(-1 + i * dx, -1);
    	glVertex2f(-1 + (i + 1) * dx, -1);
    	glVertex2f(-1 + (i + 1) * dx, 1);
    	glVertex2f(-1 + i  * dx, 1);
    }
    glEnd();
}

int main(int argc, char** argv) {
 

    /*SDL_GetTicks retourne le nombre de millisecondes ecoulées depuis le lancement du programme */
	//Uint32 randomStartTime = SDL_GetTicks();

	//int iterationTime=5.0; // 5 secondes par déplacement

    /* Initialisation de la SDL */
	if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
		return EXIT_FAILURE;
	}

    /* Ouverture d'une fenêtre et création d'un contexte OpenGL */
	if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE)) {
		fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
		return EXIT_FAILURE;
	}
	SDL_WM_SetCaption("Paint IMAC", NULL);

	glClearColor(0.1, 0.1, 0.1, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
    resizeViewport();

    /* On créé une première primitive par défaut */
	PrimitiveList primitives = allocPrimitive(GL_LINE_STRIP);
	
   	GLuint first = createdrawFirstArm();
	GLuint second = createdrawSecondArm();
	GLuint third = createdrawThirdArm();

	int loop = 1;
    int mode = 0; // le mode d'affichage. 0 = le dessin, 1 = la palette
   // unsigned int currentColor = 0; // l'index de la couleur courante dans le tableau COLORS

    /* Boucle d'affichage */
    while(loop) {
        /* Récupération du temps au début de la boucle */
    	Uint32 startTime = SDL_GetTicks();

        /* Code de dessin */

        glClear(GL_COLOR_BUFFER_BIT); // Toujours commencer par clear le buffer


        drawLandmarks();
        drawLandmark();
	//drawRoundedSquare(1.,1.);
       
	//Arm();
	glPushMatrix();

	glPushMatrix();
	       glRotatef(45,0.0,0.0,1.0);
	       glCallList(first);
	glPopMatrix();

	glPushMatrix();
	       glTranslatef(-0.1,0.4,0);
	       glCallList(second);
	glPopMatrix();

	glPushMatrix();
	       glTranslatef(0.1,-0.5,0);
	       glRotatef(35,0.0,0.0,1.0);
	       glCallList(third);
	glPopMatrix();

	glPushMatrix();
	       glTranslatef(0.2,1.3,0);
	       glRotatef(-35,0.0,0.0,1.0);
	       glCallList(third);
	glPopMatrix();
		
	glPushMatrix();
	       glTranslatef(0,-.05,0);
	       glRotatef(15,0.0,0.0,1.0);
	       glCallList(third);
	glPopMatrix();

	glPushMatrix();
	       glTranslatef(0,0.8,0);
	       glRotatef(-15,0.0,0.0,1.0);
	       glCallList(third);
	glPopMatrix();

	glPopMatrix();

        if (mode == 0) {
            drawPrimitives(primitives); // On dessine la liste de primitives
        }
        else if (mode == 1) {
        	drawColorPalette();
        }

        /* Boucle traitant les evenements */
        SDL_Event e;
        while(SDL_PollEvent(&e)) {

            /* L'utilisateur ferme la fenêtre : */

        	if(e.type == SDL_QUIT) {
        		loop = 0;
        		break;
        	}

            /* Quelques exemples de traitement d'evenements : */
        	switch(e.type) {

                /* Touche clavier */
        		case SDL_KEYDOWN:

        		printf("touche pressée (code = %d)\n", e.key.keysym.sym);

        		if (e.key.keysym.sym == SDLK_SPACE) {
        			mode = 1;
        		}

        		switch(e.key.keysym.sym) {

        			case SDLK_q:
        			loop = 0;
        			break;

        			case SDLK_SPACE:
        			mode = 1;
        			break;

        			case SDLK_o:
                    break;

                    case SDLK_m:
        			//glRotatef(-45,0.0,0.0,1.0);
        			//glTranslatef(0.2,0,0);
                    break;

                    case SDLK_n:
                    glTranslatef(0.2,0,0);
                    glRotatef(-45,0.0,0.0,1.0);
                   // drawCarre(&primitives,0.2,0.7,0.3,0.2,200,200,200);
                    break;

                    case SDLK_UP:
                    glTranslatef(0,0.2,0);
                    break;

                    case SDLK_DOWN:
                    glTranslatef(0,-0.2,0);
                    break;

                    case SDLK_LEFT:
                    glTranslatef(-0.2,0,0);
                    break;


                    case SDLK_RIGHT:
                    glTranslatef(0.2,0,0);
                    break;

                    case SDLK_p:
                    addPrimitive(allocPrimitive(GL_POINTS), &primitives);
                    break;

                    case SDLK_c:
                            /* Touche pour effacer le dessin */
                            deletePrimitive(&primitives); // on supprime les primitives actuelles
                            addPrimitive(allocPrimitive(GL_POINTS), &primitives); // on réinitialise à la primitive courante
                            break;

                            default:
                            break;
                        }

                        break;

                        case SDL_KEYUP:
                        if (e.key.keysym.sym == SDLK_SPACE) {
                        	mode = 0;
                        }
                        break;

                        case SDL_VIDEORESIZE:
                        WINDOW_WIDTH = e.resize.w;
                        WINDOW_HEIGHT = e.resize.h;
                        resizeViewport();

                        default:
                        break;
                    }
                }

        /* Echange du front et du back buffer : mise à jour de la fenêtre */
                SDL_GL_SwapBuffers();

        /* Calcul du temps écoulé */
                Uint32 elapsedTime = SDL_GetTicks() - startTime;

        /* Si trop peu de temps s'est écoulé, on met en pause le programme */
                if(elapsedTime < FRAMERATE_MILLISECONDS) {
                	SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
                }
            }

            deletePrimitive(&primitives);

    /* Liberation des ressources associées à la SDL */ 
            SDL_Quit();

            return EXIT_SUCCESS;
        }