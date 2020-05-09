#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"
//Cabeceras
void imprimeTitulo();
void pulseTeclaParaIniciar();
void rutinaSucesos(int);
double generaNumAleat();
int rutinaTiempo();
void rutinaInicializacion();
double proximoEvento(int*);
double generadorExponencial(double);
double generadorErlang(int, double);
void generarInforme(double[], double, double, double);
void inicializarContadoresEstadisticos();
void realizarEstimaciones(double[], double*, double*, double*);
char ofrecerOtraSimulacion();
void imprimeAsteriscos(int num);
void inicializaCaracteristicasInteres(double*, double*, double*, double[], int *muestra);
//Constantes
#define N_ERLANG 3
#define TASALLEGADAS 2.0
#define TASASERVICIONODO1 9.0
#define TASASERVICIONODO2 12.0
#define TASASERVICIONODO3 18.0
#define LLEGADA 0
#define SALIDANODO1 1
#define SALIDANODO2 2
#define SALIDANODO3 3
#define LIBRE 0
#define OCUPADO 1
#define MAXCOLANODO3 7
#define TAMANIOMUESTRA 10000
#define PERIODOTRANSITORIO 500
#define NUMERODEMUESTRAS 25
//Reloj de Simulación
double reloj;
double relojTrans;
//Lista de Sucesos
double listaSucesos[4];
//Variables de Estado
int servidor1;
int servidor2;
int servidor3;
int numClientesNodo1;
int numClientesNodo2;
int colaNodo3;
double tiempoSucAnterior;
List tiempoDeLlegadasAlSistema;
//Contadores estadísticos
double tiempoClientesEnSistema;
int clientesServidosNodo3;
double tiempoServidor3ocup;
double tiempoQueHayKclientesNodo3[9];
int clientesPerdidos;

void main() {
    double probabilidades[9],sumaL,sumaW,sumaCuadrL;
    int suceso, muestra;
    char otraVez;
    imprimeTitulo();
    pulseTeclaParaIniciar();
    do{
        inicializaCaracteristicasInteres(&sumaL,&sumaW,&sumaCuadrL,probabilidades,&muestra);
        while (muestra <= NUMERODEMUESTRAS){
            rutinaInicializacion();
            //PERIODO TRANSITORIO
            while(clientesServidosNodo3 < PERIODOTRANSITORIO){
                suceso = rutinaTiempo();
                rutinaSucesos(suceso);
            }
            //PERIODO ESTACIONARIO
            relojTrans = reloj;
            inicializarContadoresEstadisticos();
            while (clientesServidosNodo3 < TAMANIOMUESTRA){
                suceso = rutinaTiempo();
                rutinaSucesos(suceso);
            }
            realizarEstimaciones(probabilidades, &sumaL, &sumaW, &sumaCuadrL);
            muestra++;
        }
        generarInforme(probabilidades, sumaL, sumaW, sumaCuadrL);
        otraVez=ofrecerOtraSimulacion();
    } while (otraVez != 'N' && otraVez !='n');
}

