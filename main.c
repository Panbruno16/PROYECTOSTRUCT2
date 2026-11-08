/**
 * PROYECTO: COMUNAS ANARQUISTAS
 * Creado por: Carlos Bruno Díaz Córdoba, Emmanuel Matarrita Pineda
 * Fecha de creación: 30/09/2026
 * Última modificación: 12/10/2026
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sociedad.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"
#define BOLD    "\033[1m"

/**
 * main: Funcion principal del programa.
 * Inicia el juego y maneja el ciclo de dias.
 */
int main(){
    srand(time(NULL));
    cargarRecord();
    printf(CYAN BOLD "Record actual: %d dias\n" RESET, estado.record_dias);

    printf(CYAN "\nConfiguracion inicial:\n" RESET);

    printf("Cantidad de comunas (3-50): ");
    int numComunas = 3;
    if(scanf("%d", &numComunas)!=1){
        limpiarEntrada();
        numComunas = 3;
        printf("Entrada invalida, se usaran 3 comunas.\n");
    }
    if(numComunas < 3){
        numComunas = 3;
    }
    if(numComunas > 50){
        numComunas = 50;
    }

    printf("Cantidad de oficios en el catalogo (1-20): ");
    int numServicios = 1;
    if(scanf("%d", &numServicios)!=1){
        limpiarEntrada();
        numServicios = 1;
        printf("Entrada invalida, se usara 1 oficio.\n");
    }
    if(numServicios < 1){
        numServicios = 1;
    }
    if(numServicios > 20){
        numServicios = 20;
    }

    printf("Cantidad de bienes (1-20): ");
    int numBienes = 1;
    if(scanf("%d", &numBienes)!=1){
        limpiarEntrada();
        numBienes = 1;
        printf("Entrada invalida, se usara 1 bien.\n");
    }
    if(numBienes < 1){
        numBienes = 1;
    }
    if(numBienes > 20){
        numBienes = 20;
    }

    struct comuna* cabeza = crearComunas("comunas", numComunas, numServicios, numBienes);

    if(cabeza==NULL){
        printf(RED "Error: no se pudieron crear las comunas\n" RESET);
        return 1;
    }

    inicializarSociedad(cabeza);

    printf(GREEN "Comunas creadas: %d\n" RESET, contarComunas(cabeza));
    printf(GREEN "Tus comunas: %d\n" RESET, contarAdministradas(cabeza));

    int jugando = 1;
    recalcularIndices(cabeza);
    while(jugando){
        estado.dia_actual = estado.dia_actual + 1;
        printf(CYAN BOLD "\nDIA %d\n" RESET, estado.dia_actual);

        int diaTerminado = turnoDia(cabeza);
        turnoComunas(cabeza);

        turnoNoche(&cabeza);

        if(diaTerminado==0){
            printf(YELLOW "\nSe acabo la entrada, el juego termina aqui.\n" RESET);
            jugando = 0;
        }

        else if(cabeza==NULL || contarComunas(cabeza) < 3 || contarAdministradas(cabeza)==0){
            printf(RED BOLD "\nGAME OVER\n" RESET);
            printf(YELLOW "La sociedad quedo por debajo de 3 comunas, fracasaste.\n" RESET);
            printf(YELLOW "Has sobrevivido %d dias.\n" RESET, estado.dia_actual);
            jugando = 0;
        }

        else if(estado.dia_actual % 15 == 0){
            printf(MAGENTA "\nFelicidades! Has sobrevivido %d dias.\n" RESET, estado.dia_actual);
            printf(YELLOW "Deseas retirarte y cambiar al capitalismo?\n" RESET);
            printf("[1] Si, retirarme [2] No, seguir en la comuna: ");
            int opcion = 2;
            if(scanf("%d", &opcion)!=1){
                limpiarEntrada();
                opcion = 2;
            }
            if(opcion == 1){
                printf(GREEN BOLD "\nFIN DEL JUEGO\n" RESET);
                printf(CYAN "Donald Trump estara orgulloso de que se una a nosotros :)))))\n" RESET);
                printf(YELLOW "Has sobrevivido %d dias en las comunas anarquistas.\n" RESET, estado.dia_actual);
                jugando = 0;
            }
        }

    }

    guardarRecord();
    liberarListaComunas(cabeza);

    return 0;
}
