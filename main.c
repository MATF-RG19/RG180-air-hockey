#include <GL/glut.h>
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include <stdio.h>
#include "image.h"


#define PI 3.1415926535
#define EPSILON 0.01
#define TIMER_ID 0
#define TIMER_INTERVAL 25
#define FILENAME1 "AirHockeySurface.bmp"


const static float size = 0.2;

static GLuint names[2];
static float matrix[16];

/*tekuce koordinate lopte*/
static float x_curr, z_curr;
/*vektor brzine*/
static float v_x, v_z;   
static float first_x = -4,first_z= 0;
static float second_x = 4,second_z= 0;
static int count_fsd = 0, count_scd = 0;
static void init_image();

static int animation_ongoing;

/*fj-e*/
static void on_display(void);
static void on_timer(int value);
static void on_keyboard(unsigned char key, int x, int y);
static void on_reshape(int width, int height);
void draw_object();
void draw_ball();
void draw_ground();
void light();
void collision();

int main(int argc, char **argv){
    
    /*inicijalizacija*/
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    /*kreiramo prozor*/
    glutInitWindowSize(900, 580);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);
    
    /*callback funkcije*/
    glutDisplayFunc(on_display);
    glutKeyboardFunc(on_keyboard);
    glutReshapeFunc(on_reshape);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,1);
    
    light();
    
    srand(time(NULL));

    
    /*inicijalizacija koordinata*/
    x_curr = 0;
    z_curr = 0;

    /*inicjalizacija vektora*/
    v_x = -size / 2 + size * rand() / (float) RAND_MAX;
    v_z = -size / 2 + size * rand() / (float) RAND_MAX;
    
    animation_ongoing = 0;

    /*openGl inicjalizacija*/
    glClearColor(0,0,0,0);
    
    init_image();
    
    /*poziv glavne petlje*/
    glutMainLoop();
    
    exit(EXIT_SUCCESS);
}

void light(){
        
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
    float light_position[] = {-1, 3, 2, 1};
    
    /* Ambijentalna boja svetla. */
    float light_ambient[] = {.3f, .3f, .3f, 1};
    
    /* Difuzna boja svetla. */
    float light_diffuse[] = {.7f, .7f, .7f, 1};
    
    /* Spekularna boja svetla. */
    float light_specular[] = {.7f, .7f, .7f, 1};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
}



static void on_display(void){
    
    /*Brisemo prethodni sadrzaj frejma*/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    /* Podesava se pogled. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt( 
                0, 6, -10,
                0, 0, 0,
                0, 1, 0 
             );
    
    
    
    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0.3, 0.3, 0.3, 1 };

    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = { 0.0, 0.0, 0.8, 1 };

    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };

    /* Koeficijent glatkosti materijala. */
    GLfloat shininess = 40;
  
    
        /* Podesavaju se parametri materijala. */
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    
    /*iscrtavanje prvog igraca*/
    glPushMatrix();
        glTranslatef(0,0,first_z);
        GLfloat diffuse_coeffs_red[] = { 1, 0.1, 0.1, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs_red);
        glTranslatef(first_x,0.1,0);
        draw_object();
    glPopMatrix();
    
    /*iscrtavanje drugog igraca*/
    glPushMatrix();
        glTranslatef(0,0,second_z);
        GLfloat diffuse_coeffs_blue[] = { 0.1, 0.1, 1, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs_blue);
        glTranslatef(second_x,0.1,0);
        draw_object();
    glPopMatrix();
    
    
    /*crtanje terena*/
    glPushMatrix();
        GLfloat diffuse_coeffs_white[] = { 1, 1, 1, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs_white);
        draw_ground();
    glPopMatrix();
    
        /*iscrtavanje lopte*/
    glPushMatrix();
        glTranslatef(x_curr, 0 , z_curr);
        GLfloat diffuse_coeffs_black[] = { 0, 0, 0, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs_black);
        glTranslatef(0,0.2,0);
        draw_ball();
    glPopMatrix();
    
    
    
    /*Novu sliku saljemo na ekran*/
    glutSwapBuffers();
}