void imprimeTitulo() {
    printf("\t\t\t");
    imprimeAsteriscos(62);
    printf("\n\t\t\tSimulacion de una red en tandem con tiempos de servicio Erlang\n");
    printf("\t\t\t");
    imprimeAsteriscos(62);
    printf("\nKhalid Al Fozan de la Cerda, bo0264\n");
    printf("\nEste programa simula el comportamiento de una red de 3 colas en tandem y con las siguientes especificaciones: \n");
    printf("\t# El tiempo entre llegadas a la cola 1 es exponencial con tasa de llegada 2 clientes/minuto. A las cola 2 y 3 no llegan clientes desde el exterior.\n");
    printf("\t# El tiempo de servicio para cada servidor es: Erlang de media 1/3 de minuto para el servidor 1, 1/4 de minuto para el servidor 2 y 1/6 de minuto para el servidor 3.\n");
    printf("\t# La capacidad de las colas 1 y 2 es infinita, la de la 3 finita. Si hay 7 clientes en la tercera cola se pierden los clientes que lleguen.\n");
    printf("\t# Los clientes que salen del servidor 1 van al servidor 2 y los que salen del 2 van al 3.\n");
    printf("\t# Cuando un servidor queda vacio selecciona al primer cliente que este en cola. (Disciplina de cola FIFO).\n");
    printf("\t# La simulacion consta de 25 muestras independientes, y en cada muestra 10000 clientes servidos en el servidor 3 sin incluir el periodo transitorio.\n\n");
}
void pulseTeclaParaIniciar() {
    printf("Pulse enter cuando desee iniciar la simulacion:");
    while ( getchar() != '\n');
}
void rutinaSucesos(int suceso) {
    double erlang, exponencial;
    tiempoQueHayKclientesNodo3[servidor3+colaNodo3] += reloj-tiempoSucAnterior;
    if (servidor3 == OCUPADO)
        tiempoServidor3ocup += reloj-tiempoSucAnterior;
    switch (suceso)
    {
        case LLEGADA:
            exponencial = generadorExponencial(TASALLEGADAS);
            insertback(&tiempoDeLlegadasAlSistema,reloj);
            listaSucesos[LLEGADA] = reloj + exponencial;
            numClientesNodo1++;
            if (servidor1 == LIBRE){
                servidor1 = OCUPADO;
                erlang = generadorErlang(N_ERLANG,TASASERVICIONODO1);
                listaSucesos[SALIDANODO1] = reloj + erlang;
            }
            break;
        case SALIDANODO1:
            erlang = generadorErlang(N_ERLANG, TASASERVICIONODO1);
            numClientesNodo1--;
            numClientesNodo2++;
            if (numClientesNodo1 != 0)
                listaSucesos[SALIDANODO1] = reloj + erlang;
            else{
                servidor1 = LIBRE;
                listaSucesos[SALIDANODO1] = INFINITY;
            }
            if (servidor2 == LIBRE){
                servidor2 = OCUPADO;
                erlang = generadorErlang(N_ERLANG,TASASERVICIONODO2);
                listaSucesos[SALIDANODO2] = reloj + erlang;
            }
            break;
        case SALIDANODO2:
            erlang = generadorErlang(N_ERLANG, TASASERVICIONODO2);
            numClientesNodo2--;
            if (numClientesNodo2 != 0)
                listaSucesos[SALIDANODO2] = reloj + erlang;
            else{
                servidor2 = LIBRE;
                listaSucesos[SALIDANODO2] = INFINITY;
            }
            if (servidor3 == LIBRE){
                servidor3 = OCUPADO;
                erlang = generadorErlang(N_ERLANG, TASASERVICIONODO3);
                listaSucesos[SALIDANODO3] = reloj + erlang;
            } else{
                if (colaNodo3 != MAXCOLANODO3){
                    colaNodo3++;
                } else{
                    clientesPerdidos++;
                    tiempoClientesEnSistema +=reloj - getitem(tiempoDeLlegadasAlSistema,MAXCOLANODO3+1);
                    delete_from_position(&tiempoDeLlegadasAlSistema, MAXCOLANODO3+1);
                }
            }
            break;
        case SALIDANODO3:
            erlang = generadorErlang(N_ERLANG, TASASERVICIONODO3);
            clientesServidosNodo3++;
            tiempoClientesEnSistema += (reloj - getitem(tiempoDeLlegadasAlSistema,0));
            delete_from_begin(&tiempoDeLlegadasAlSistema);
            if (colaNodo3 == 0) {
                servidor3 = LIBRE;
                listaSucesos[SALIDANODO3] = INFINITY;
            } else{
                colaNodo3--;
                listaSucesos[SALIDANODO3] = reloj + erlang;
            }
            break;
    }
    tiempoSucAnterior = reloj;
}
void inicializaCaracteristicasInteres(double *sumaL, double *sumaW, double *sumaCuadrL, double probabilidades[], int *muestra){
    *muestra = 1;
    *sumaL=0;
    *sumaW=0;
    *sumaCuadrL = 0;
    for (int i=0; i<9; i++)
        probabilidades[i] = 0;
    srand(time(NULL));
    printf("\nSimulacion en progreso...\n");
}
char ofrecerOtraSimulacion() {
    char respuesta;
    do{
        printf("\nDesea realizar otra simulacion?[S,N]:");
        scanf(" %c",&respuesta);
    }while (respuesta!='S' && respuesta!='N' && respuesta!='s' && respuesta!= 'n');
    return respuesta;
}
void realizarEstimaciones(double probabilidades[], double *sumaL, double *sumaW, double *sumaCuadrL) {
    double relojEstacionario = reloj-relojTrans;
    double Li=0;
    for (int i = 0; i < 9; i++){
        probabilidades[i] += tiempoQueHayKclientesNodo3[i]/relojEstacionario;
        Li += i * tiempoQueHayKclientesNodo3[i]/relojEstacionario;
    }
    *sumaL += Li;
    *sumaCuadrL += pow(Li,2);
    *sumaW += tiempoClientesEnSistema / (clientesServidosNodo3 + clientesPerdidos);
    destroy(&tiempoDeLlegadasAlSistema); //Ya que ha concluido la muestra destruyo la lista para no desperdiciar memoria. En la rutina de incialización se inicializará la lista otra vez.
}
void inicializarContadoresEstadisticos() {
    tiempoClientesEnSistema=0;
    clientesServidosNodo3 = 0;
    tiempoServidor3ocup = 0;
    for (int i=0; i<9; i++){
        tiempoQueHayKclientesNodo3[i] = 0;
    }
    clientesPerdidos = 0;
}
void generarInforme(double probabilidades[], double sumaL, double sumaW, double sumaCuadrL) {
    double L = sumaL/NUMERODEMUESTRAS;
    double W = sumaW/NUMERODEMUESTRAS;
    double varianza = (sumaCuadrL/NUMERODEMUESTRAS) - pow(L,2);
    double cuasiVarianza = ( NUMERODEMUESTRAS/(NUMERODEMUESTRAS-1) ) * varianza;
    double cuasidt = sqrt(cuasiVarianza);
    double errorAbsoluto = 2.06*cuasidt/sqrt(NUMERODEMUESTRAS);
    double errorRelativo = errorAbsoluto/L;
    printf("\nSIMULACION COMPLETADA\n\n");
    imprimeAsteriscos(printf("RESUMEN PROBABILISTICO\n"));
    for (int j = 0; j < 9; ++j)
        printf("\nProbabilidad de que haya %d cliente(s) en el nodo 3 = %lf", j, probabilidades[j]/NUMERODEMUESTRAS );
    printf("\n\nProbabilidad de que se pierda un cliente en el nodo 3 = %lf\n",(double)clientesPerdidos/(clientesServidosNodo3+clientesPerdidos));
    imprimeAsteriscos(printf("\nPARAMETROS DE RENDIMIENTO DEL SISTEMA\n"));
    printf("\nLa estimacion de L para el nodo 3 es:\n");
    printf("L = %lf clientes\n", L);
    printf("\nLa estimacion de W para todo el sistema es:\n");
    printf("W = %lf minutos/cliente\n",W);
    imprimeAsteriscos(printf("\nINTERVALO DE CONFIANZA Y ERROR REALATIVO EN LA ESTIMACION DE L\n"));
    printf("\nAl 95 por ciento de confianza: %lf < L < %lf",L-errorAbsoluto,L+errorAbsoluto);
    printf("\n\nEl error relativo en la estimacion de L es de: %lf\n", errorRelativo);
}
void imprimeAsteriscos(int num) {
    for (int i=0; i<num; i++)
        printf("*");
}
void rutinaInicializacion() {
    initlist(&tiempoDeLlegadasAlSistema);
    listaSucesos[LLEGADA] = generadorExponencial(TASALLEGADAS);
    for (int k=1; k<4; k++)
        listaSucesos[k] = INFINITY;
    reloj = 0.0;
    clientesServidosNodo3=0;
    servidor1=LIBRE;
    servidor2=LIBRE;
    servidor3=LIBRE;
    colaNodo3=0;
    tiempoClientesEnSistema = 0;
    tiempoSucAnterior=0.0;
    clientesServidosNodo3=0;
    tiempoServidor3ocup=0.0;
    clientesPerdidos=0;
    for (int j=0; j<9;j++)
        tiempoQueHayKclientesNodo3[j]=0.0;
    numClientesNodo1 = 0;
    numClientesNodo2 = 0;
}
int rutinaTiempo() {
    int siguiente;
    reloj = proximoEvento(&siguiente); //determina el valor del reloj y el del siguiente evento de entre LLEGADA, SALIDANODO1, SALIDANODO2 O SALIDANODO3
    return siguiente;
}
double proximoEvento(int *siguiente) {
    double minimo = listaSucesos[0];
    *siguiente = LLEGADA;
    for (int i=1; i<4; i++)
        if (listaSucesos[i]<minimo) {
            minimo = listaSucesos[i];
            *siguiente = i;
        }
    return minimo;
}
double generaNumAleat() {
    double numaleat;
    do
        numaleat = rand() / (double)RAND_MAX;
    while (numaleat==0);
    return numaleat;
}
double generadorErlang(int n, double beta) {
    double aleatUnif01;
    double erlang;
    double productorio = 1;
    for (int i=0; i<n; i++){
        aleatUnif01 = generaNumAleat();
        productorio *= aleatUnif01;
    }
    erlang = (-1.0)/beta  *  log(productorio);
    return erlang;
}
double generadorExponencial(double beta) {
    double numAleat = generaNumAleat();
    double exp = (-1.0)/beta * log(numAleat) ;
    return exp;
}


