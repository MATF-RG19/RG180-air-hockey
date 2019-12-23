#include <GL/glut.h>
#include <stdlib.h> 
#include <time.h>
#include <math.h>

#define PI 3.1415926535
#define EPSILON 0.01
#define TIMER_ID 0
#define TIMER_INTERVAL 50


const static float size = 0.2;

static int window_height, window_width;
/*tekuce koordinate lopte*/
static float x_curr, z_curr;
/*vektor brzine*/
static float v_x, v_z;   
static float first_x,first_z= 0;
static float second_x,second_z= 0;


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

int main(int argc, char **argv){
    
    /*inicijalizacija*/
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    
    /*kreiramo prozor*/
    glutInitWindowSize(800,600);
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
    x_curr = -(1 - size / 2) + (2 - size) * rand() / (float) RAND_MAX;
    z_curr = -(1 - size / 2) + (2 - size) * rand() / (float) RAND_MAX;

    /*inicjalizacija vektora*/
    v_x = -size / 2 + size * rand() / (float) RAND_MAX;
    v_z = -size / 2 + size * rand() / (float) RAND_MAX;
    
    animation_ongoing = 0;

    /*openGl inicjalizacija*/
    glClearColor(0,0,0,0);
    
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
        glTranslatef(4,0.1,0);
        draw_object();
    glPopMatrix();
    
    /*iscrtavanje drugog igraca*/
    glPushMatrix();
        glTranslatef(0,0,second_z);
        GLfloat diffuse_coeffs_blue[] = { 0.1, 0.1, 1, 1 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_coeffs_blue);
        glTranslatef(-4,0.1,0);
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
            if(second_z <= 3.8)
                second_z += 0.3;
            break;
        case 'k':
        case 'K':
            if(second_z >= -3.8)
                second_z -= 0.3;
            break;
            
        case 'w':
        case 'W':
            if(first_z <= 3.8)
                first_z += 0.3;
            break;
        case 'a':
        case 'A':
            if(first_z >= -3.8)
                first_z -= 0.3;
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
    if(x_curr <= -(5 - size / 2) || x_curr >= (5 - size / 2)){
        v_x *= -1;
    }
    z_curr += v_z*2;
    if(z_curr <= -(5 - size / 2) || z_curr >= (5 - size / 2)){
        v_z *= -1;
    }
    
    
    /*ponovno iscrtavanje prozora*/
    glutPostRedisplay();
    
    if(animation_ongoing){
        glutTimerFunc(TIMER_INTERVAL, on_timer, TIMER_ID);
    }
    
}




