#include "search.h"
#include "../include/movement.h"
#include "config.h"
#include "all_libraries.h"

#define Sleep( msec ) usleep(( msec ) * 1000 )

uint8_t sn_sonar;
//uint8_t sn_gyro;

///get random value between min & max
int random_int(int min, int max)
{
    return min + rand() % (max+1 - min);
}

/** 
 * function to turn to angle to begin a scan
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param begin angle to turn to in order to start the scan
 * written by Laila
 **/
void go_to_position_to_scan(int motor1, int motor2, int speed, float begin) {
    turn_left_precision(motor1, motor2, speed, begin);
}

/** 
 * function to get the sonar value
 * @param value pointer to store the value of the sonar
 * written by Laila
 **/
void get_distance_value(float *value){
    get_sensor_value0(sn_sonar, value);
}

/** 
 * simple scan to go to the minimum distance of object found
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param end angle to end scan 
 * @param radius radius to scan 
 * @param result angle to store the angle of the minimum distance found object
 * @param distance_to_object minimum distance to object found
 * written by Laila
 **/
void scan(int motor1, int motor2, int speed, float end, int radius, float* result, float* distance_to_object)
{
    int N = (int)end/3;
    float *distance;
    float *orientation;
    float min = 3000 ;
    int indicemin = -1;
    int i;
    float value;
    float dist;

    distance = malloc(N * sizeof(float));
    orientation = malloc(N * sizeof(float));

    pthread_t thread;
    pthread_create(&thread, NULL, SensorThread, NULL);

    for(i=0; i< N; i++)
    {
        turn_right_precision(motor1, motor2, speed, 3);
        get_distance_value(&dist);
        distance[i] = readSharedF(&sonar_shared_value);
        get_compass_value(&value);
        orientation[i] = (float)value;

        printf("distance: %f\n", distance[i]);
        printf("ORIENTATION: %f\n", orientation[i]);
        if (distance[i]<min )
        {
            
            min=distance[i] ;
            indicemin = i ;
        }
    }

    ///check if there is a value smaller then the radius
    if(min >= (float)radius){
        *result=-1;
    } else {
        printf("POTENTIAL OBSTACLE\n");
        *result = orientation[indicemin];
        printf("Angle: %f\n", *result);
        printf("Distance: %f\n", distance[indicemin]);

        int j = N;
        while (j > indicemin){
            turn_right_precision(motor1, motor2, speed, -3);
            j--;
        }
        goUntilDistance(motor1, motor2, speed/2, 5, distance[indicemin]);
        distance_to_object=(float*)abs(distance[indicemin]/10-4);
        
    }
    free(distance);
    free(orientation);
}

/** 
 * scan to go to the middle of minimum distance of object found
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param end angle to end scan 
 * @param radius radius to scan 
 * @param result angle to store the angle of the minimum distance found object
 * @param distance_to_object minimum distance to object found
 * written by Laila
 **/
void scanMiddle(int motor1, int motor2, int speed, float end, int radius, float *results, float *distance_to_object)
{
    int firstmin = radius, secmin = radius, thirdmin = radius; 
    int N = (int)end/3;
    float *distance;
    float *orientation;
    float min = 3000 ;
    int indicemin = -1;
    int indicemin1 = -1;
    int indicemin2 = -1;
    int indicemin3 = -1;
    int i;
    float value;

    distance = malloc(N * sizeof(float));
    orientation = malloc(N * sizeof(float));


    for(i=0; i< N; i++)
    {
        
        turn_right_precision(motor1, motor2, speed, 3);
        distance[i] = readSharedF(&sonar_shared_value);
        get_compass_value(&value);
        orientation[i] = (float)value;

        //printf("distance: %f\n", distance[i]);
        //printf("ORIENTATION: %f\n", orientation[i]);
        
        if (distance[i] < firstmin)
        { 
            thirdmin = secmin;
            secmin = firstmin;
            firstmin = distance[i];
            indicemin1 = i;
        } 

        else if (distance[i] < secmin)
        { 
            thirdmin = secmin;
            secmin = distance[i];
            indicemin2 = i;
        } 

        else if (distance[i] < thirdmin) {
            thirdmin = distance[i];
            indicemin3 = i;
        }
    }
    results[0] = indicemin1;
    results[1] = indicemin2;
    results[2] = indicemin3;

    float dist = 0;
    float orient = 0;
    int indicemean = 0;
    int p = 0;
    for (i = 0; i<3; i++) {
        if (results[i] != -1) {
            dist += distance[(int)results[i]];
            orient += orientation[(int)results[i]];
            indicemean += (int)results[i];
            p++;
        }
    }
    
    int j = N;
    if (indicemean != 0) {
        dist = dist/p;
        orient = orient/p;
        indicemean = (int)indicemean/p;
        while (j > indicemean){
            turn_right_precision(motor1, motor2, speed, -3);
            j--;
        }
        goUntilDistance(motor1, motor2, speed/2, 4, distance[indicemean]);
        *distance_to_object=abs(distance[indicemean]/10-4);
        *results=orientation[indicemean];

    } else {
        while (abs(orientation[j]) < 2){
            turn_right_precision(motor1, motor2, speed, -3);
            j--;
        }
    }


    free(distance);
    free(orientation);
}