static void on_keyboard(unsigned char key, int x, int y){
    switch(key){
        case 27:
            exit(EXIT_SUCCESS);
            break;
        case 'g':
        case 'G':
            if(!animation_ongoing){
                animation_ongoing = 1;
                glutTimerFunc(TIMER_INTERVAL,on_timer,TIMER_ID);
            }
            break;
        case 's':
        case 'S':
            animation_ongoing = 0;
            break;
        case 'r':
        case 'R':
            x_curr = 0;
            z_curr = 0;
            first_z = 0;
            second_z = 0;
            glutPostRedisplay();
            break;
        case 'o':
        case 'O':
            if(first_z <= 3.8)
                first_z += 0.3;
            break;
        case 'k':
        case 'K':
            if(first_z >= -3.8)
                first_z -= 0.3;
            break;
        case 'w':
        case 'W':
            if(second_z <= 3.8)
                second_z += 0.3;
            break;
        case 'a':
        case 'A':
            if(second_z >= -3.8)
                second_z -= 0.3;
        break;

    }
    
}


static void on_reshape(int width, int height){
    /*pamte se sirina i visina prozora*/
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 3000);
}

void collision(){
    
}


/*iscrtavanje plocice */
void draw_object(){
    glPushMatrix();
        glScalef(0.5,0.1,2);
        glutSolidCube(1);
    glPopMatrix();
}

/*crtanje terena*/
void draw_ground(){
    glPushMatrix();
//         glBindTexture(GL_TEXTURE_2D, names[0]);
//         glBegin(GL_QUADS);
//             glNormal3f(0, 0, 1);
// 
//             glTexCoord2f(0, 0);
//             glVertex3f(-990, -580, 0);
// 
//             glTexCoord2f(0,1);
//             glVertex3f(-990, 580, 0);
// 
//             glTexCoord2f(1, 1);
//             glVertex3f(900, 580, 0);
// 
//             glTexCoord2f(1, 0);
//             glVertex3f(900, -580, 0);
//         glEnd();
//         glBindTexture(GL_TEXTURE_2D, 0);
        glScalef(10,0.1,10);
        glutSolidCube(1);
    glPopMatrix();
}

/*crtanje lopte*/
void draw_ball(){
    glPushMatrix();
        glutSolidSphere(size,20,20);
    glPopMatrix();
}

static void on_timer(int value){
    if(value != TIMER_ID)
        return;
    
    x_curr += v_x*2;
    if(x_curr  <= -(5 - size / 2)){
        v_x *= -1;
        count_scd++;
        animation_ongoing = 0;
        x_curr = 0;
        z_curr = 0;
        first_x = -4;
        first_z = 0;
        second_x = 4;
        second_z = 0;
    }
    if(x_curr  >= (5 - size / 2)){
        v_x *= -1;
        count_fsd++;
        animation_ongoing = 0;
        x_curr = 0;
        z_curr = 0;
        first_x = -4;
        first_z = 0;
        second_x = 4;
        second_z = 0;
        
    }
    
    if(x_curr <= -3.55 && z_curr >= first_z - 1 &&  z_curr <= first_z + 1){
        v_x *= -1;
    }
    
    if(x_curr >= 3.55 && z_curr >= second_z - 1 &&  z_curr <= second_z + 1){
        v_x *= -1;
    }
    
    
    printf("%d %d\n", count_fsd, count_scd);
    
    z_curr += v_z*2;
    if(z_curr <= -(5 - size / 2) || z_curr >= (5 - size / 2)){
        v_z *= -1;
    }
    
    
    
    
    /*fsd crveni*/
        
    /*ponovno iscrtavanje prozora*/
    glutPostRedisplay();
    
    if(animation_ongoing){
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
    
}


static void init_image(void)
{
    /* Objekat koji predstavlja teskturu ucitanu iz fajla. */
    Image * image;

    /* Postavlja se boja pozadine. */

    /* Ukljucuje se testiranje z-koordinate piksela. */
    glEnable(GL_DEPTH_TEST);

    /* Ukljucuju se teksture. */
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_REPLACE);

    /*
     * Inicijalizuje se objekat koji ce sadrzati teksture ucitane iz
     * fajla.
     */
    image = image_init(900, 580);


    /* Generisu se identifikatori tekstura. */
    glGenTextures(2, names);


    /* Kreira se druga tekstura. */
    image_read(image, FILENAME1);

    glBindTexture(GL_TEXTURE_2D, names[1]);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 image->width, image->height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, image->pixels);

    /* Iskljucujemo aktivnu teksturu */
    glBindTexture(GL_TEXTURE_2D, 0);

    /* Unistava se objekat za citanje tekstura iz fajla. */
    image_done(image);

    /* Inicijalizujemo matricu rotacije. */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
}