/** 
 * check difference between indices for several scan to get 3 minimums of 3 different objects
 * @param indicemin1 index of first minimum
 * @param indicemin2 index of second minimum
 * @param diff ecart between two indices
 * written by Laila
 **/
bool indDifference(int indicemin1, int indicemin2, int diff) {
    if (indicemin2 != -1 && indicemin1 !=-1) {
        return abs(indicemin1 - indicemin2) < diff;
    } else {
        return true;
    } 
}

struct mydata {
  float* dist;
  float* or;
};

void delData (struct mydata *vector) {
    // Can safely assume vector is NULL or fully built.
    free(vector);
}

int data_cmp(const void *a, const void *b)
{
    const struct mydata *da = a, *db = b;
    float* l = da->dist; 
    float* r = db->dist; 
    //return (*l - *r); 
    return *l < *r ? -1 : *l > *r;
}

/** 
 * Scan to get the minimum distance of maximum three objects found
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param end angle to end scan 
 * @param radius radius to scan 
 * @param orientation_to_object angle to store the angle of the minimum distance found object
 * @param distance_to_object minimum distance to object found
 * written by Laila
 **/
void scanSeveral(int motor1, int motor2, int speed, float begin, float end, int radius, float *orientation_to_object, float *distance_to_object) {
    int firstmin = radius, secmin = radius, thirdmin = radius; 
    int N = (int)end/3;
    float results[3];
    float *distance;
    float *orientation;
    int indicemin1 = -1, indicemin2 = -1, indicemin3 = -1;
    int indicemin = -1, indicemean = -1;
    int i;
    float value;
    float dist;
    
    distance = malloc(N * sizeof(float));
    orientation = malloc(N * sizeof(float));

    struct mydata *newVector = (struct mydata*)malloc(sizeof(struct mydata) * N);

    pthread_t thread;
    pthread_create(&thread, NULL, SensorThread, NULL);

    for (i = 0; i < N; i++) 
    { 
        turn_right_precision(motor1, motor2, speed, 3);
        distance[i] = readSharedF(&sonar_shared_value);
        get_compass_value(&value);
        orientation[i] = (float)value;
        printf("distance: %f\n", distance[i]);
        printf("ORIENTATION: %f\n", orientation[i]);
    }

    newVector -> dist = distance;
    newVector -> or = orientation;

    printf("dist: %f\n", newVector -> dist[0]);
    printf("dist: %f\n", newVector -> dist[1]);
    
    qsort(newVector, N, sizeof(*newVector), data_cmp);

    printf("dist: %f\n", newVector -> dist[0]);
    printf("dist: %f\n", newVector -> dist[1]);
    
    for (i = 0; i < N; i++) 
    { 
        if (distance[i] < firstmin && indDifference(indicemin2, indicemin1, 3) && indDifference(indicemin3, indicemin1, 3)) 
        { 
            thirdmin = secmin;
            secmin = firstmin;
            firstmin = distance[i];
            indicemin1 = i;
        } 

        else if (distance[i] < secmin && indDifference(indicemin2, indicemin1, 3) && indDifference(indicemin3, indicemin2, 3)) 
        { 
            thirdmin = secmin;
            secmin = distance[i];
            indicemin2 = i;
        } 

        else if (distance[i] < thirdmin && indDifference(indicemin2, indicemin3, 3) && indDifference(indicemin1, indicemin3, 3))
        { 
            thirdmin = distance[i];
            indicemin3 = i;
        }
    }

    if (indDifference(indicemin1, indicemin2, 3)) {
        indicemin1 = (int)(indicemin1 + indicemin2)/2;
        indicemin2 = -1;
    } else if (indDifference(indicemin3, indicemin2, 3)) {
        indicemin2 = (int)(indicemin3 + indicemin2)/2;
        indicemin3 = -1;
    } else if (indDifference(indicemin1, indicemin3, 3)) {
        indicemin1 = (int)(indicemin3 + indicemin1)/2;
        indicemin3 = -1;
    }
    results[0] = indicemin1;
    results[1] = indicemin2;
    results[2] = indicemin3;
    
    printf("indicemin1: %d\n", indicemin1);
    printf("indicemin2: %d\n", indicemin2);
    printf("indicemin3: %d\n", indicemin3);
    int p;
    for (p = 0; p < 3; p++){
        distance_to_object[p] = distance[(int)results[p]];
        orientation_to_object[p] = orientation[(int)results[p]];
    }

    int j = N;
    if (indicemin1 != -1 || indicemin2 != -1 || indicemin3 != -1) {
        int q = 0;
        if (indicemin1 != -1) {
            indicemin = indicemin1;
            //distance_to_object[0] = distance[(int)results[0]];
            //orientation_to_object[0] = orientation[(int)results[0]];
        } else if (indicemin2 != -1) {
            indicemin = indicemin2;
            //distance_to_object[1] = distance[(int)results[1]];
            //orientation_to_object[1] = orientation[(int)results[1]];
        } else if(indicemin3 != -1) {
            indicemin = indicemin3;
            //distance_to_object[1] = distance[(int)results[1]];
            //orientation_to_object[1] = orientation[(int)results[1]];
        }

        printf("indicemin: %d\n", indicemin);
        for (q = 0; q < 3; q++){
            printf("distance: %f\n", distance[(int)results[q]]);
            if (distance[(int)results[q]] < distance[indicemin] && (int)results[q]!=-1) {
                indicemin = (int)results[q];
            }
        }
        printf("indicemin: %d\n", indicemin);

        while (abs(orientation[j]) < 2){
            turn_right_precision(motor1, motor2, speed, -3);
            j--;
        }
        /*
        while (j > indicemin + 1){
            turn_right_precision(motor1, motor2, speed, -3);
            j--;
        }
        goUntilDistance(motor1, motor2, speed/2, 5, distance[indicemean]);
        */
    } else {
        while (abs(orientation[j]) < 2){
            turn_right_precision(motor1, motor2, speed, -3);
            j--;
        }
    }
    delData(newVector);
}

/** 
 * Scan to get the minimum distance of maximum three objects found
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param end angle to end scan 
 * @param radius radius to scan 
 * @param orientation_to_object angle to store the angle of the minimum distance found object
 * @param distance_to_object minimum distance to object found
 * written by Laila
 **/
void scanSeveral2(int motor1, int motor2, int speed, float begin, float end, int radius, float *orientation_to_object, float *distance_to_object) {
    int firstmin = radius, secmin = radius, thirdmin = radius; 
    int N = (int)end/3;
    float results[3];
    float *distance;
    float *orientation;
    int indicemin1 = -1, indicemin2 = -1, indicemin3 = -1;
    int indicemin = -1, indicemean = -1;
    int i;
    float value;
    float dist;
    
    distance = malloc(N * sizeof(float));
    orientation = malloc(N * sizeof(float));

    pthread_t thread;
    pthread_create(&thread, NULL, SensorThread, NULL);

    for (i = 0; i < N; i++) 
    { 
        turn_right_precision(motor1, motor2, speed, 3);
        distance[i] = readSharedF(&sonar_shared_value);
        get_compass_value(&value);
        orientation[i] = (float)value;
        printf("distance: %f\n", distance[i]);
        printf("ORIENTATION: %f\n", orientation[i]);
    }
    
    for (i = 0; i < N; i++) 
    { 
        if (distance[i] < firstmin && indDifference(indicemin2, indicemin1, 3) && indDifference(indicemin3, indicemin1, 3)) 
        { 
            thirdmin = secmin;
            secmin = firstmin;
            firstmin = distance[i];
            indicemin1 = i;
        } 

        else if (distance[i] < secmin && indDifference(indicemin2, indicemin1, 3) && indDifference(indicemin3, indicemin2, 3)) 
        { 
            thirdmin = secmin;
            secmin = distance[i];
            indicemin2 = i;
        } 

        else if (distance[i] < thirdmin && indDifference(indicemin2, indicemin3, 3) && indDifference(indicemin1, indicemin3, 3))
        { 
            thirdmin = distance[i];
            indicemin3 = i;
        }
    }

    if (indDifference(indicemin1, indicemin2, 3)) {
        indicemin1 = (int)(indicemin1 + indicemin2)/2;
        indicemin2 = -1;
    } else if (indDifference(indicemin3, indicemin2, 3)) {
        indicemin2 = (int)(indicemin3 + indicemin2)/2;
        indicemin3 = -1;
    } else if (indDifference(indicemin1, indicemin3, 3)) {
        indicemin1 = (int)(indicemin3 + indicemin1)/2;
        indicemin3 = -1;
    }
    results[0] = indicemin1;
    results[1] = indicemin2;
    results[2] = indicemin3;
    
    printf("indicemin1: %d\n", indicemin1);
    printf("indicemin2: %d\n", indicemin2);
    printf("indicemin3: %d\n", indicemin3);
    int p;
    for (p = 0; p < 3; p++){
        distance_to_object[p] = distance[(int)results[p]];
        orientation_to_object[p] = orientation[(int)results[p]];
    }

    int j = N;
    if (indicemin1 != -1 || indicemin2 != -1 || indicemin3 != -1) {
        int q = 0;
        if (indicemin1 != -1) {
            indicemin = indicemin1;
            //distance_to_object[0] = distance[(int)results[0]];
            //orientation_to_object[0] = orientation[(int)results[0]];
        } else if (indicemin2 != -1) {
            indicemin = indicemin2;
            //distance_to_object[1] = distance[(int)results[1]];
            //orientation_to_object[1] = orientation[(int)results[1]];
        } else if(indicemin3 != -1) {
            indicemin = indicemin3;
            //distance_to_object[1] = distance[(int)results[1]];
            //orientation_to_object[1] = orientation[(int)results[1]];
        }

        printf("indicemin: %d\n", indicemin);
        for (q = 0; q < 3; q++){
            printf("distance: %f\n", distance[(int)results[q]]);
            if (distance[(int)results[q]] < distance[indicemin] && (int)results[q]!=-1) {
                indicemin = (int)results[q];
            }
        }
        printf("indicemin: %d\n", indicemin);

        while (abs(orientation[j]) < 2){
            turn_right_precision(motor1, motor2, speed, -3);
            j--;
        }
        /*
        while (j > indicemin + 1){
            turn_right_precision(motor1, motor2, speed, -3);
            j--;
        }
        goUntilDistance(motor1, motor2, speed/2, 5, distance[indicemean]);
        */
    } else {
        while (abs(orientation[j]) < 2){
            turn_right_precision(motor1, motor2, speed, -3);
            j--;
        }
    }
}

/** 
 * Function to go to a certain distance to an object found
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param distance_to_obstacle distance to object found
 * @param distancemin distance to be at the object
 * written by Laila
 **/
void goUntilDistance(int motor1, int motor2, int speed, float distance_to_obstacle, float distancemin)
{
    printf("distance final: %f\n", distancemin/10 - distance_to_obstacle);
    move_forward_cm(motor1, motor2, abs(distancemin/10 - distance_to_obstacle), speed);
}

/** 
 * final scan
 * @param motor1 first motor
 * @param motor2 second motor
 * @param speed speed to turn
 * @param radius radius to scan 
 * @param result angle to store the angle of the minimum distance found object
 * @param distance_to_object minimum distance to object found
 * @param side left or right scan
 * written by Laila
 **/
void allScan(int motor1, int motor2, int speed, int radius, float *results, float* distance_to_object, int side){
    int i;
    if (side==1) { // Right side scan
    	go_to_position_to_scan(motor1, motor2, speed, 160);
    	scanMiddle(motor1, motor2, speed, 100, radius, results, distance_to_object);
    }
    else if (side==0) { // Left side scan
    	scanMiddle(motor1, motor2, speed, 100, radius, results, distance_to_object);
    }
    /*go_to_position_to_scan(motor1, motor2, speed, 90);
    scan(motor1, motor2, speed, 200, radius, results, distance_to_object);*/
    //scanSeveral(motor1, motor2, speed, 0, 200, radius, results, distance_to_object);
    //scan(motor1, motor2, speed, 160, radius, results, distance_to_object);
    //printf("result middle scan: %f\n", *result);
}

/** 
 * Function to calculate the objectPosition
 * @param result angle of the minimum distance found object
 * @param distance_to_object minimum distance to object found
 * @param x stores x axis distance
 * @param y stores y axis distance
 * written by Andreas
 **/
void objectPosition(float result, float distance_to_object, int8_t* x, int8_t* y){
    printf("Angle: %f\nDistance: %f\n", result, distance_to_object);
    float x_f = -(sin(-result*3.1415/180.0) * (distance_to_object + 5 + 5));
    *x = x_f;
    printf("X_d: %d, X_f: %f\n", *x, x_f);
    float y_f = cos(-result*3.1415/180.0) * (distance_to_object + 5 + 5);
    *y = y_f;
    printf("Y_d: %d, Y_f: %f\n", *y, y_f);
}
