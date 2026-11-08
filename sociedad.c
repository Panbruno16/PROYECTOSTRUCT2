/**
 * PROYECTO: COMUNAS ANARQUISTAS
 * Creado por: Carlos Bruno Díaz Córdoba, Emmanuel Matarrita Pineda
 * Fecha de creación: 30/09/2026
 * Última modificación: 07/10/2026
 */
#include "sociedad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// estructura global del juego
struct juego estado;

/**
 * crearComuna: Crea una nueva comuna con el nombre especificado.
 * Inicializa los indices en 0 y los punteros de la lista circular apuntan a si mismo.
 * Nace como comuna autonoma (administrada en 0).
 */
struct comuna* crearComuna(char* nombre){
    struct comuna* nueva = calloc(1, sizeof(struct comuna));
    strcpy(nueva->nombre, nombre);
    nueva->necesidad = 0.0;
    nueva->satisfaccion = 0.0;
    nueva->pobreza = 0.0;
    nueva->reciprocidad = 0.0;
    nueva->administrada = 0;
    nueva->siguiente = nueva;
    nueva->anterior = nueva;
    return nueva;
}

/**
 * mostrarComuna: Muestra en pantalla la informacion de una comuna.
 * Imprime el nombre y los indices de necesidad, satisfaccion, pobreza y reciprocidad.
 */
void mostrarComuna(struct comuna* comuna){
    if(comuna==NULL){
        return;
    }
    printf("\n");
    printf("########################################\n");
    printf("#  COMUNA: %s\n", comuna->nombre);
    printf("########################################\n");
    printf("#  Necesidad:    %.2f%%\n", comuna->necesidad);
    printf("#  Satisfaccion: %.2f%%\n", comuna->satisfaccion);
    printf("#  Pobreza:      %.2f%%\n", comuna->pobreza);
    printf("#  Reciprocidad: %.2f\n", comuna->reciprocidad);
    printf("########################################\n");
}

/**
 * agregarBien: Agrega un nuevo bien a la lista de bienes de una comuna.
 * Inserta el bien al inicio de la lista enlazada.
 */
void agregarBien(struct comuna* comuna, char* nombre, int maximaCantidad, int actualCantidad){
    struct bien* nuevo = calloc(1, sizeof(struct bien));
    strcpy(nuevo->nombre, nombre);
    nuevo->maximaCantidad = maximaCantidad;
    nuevo->actualCantidad = actualCantidad;
    nuevo->siguiente = comuna->bienes;
    comuna->bienes = nuevo;
}

/**
 * agregarPersona: Agrega una nueva persona a la lista de habitantes de una comuna.
 * Inserta la persona al inicio de la lista enlazada.
 */
void agregarPersona(struct comuna* comuna, char* nombre){
    struct persona* nuevo = calloc(1, sizeof(struct persona));
    strcpy(nuevo->nombre, nombre);
    nuevo->siguiente = comuna->habitantes;
    comuna->habitantes = nuevo;
}

/**
 * agregarServicioAPersona: Agrega un oficio a la lista de servicios de una persona.
 * Inserta el servicio al inicio de la lista enlazada de la persona.
 */
void agregarServicioAPersona(struct persona* persona, char* nombre){
    if(persona==NULL){
        return;
    }

    struct servicio* nuevo = calloc(1, sizeof(struct servicio));
    strcpy(nuevo->nombre, nombre);
    nuevo->siguiente = persona->servicios;
    persona->servicios = nuevo;
}

/**
 * migrarPersona: Mueve una persona de una comuna a otra llevandose sus oficios.
 * Es una funcion auxiliar interna para la logica de disolucion de comunas.
 * Busca a la persona en el origen, la desenlaza y la enlaza al destino.
 * Retorna 1 si la persona encontro destino, 0 si fallo.
 */
int migrarPersona(struct comuna* origen, struct comuna* destino, char* nombre){
    if(origen==NULL || destino==NULL || nombre==NULL){
        return 0;
    }

    if(origen==destino){
        return 0;
    }

    // Buscar la persona en el origen
    struct persona* anterior = NULL;
    struct persona* actual = origen->habitantes;
    while(actual!=NULL){
        if(strcmp(actual->nombre, nombre)==0){
            break;
        }

        anterior = actual;
        actual = actual->siguiente;
    }

    if(actual==NULL){
        return 0;
    }

    // Desenlazar del origen
    if(anterior==NULL){
        origen->habitantes = actual->siguiente;
    }
    else{
        anterior->siguiente = actual->siguiente;
    }

    // Enlazar al inicio del destino
    actual->siguiente = destino->habitantes;
    destino->habitantes = actual;

    return 1;
}

/**
 * contarPersonas: Cuenta cuantas personas hay en una comuna.
 * Recorre la lista de habitantes y retorna el conteo total.
 */
int contarPersonas(struct comuna* comuna){
    if(comuna==NULL){
        return 0;
    }

    int contador = 0;
    struct persona* actual = comuna->habitantes;
    while(actual!=NULL){
        contador = contador + 1;
        actual = actual->siguiente;
    }

    return contador;
}

/**
 * contarBienes: Cuenta cuantos bienes hay en una comuna.
 * Recorre la lista de bienes y retorna el conteo total.
 */
int contarBienes(struct comuna* comuna){
    if(comuna==NULL){
        return 0;
    }

    int contador = 0;
    struct bien* actual = comuna->bienes;
    while(actual!=NULL){
        contador = contador + 1;
        actual = actual->siguiente;
    }

    return contador;
}

/**
 * contarServicios: Cuenta cuantos oficios tienen las personas de una comuna.
 * Recorre los habitantes y suma los servicios que sabe cada uno.
 */
int contarServicios(struct comuna* comuna){
    if(comuna==NULL){
        return 0;
    }

    int contador = 0;
    struct persona* personaActual = comuna->habitantes;
    while(personaActual!=NULL){
        struct servicio* servicioActual = personaActual->servicios;
        while(servicioActual!=NULL){
            contador = contador + 1;
            servicioActual = servicioActual->siguiente;
        }

        personaActual = personaActual->siguiente;
    }

    return contador;
}

/**
 * mostrarBienes: Muestra en pantalla los bienes de una comuna.
 * Recorre la lista de bienes e imprime el nombre y las cantidades de cada uno.
 */
void mostrarBienes(struct comuna* comuna){
    if(comuna==NULL){
        return;
    }

    printf("\n");
    printf("########################################\n");
    printf("#  BIENES DE: %s\n", comuna->nombre);
    printf("########################################\n");
    struct bien* actual = comuna->bienes;
    while(actual!=NULL){
        printf("#  %-20s %3d/%3d\n", actual->nombre, actual->actualCantidad, actual->maximaCantidad);
        actual = actual->siguiente;
    }

    printf("########################################\n");
}

/**
 * mostrarServicios: Muestra en pantalla los oficios de cada persona de una comuna.
 * Recorre los habitantes e imprime los servicios que sabe cada persona.
 * Muestra un resumen con el porcentaje de gente que al menos sabe un oficio.
 */
void mostrarServicios(struct comuna* comuna){
    if(comuna==NULL){
        return;
    }

    int totalPersonas = contarPersonas(comuna);
    int personasConOficio = 0;
    struct persona* recorrido = comuna->habitantes;
    while(recorrido!=NULL){
        if(recorrido->servicios!=NULL){
            personasConOficio = personasConOficio + 1;
        }

        recorrido = recorrido->siguiente;
    }
    float porcentaje = 0.0;
    if(totalPersonas>0){
        porcentaje = ((float)personasConOficio * 100.0) / (float)totalPersonas;
    }

    printf("\n");
    printf("########################################\n");
    printf("#  OFICIOS DE: %s\n", comuna->nombre);
    printf("#  Con oficio: %d/%d (%.1f%%)\n", personasConOficio, totalPersonas, porcentaje);
    printf("########################################\n");
    struct persona* personaActual = comuna->habitantes;
    int contador = 1;
    while(personaActual!=NULL){
        printf("#  %d. %s:", contador, personaActual->nombre);
        struct servicio* servicioActual = personaActual->servicios;
        if(servicioActual==NULL){
            printf(" no sabe hacer nada\n");
        }
        else{
            printf("\n");
            while(servicioActual!=NULL){
                printf("#      %s\n", servicioActual->nombre);
                servicioActual = servicioActual->siguiente;
            }

        }

        contador = contador + 1;
        personaActual = personaActual->siguiente;
    }

    printf("########################################\n");
}

/**
 * mostrarPersonas: Muestra en pantalla los habitantes de una comuna.
 * Recorre la lista de habitantes e imprime el nombre de cada persona.
 */
void mostrarPersonas(struct comuna* comuna){
    if(comuna==NULL){
        return;
    }

    printf("\n");
    printf("########################################\n");
    printf("#  HABITANTES DE: %s\n", comuna->nombre);
    printf("########################################\n");
    struct persona* actual = comuna->habitantes;
    int contador = 1;
    while(actual!=NULL){
        printf("#  %d. %s\n", contador, actual->nombre);
        contador = contador + 1;
        actual = actual->siguiente;
    }

    printf("########################################\n");
}

/**
 * agregarPersonas: Agrega varias personas a una comuna desde un archivo.
 * Lee nombres del archivo y los agrega a la lista de habitantes.
 * Retorna el numero de personas agregadas.
 */
int agregarPersonas(struct comuna* comuna, char* nombreArchivo){
    if(comuna==NULL || nombreArchivo==NULL){
        return 0;
    }
    FILE* archivo = fopen(nombreArchivo, "r");
    if(archivo==NULL){
        printf("Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return 0;
    }
    char nombre[50];
    int contador = 0;
    while(fgets(nombre, 50, archivo)){
        nombre[strcspn(nombre, "\n")] = 0;
        agregarPersona(comuna, nombre);
        contador = contador + 1;
    }
    fclose(archivo);
    return contador;
}

/**
 * agregarBienes: Agrega varios bienes a una comuna desde un archivo.
 * Lee nombres del archivo y los agrega a la lista de bienes.
 * Retorna el numero de bienes agregados.
 */
int agregarBienes(struct comuna* comuna, char* nombreArchivo){
    if(comuna==NULL || nombreArchivo==NULL){
        return 0;
    }
    FILE* archivo = fopen(nombreArchivo, "r");
    if(archivo==NULL){
        printf("Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return 0;
    }
    char nombre[50];
    int contador = 0;
    while(fgets(nombre, 50, archivo)){
        nombre[strcspn(nombre, "\n")] = 0;
        agregarBien(comuna, nombre, 100, 90);
        contador = contador + 1;
    }
    fclose(archivo);
    return contador;
}

/**
 * crearComunas: Crea varias comunas y las conecta en lista circular doble.
 * Lee nombres del archivo y crea una comuna por cada nombre.
 * Las 500 personas se reparten aleatoriamente entre las comunas.
 * Cada persona tiene un 30% de probabilidad de recibir un oficio al azar.
 * Las primeras 3 comunas quedan marcadas como administradas por el jugador.
 * Retorna la lista de comunas creadas.
 */
struct comuna* crearComunas(char* nombreArchivo, int cantidad, int numServicios, int numBienes){
    FILE* archivo = fopen(nombreArchivo, "r");
    if(archivo==NULL){
        printf("Error: no se pudo abrir el archivo %s\n", nombreArchivo);
        return NULL;
    }
    char nombres[50][50];
    int total = 0;
    char nombre[50];
    while(fgets(nombre, 50, archivo) && total < 50){
        nombre[strcspn(nombre, "\n")] = 0;
        strcpy(nombres[total], nombre);
        total = total + 1;
    }
    fclose(archivo);
    int seleccionadas[50];
    for(int i = 0; i < 50; i++){
        seleccionadas[i] = 0;
    }
    struct comuna* lista = NULL;
    int creadas = 0;
    while(creadas < cantidad && creadas < total){
        int indice = rand() % total;
        if(seleccionadas[indice] == 0){
            seleccionadas[indice] = 1;
            agregarComuna(&lista, nombres[indice]);
            creadas = creadas + 1;
        }
    }
    FILE* archivoBienes = fopen("bienes", "r");
    if(archivoBienes != NULL){
        char bien[50];
        int cargados = 0;
        while(fgets(bien, 50, archivoBienes) && cargados < numBienes){
            bien[strcspn(bien, "\n")] = 0;
            struct comuna* actual = lista;
            if(actual != NULL){
                do{
                    agregarBien(actual, bien, 100, 90);
                    actual = actual->siguiente;
                }while(actual != lista);
            }
            cargados = cargados + 1;
        }
        fclose(archivoBienes);
    }
    // Catalogo de oficios: se lee el archivo de servicios para tenerlos disponibles
    char catalogo[20][50];
    int totalOficios = 0;
    FILE* archivoServicios = fopen("servicios", "r");
    if(archivoServicios != NULL){
        char oficio[50];
        while(fgets(oficio, 50, archivoServicios) && totalOficios < numServicios && totalOficios < 20){
            oficio[strcspn(oficio, "\n")] = 0;
            strcpy(catalogo[totalOficios], oficio);
            totalOficios = totalOficios + 1;
        }
        fclose(archivoServicios);
    }
    FILE* archivoPersonas = fopen("personas", "r");
    if(archivoPersonas != NULL){
        char persona[50];
        int agregadas = 0;
        while(fgets(persona, 50, archivoPersonas) && agregadas < 500){
            persona[strcspn(persona, "\n")] = 0;
            int comunaIndice = rand() % cantidad;
            struct comuna* actual = lista;
            for(int i = 0; i < comunaIndice; i++){
                actual = actual->siguiente;
            }
            agregarPersona(actual, persona);
            // La persona recien agregada queda al inicio de la lista de habitantes
            int probabilidad = rand() % 100;
            if(probabilidad < 30 && totalOficios > 0){
                int indiceOficio = rand() % totalOficios;
                agregarServicioAPersona(actual->habitantes, catalogo[indiceOficio]);
            }

            agregadas = agregadas + 1;
        }
        fclose(archivoPersonas);
    }
    // Las primeras 3 comunas creadas son las que maneja el jugador
    struct comuna* marcada = lista;
    int administradas = 0;
    if(marcada!=NULL){
        do{
            if(administradas < 3){
                marcada->administrada = 1;
                administradas = administradas + 1;
            }
            marcada = marcada->siguiente;
        }while(marcada!=lista && administradas < 3);
    }
    return lista;
}

/**
 * agregarComuna: Agrega una comuna a la lista circular doble.
 * Inserta la comuna al final de la lista y conecta los punteros.
 */
void agregarComuna(struct comuna** lista, char* nombre){
    if(lista==NULL){
        return;
    }
    struct comuna* nueva = crearComuna(nombre);
    if(nueva==NULL){
        return;
    }
    if(*lista==NULL){
        *lista = nueva;
        return;
    }
    struct comuna* ultimo = (*lista)->anterior;
    ultimo->siguiente = nueva;
    nueva->anterior = ultimo;
    nueva->siguiente = *lista;
    (*lista)->anterior = nueva;
}

/**
 * contarComunas: Cuenta cuantas comunas hay en la lista.
 * Recorre la lista circular y retorna el conteo total.
 */
int contarComunas(struct comuna* lista){
    if(lista==NULL){
        return 0;
    }
    int contador = 0;
    struct comuna* actual = lista;
    do{
        contador = contador + 1;
        actual = actual->siguiente;
    }while(actual!=lista);
    return contador;
}

/**
 * contarAdministradas: Cuenta las comunas que maneja el jugador.
 * Recorre la lista circular y cuenta las que tienen administrada en 1.
 * El jugador empieza con 3 y no tiene reposicion.
 */
int contarAdministradas(struct comuna* lista){
    if(lista==NULL){
        return 0;
    }
    int contador = 0;
    struct comuna* actual = lista;
    do{
        if(actual->administrada==1){
            contador = contador + 1;
        }
        actual = actual->siguiente;
    }while(actual!=lista);
    return contador;
}

/**
 * inicializarSociedad: Inicializa todas las comunas con personas, bienes y servicios.
 * Carga los datos desde los archivos y los asigna a cada comuna.
 */
void inicializarSociedad(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    // Las personas ya fueron cargadas en crearComunas
    // Aqui solo verificamos que todo este correcto
    int totalPersonas = 0;
    struct comuna* actual = lista;
    do{
        totalPersonas = totalPersonas + contarPersonas(actual);
        actual = actual->siguiente;
    }while(actual!=lista);
    recalcularIndices(lista);
    printf("Sociedad inicializada: %d comunas, %d personas totales\n", contarComunas(lista), totalPersonas);
}

/**
 * calcularNecesidad: Calcula el indice de necesidad de una comuna.
 * Recorre los bienes y calcula el promedio de (actual/maxima) de llenado.
 * La necesidad es 100 menos ese llenado: alto = le falta recursos,
 * bajo = tiene recursos. Asi se disuelve cuando la necesidad es muy alta.
 */
float calcularNecesidad(struct comuna* comuna){
    if(comuna==NULL){
        return 0.0;
    }
    float suma = 0.0;
    int contador = 0;
    struct bien* actual = comuna->bienes;
    while(actual!=NULL){
        if(actual->maximaCantidad>0){
            suma = suma + ((float)actual->actualCantidad / actual->maximaCantidad);
            contador = contador + 1;
        }
        actual = actual->siguiente;
    }
    if(contador==0){
        return 100.0;
    }
    float llenado = (suma/contador)*100.0;
    float necesidad = 100.0 - llenado;
    return necesidad;
}

/**
 * calcularSatisfaccion: Calcula el indice de satisfaccion de una comuna.
 * Considera el llenado de bienes (100 - necesidad) y el porcentaje de personas con oficio.
 * Mas recursos y mas gente con oficio = mas satisfecha.
 */
float calcularSatisfaccion(struct comuna* comuna){
    if(comuna==NULL){
        return 50.0;
    }
    int totalPersonas = contarPersonas(comuna);
    int personasConOficio = 0;
    struct persona* personaActual = comuna->habitantes;
    while(personaActual!=NULL){
        if(personaActual->servicios!=NULL){
            personasConOficio = personasConOficio + 1;
        }
        personaActual = personaActual->siguiente;
    }
    float ratioServicios;
    if(totalPersonas>0){
        ratioServicios = (float)personasConOficio/(float)totalPersonas;
    }
    else{
        ratioServicios = 0.0;
    }
    float satisfaccion = ((100.0 - comuna->necesidad)*0.6)+(ratioServicios*40.0);
    if(satisfaccion>100.0){
        satisfaccion = 100.0;
    }
    if(satisfaccion<0.0){
        satisfaccion = 0.0;
    }
    return satisfaccion;
}

/**
 * recalcularIndices: Recalcula los indices de todas las comunas.
 */
void recalcularIndices(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    struct comuna* actual = lista;
    do{
        actual->necesidad = calcularNecesidad(actual);
        actual->satisfaccion = calcularSatisfaccion(actual);
        actual = actual->siguiente;
    }while(actual!=lista);
}

/**
 * aplicarConsumoNocturno: Reduce los bienes de todas las comunas.
 * Cada persona consume 0.1 bienes por dia (minimo 7, maximo 10).
 */
void aplicarConsumoNocturno(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    struct comuna* actual = lista;
    do{
        int personas = contarPersonas(actual);
        int consumo = (int)(personas*0.1);
        if(consumo<7){
            consumo = 7;
        }
        if(consumo>10){
            consumo = 10;
        }
        struct bien* bien = actual->bienes;
        while(bien!=NULL){
            bien->actualCantidad = bien->actualCantidad - consumo;
            if(bien->actualCantidad<0){
                bien->actualCantidad = 0;
            }
            bien = bien->siguiente;
        }
        actual = actual->siguiente;
    }while(actual!=lista);
    printf("Consumo nocturno aplicado a %d comunas\n", contarComunas(lista));
}

/**
 * calcularPobrezaGlobal: Calcula la pobreza global del juego.
 * Se calcula como: (personas_fuera / 500) * 100.
 */
float calcularPobrezaGlobal(){
    return ((float)estado.personas_fuera/500.0)*100.0;
}

/**
 * actualizarPobreza: Actualiza la pobreza global y asigna el valor a todas las comunas.
 */
void actualizarPobreza(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    estado.pobreza_global = calcularPobrezaGlobal();
    struct comuna* actual = lista;
    do{
        actual->pobreza = estado.pobreza_global;
        actual = actual->siguiente;
    }while(actual!=lista);
    printf("Pobreza global: %.2f%% (%d personas fuera)\n", estado.pobreza_global, estado.personas_fuera);
}

/**
 * verificarEmergencias: Verifica y aplica emergencias aleatorias.
 * Huelga: probabilidad base 10% + pobreza * 0.3%
 * Vandalos: probabilidad base 5% + pobreza * 0.4%
 * Temblor: probabilidad base 3%
 * Inundacion: probabilidad base 2%
 * Feriado: probabilidad base 4%
 * Presidencia: probabilidad base 3%
 */
void verificarEmergencias(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    float pobreza = estado.pobreza_global;

    // Huelga: probabilidad base 10% + pobreza * 0.3%
    float probabilidadHuelga = 0.10+(pobreza*0.003);
    if((float)rand()/RAND_MAX < probabilidadHuelga){
        printf("HUELGA en una comuna! Intercambios bloqueados, pues estan exigiendo aguinaldo.\n");
    }

    // Vandalos: probabilidad base 5% + pobreza * 0.4%
    float probabilidadVandalos = 0.05+(pobreza*0.004);
    if((float)rand()/RAND_MAX < probabilidadVandalos){
        printf("Hay vandalismo, pero como no es dia de revision, los policias no hacen nada por ello. :)\n");
    }

    // Temblor: probabilidad base 3%
    float probabilidadTemblor = 0.03;
    if((float)rand()/RAND_MAX < probabilidadTemblor){
        printf("TEMBLOR! Los servicios y bienes bajaron, debido a que casi se nos va Panama.\n");
    }

    // Inundacion: probabilidad base 2%
    float probabilidadInundacion = 0.02;
    if((float)rand()/RAND_MAX < probabilidadInundacion){
        printf("INUNDACION! Los bienes bajaron, ahora resulta que vivimos como sirenas.\n");
    }

    // Feriado: probabilidad base 4%
    float probabilidadFeriado = 0.04;
    if((float)rand()/RAND_MAX < probabilidadFeriado){
        printf("FERIADO! No hay consumo de bienes ni intercambios, todos a la playa.\n");
    }

    // Presidencia: probabilidad base 3%
    float probabilidadPresidencia = 0.03;
    if((float)rand()/RAND_MAX < probabilidadPresidencia){
        estado.transferencias_bloqueadas = 1;
        printf("ANUNCIO! Nuestra querida presidencia nos dicta que hoy si van a trabajar, por lo tanto no hay intercambios.\n");
    }
}

/**
 * liberarComuna: Libera toda la memoria asignada a una comuna.
 * Libera los bienes, las personas, los servicios y finalmente la comuna.
 * Asigna NULL a cada puntero despues de liberarlo.
 */
void liberarComuna(struct comuna* comuna){
    if(comuna==NULL){
        return;
    }
    struct bien* bienActual = comuna->bienes;
    while(bienActual!=NULL){
        struct bien* temporal = bienActual;
        bienActual = bienActual->siguiente;
        free(temporal);
        temporal = NULL;
    }
    struct persona* personaActual = comuna->habitantes;
    while(personaActual!=NULL){
        struct servicio* servicioActual = personaActual->servicios;
        while(servicioActual!=NULL){
            struct servicio* temporalServicio = servicioActual;
            servicioActual = servicioActual->siguiente;
            free(temporalServicio);
            temporalServicio = NULL;
        }
        personaActual->servicios = NULL;
        struct persona* temporal = personaActual;
        personaActual = personaActual->siguiente;
        free(temporal);
        temporal = NULL;
    }
    free(comuna);
    comuna = NULL;
}

/**
 * liberarListaComunas: Libera toda la lista de comunas.
 */
void liberarListaComunas(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    struct comuna* actual = lista;
    struct comuna* siguiente;
    do{
        siguiente = actual->siguiente;
        liberarComuna(actual);
        actual = siguiente;
    }while(actual!=lista);
}

/**
 * calcularTurnosPorDia: Calcula los turnos disponibles por dia.
 * Se calcula como: totalComunas / 10, con minimo 2 y maximo 10.
 */
int calcularTurnosPorDia(int totalComunas){
    int turnos = totalComunas / 10;
    if(turnos < 2){
        turnos = 2;
    }
    if(turnos > 10){
        turnos = 10;
    }
    return turnos;
}

/**
 * guardarRecord: Guarda el record de dias en un archivo.
 */
void guardarRecord(){
    FILE* archivo = fopen("record.txt", "w");
    if(archivo==NULL){
        printf("Error: no se pudo guardar el record.\n");
        return;
    }
    fprintf(archivo, "%d\n", estado.record_dias);
    fclose(archivo);
}

/**
 * cargarRecord: Carga el record de dias desde un archivo.
 */
void cargarRecord(){
    FILE* archivo = fopen("record.txt", "r");
    if(archivo==NULL){
        estado.record_dias = 0;
        return;
    }
    fscanf(archivo, "%d", &estado.record_dias);
    fclose(archivo);
}

/**
 * aumentarValores: Aumenta los bienes de una comuna por transferencia.
 * El aumento es proporcional a la cantidad transferida (cantidad * 0.1).
 * Los servicios ya no se aumentan porque son conocimiento, no recursos.
 */
void aumentarValores(struct comuna* comuna, int cantidad){
    if(comuna==NULL || cantidad<=0){
        return;
    }
    int aumentoBienes = (int)(cantidad * 0.1);
    if(aumentoBienes < 1){
        aumentoBienes = 1;
    }
    struct bien* bien = comuna->bienes;
    while(bien!=NULL){
        bien->actualCantidad = bien->actualCantidad + aumentoBienes;
        if(bien->actualCantidad > bien->maximaCantidad){
            bien->actualCantidad = bien->maximaCantidad;
        }
        bien = bien->siguiente;
    }
    printf("Aumento: +%d bienes\n", aumentoBienes);
}

/**
 * mostrarResumen: Muestra un resumen de todas las comunas.
 * Muestra nombre, indices y cantidad de personas de cada comuna.
 * Marca con TUYA las comunas que maneja el jugador y con AUTO las autonomas.
 */
void mostrarResumen(struct comuna* lista){
    if(lista==NULL){
        printf("No hay comunas.\n");
        return;
    }
    printf("\n");
    printf("########################################\n");
    printf("#  RESUMEN DE COMUNAS\n");
    printf("########################################\n");
    struct comuna* actual = lista;
    int contador = 1;
    do{
        char marca[10];
        if(actual->administrada==1){
            strcpy(marca, "TUYA");
        }
        else{
            strcpy(marca, "AUTO");
        }
        printf("#  %d. [%s] %-15s N:%5.1f%% S:%5.1f%% P:%5.1f%% Personas:%d\n",
               contador, marca, actual->nombre, actual->necesidad, actual->satisfaccion, actual->pobreza, contarPersonas(actual));
        contador = contador + 1;
        actual = actual->siguiente;
    }while(actual!=lista);
    printf("########################################\n");
}

/**
 * mostrarMenu: Muestra el menu de opciones del jugador.
 */
void mostrarMenu(){
    printf("\n");
    printf("########################################\n");
    printf("#  MENU PRINCIPAL\n");
    printf("########################################\n");
    printf("#  [1] Ver resumen de comunas\n");
    printf("#  [2] Realizar transferencia\n");
    printf("#  [3] Ver detalle de comuna\n");
    printf("#  [4] Finalizar dia\n");
    printf("########################################\n");
    printf("\n");
    printf("Selecciona una opcion: ");
}

/**
 * regalarAlAzar: Elige al azar un bien de la comuna y se lo regala a una vecina.
 * Solo funciona si la comuna tiene bienes con cantidad y si no es la unica comuna.
 * La vecina se elige al azar entre la siguiente y la anterior.
 * Aumenta los bienes de las dos comunas igual que una transferencia normal.
 * Retorna 1 si logro regalar, 0 si no pudo.
 */
int regalarAlAzar(struct comuna* origen){
    if(origen==NULL || origen->bienes==NULL){
        return 0;
    }
    if(origen->siguiente==origen){
        return 0;
    }
    struct comuna* destino = origen->siguiente;
    int vecino = rand() % 2;
    if(vecino==1){
        destino = origen->anterior;
    }
    int totalBienes = contarBienes(origen);
    int intentos = 0;
    while(intentos<totalBienes){
        int idxBien = (rand() % totalBienes) + 1;
        struct bien* elegido = origen->bienes;
        for(int i=1; i<idxBien && elegido!=NULL; i++){
            elegido = elegido->siguiente;
        }
        if(elegido!=NULL && elegido->actualCantidad>0){
            int cantidad = (rand() % elegido->actualCantidad) + 1;
            int exito = realizarTransferencia(origen, destino, elegido->nombre, cantidad);
            if(exito==1){
                aumentarValores(origen, cantidad);
                aumentarValores(destino, cantidad);
                return 1;
            }
            return 0;
        }
        intentos = intentos + 1;
    }
    return 0;
}

/**
 * turnoComunas: Maneja el turno de las comunas que no administra el jugador.
 * Cada comuna autonoma elige una accion al azar del 0 al 4.
 * La accion 1 regala un bien a una comuna vecina.
 * Las acciones 0, 2, 3 y 4 dejan la comuna tranquila ese dia.
 * Las comunas del jugador no actuan aqui: esas las mueve el jugador.
 * Recorre la lista completa una sola vez, siempre avanzando a la siguiente.
 * Si alguna comuna llego a regalar se recalculan los indices de la sociedad.
 */
void turnoComunas(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    int accionMinima = 0;
    int accionMaxima = 4;
    int total = contarComunas(lista);
    int procesadas = 0;
    int huboRegalo = 0;
    struct comuna* temp = lista;
    while(procesadas<total){
        int accion = (rand() % (accionMaxima - accionMinima + 1)) + accionMinima;
        if(temp->administrada==0 && accion==1){
            int regalo = regalarAlAzar(temp);
            if(regalo==1){
                huboRegalo = 1;
            }
        }
        temp = temp->siguiente;
        procesadas = procesadas + 1;
    }
    if(huboRegalo==1){
        recalcularIndices(lista);
    }
}

/**
 * turnoDia: Maneja el turno del dia (jugador realiza transferencias).
 */
void turnoDia(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    int turnos = calcularTurnosPorDia(contarComunas(lista));
    printf("\nTienes %d turnos para realizar transferencias.\n", turnos);
    while(turnos>0){
        mostrarResumen(lista);
        mostrarMenu();
        int opcion;
        scanf("%d", &opcion);
        if(opcion==1){
            mostrarResumen(lista);
        }
        else if(opcion==2){
            printf("Selecciona comuna origen: ");
            int idxOrigen;
            scanf("%d", &idxOrigen);
            printf("Selecciona comuna destino: ");
            int idxDestino;
            scanf("%d", &idxDestino);
            if(idxOrigen<1 || idxDestino<1 || idxOrigen>contarComunas(lista) || idxDestino>contarComunas(lista)){
                printf("Indice invalido.\n");
                continue;
            }
            struct comuna* origen = lista;
            for(int i=1; i<idxOrigen; i++){
                origen = origen->siguiente;
            }
            struct comuna* destino = lista;
            for(int i=1; i<idxDestino; i++){
                destino = destino->siguiente;
            }
            if(origen==destino){
                printf("No puedes transferir a la misma comuna.\n");
                continue;
            }
            if(origen->administrada==0){
                printf("Solo puedes transferir con tus 3 comunas.\n");
                
                continue;
            }
            if(origen->siguiente!=destino && origen->anterior!=destino){
                printf("Las comunas no son vecinas.\n");
                continue;
            }
            printf("\nInventario de %s:\n", origen->nombre);
            struct bien* bienActual = origen->bienes;
            int contadorBien = 1;
            while(bienActual!=NULL){
                printf("  [%d] %s (%d/%d)\n", contadorBien, bienActual->nombre, bienActual->actualCantidad, bienActual->maximaCantidad);
                contadorBien = contadorBien + 1;
                bienActual = bienActual->siguiente;
            }
            printf("\nSelecciona bien (numero): ");
            int idxBien;
            scanf("%d", &idxBien);
            if(idxBien<1 || idxBien>contarBienes(origen)){
                printf("Numero invalido.\n");
                continue;
            }
            char* nombreBien = NULL;
            struct bien* actual = origen->bienes;
            for(int i=1; i<idxBien && actual!=NULL; i++){
                actual = actual->siguiente;
            }
            if(actual!=NULL){
                nombreBien = actual->nombre;
            }
            printf("Cantidad: ");
            int cantidad;
            scanf("%d", &cantidad);
            int exito = realizarTransferencia(origen, destino, nombreBien, cantidad);
            if(exito==1){
                aumentarValores(origen, cantidad);
                aumentarValores(destino, cantidad);
                turnos = turnos - 1;
            }
        }
        else if(opcion==3){
            printf("Selecciona comuna: ");
            int idx;
            scanf("%d", &idx);
            if(idx<1 || idx>contarComunas(lista)){
                printf("Indice invalido.\n");
                continue;
            }
            struct comuna* seleccionada = lista;
            for(int i=1; i<idx; i++){
                seleccionada = seleccionada->siguiente;
            }
            mostrarComuna(seleccionada);
            mostrarBienes(seleccionada);
            mostrarServicios(seleccionada);
        }
        else if(opcion==4){
            return;
        }
        else{
            printf("Opcion invalida.\n");
        }
    }
}

/**
 * turnoNoche: Maneja el turno de la noche (consumo, emergencias, disoluciones).
 */
void turnoNoche(struct comuna** lista){
    printf("\n");
    printf("########################################\n");
    printf("#  NOCHE\n");
    printf("########################################\n");
    aplicarConsumoNocturno(*lista);
    verificarEmergencias(*lista);
    // Recalculo antes de disolver para decidir con indices frescos del consumo
    recalcularIndices(*lista);
    verificarDisoluciones(lista);
    // Recalculo despues para reflejar a las personas que migraron
    recalcularIndices(*lista);
    actualizarPobreza(*lista);
    printf("Noche completada.\n");
}



/**
 * realizarTransferencia: Transfiere un bien de una comuna a otra vecina.
 * Verifica que sean vecinas, que haya suficiente cantidad y descuenta del origen.
 */
int realizarTransferencia(struct comuna* origen, struct comuna* destino, char* bien, int cantidad){
    if(origen==NULL || destino==NULL || bien==NULL || cantidad<=0){
        printf("Esos no son parametros para transferencia.\n");
        return 0;
    }
    if(estado.transferencias_bloqueadas){
        printf("Transferencia bloqueada por emergencia.\n");
        return 0;
    }
    if(origen->siguiente!=destino && origen->anterior!=destino){
        printf("Estimado/a, las comunas no son vecinas.\n");
        return 0;
    }
    struct bien* actual = origen->bienes;
    while(actual!=NULL){
        if(strcmp(actual->nombre, bien)==0){
            if(actual->actualCantidad<cantidad){
                printf("Claramente no hay suficiente %s en %s.\n", bien, origen->nombre);
                return 0;
            }
            actual->actualCantidad = actual->actualCantidad - cantidad;
            printf("Transferencia exitosa, todos ganan: %d de %s de %s a %s\n", cantidad, bien, origen->nombre, destino->nombre);
            return 1;
        }
        actual = actual->siguiente;
    }
    printf("Sinceramente no se encontro el bien '%s' en %s.\n", bien, origen->nombre);
    return 0;
}

/**
 * disolverComuna: Disuelve una comuna de la sociedad.
 * Parte de sus personas migra a comunas vecinas con sus oficios.
 * La minoria se queda sin hogar y sube la pobreza global.
 * Los bienes de la comuna se pierden. La quita de la lista circular.
 * Si la comuna era del jugador se pierde sin reposicion.
 */
void disolverComuna(struct comuna** lista, struct comuna* comuna){
    if(lista==NULL || comuna==NULL){
        return;
    }

    int total = contarPersonas(comuna);
    int pobres = (total * 30) / 100;
    int migran = total - pobres;
    int migraron = 0;
    int sinHogar = 0;

    struct comuna* vecinaIzquierda = comuna->anterior;
    struct comuna* vecinaDerecha = comuna->siguiente;
    int hayVecina = 1;
    if(vecinaDerecha==comuna){
        hayVecina = 0;
    }

    // Migrar la mayoria a las comunas vecinas, una persona a una
    while(migraron < migran && hayVecina==1 && comuna->habitantes!=NULL){
        struct comuna* destino = vecinaDerecha;
        int azar = rand() % 2;
        if(azar==1 && vecinaIzquierda!=vecinaDerecha){
            destino = vecinaIzquierda;
        }

        char nombre[50];
        strcpy(nombre, comuna->habitantes->nombre);
        int movida = migrarPersona(comuna, destino, nombre);
        if(movida==1){
            migraron = migraron + 1;
        }
        else{
            break;
        }
    }

    sinHogar = contarPersonas(comuna);
    estado.personas_fuera = estado.personas_fuera + sinHogar;

    printf("\n");
    printf("########################################\n");
    printf("#  LA COMUNA %s SE HA DISUELTO\n", comuna->nombre);
    if(hayVecina==0){
        printf("#  Era la ultima comuna, no hay donde migrar\n");
    }
    if(comuna->administrada==1){
        int restantes = contarAdministradas(*lista) - 1;
        printf("#  Era una comuna de tu gestion\n");
        printf("#  No hay reposicion, perdiste una de tus 3\n");
        if(restantes==1){
            printf("#  Te queda 1 comuna\n");
        }
        else{
            printf("#  Te quedan %d comunas\n", restantes);
        }
    }

    printf("#  %d personas migraron a comunas vecinas\n", migraron);
    printf("#  %d personas se quedaron sin hogar\n", sinHogar);
    printf("#  Sus bienes quedaron abandonados\n");
    printf("########################################\n");

    // Quitar la comuna de la lista circular doble
    if(comuna->siguiente==comuna){
        *lista = NULL;
    }
    else{
        comuna->anterior->siguiente = comuna->siguiente;
        comuna->siguiente->anterior = comuna->anterior;
        if(comuna==*lista){
            *lista = comuna->siguiente;
        }
    }

    liberarComuna(comuna);
    comuna = NULL;
}

/**
 * verificarDisoluciones: Verifica si alguna comuna debe disolverse.
 * Se disuelve si la necesidad es muy alta o la satisfaccion es muy baja.
 * Sus personas se reparten entre migracion y pobreza.
 * Si la lista queda vacia se vuelve NULL.
 */
void verificarDisoluciones(struct comuna** lista){
    if(lista==NULL || *lista==NULL){
        return;
    }

    float necesidadMaxima = 80.0;
    float satisfaccionMinima = 20.0;
    int total = contarComunas(*lista);
    int procesadas = 0;
    struct comuna* actual = *lista;

    while(procesadas < total && *lista!=NULL && actual!=NULL){
        struct comuna* siguiente = actual->siguiente;
        int disolver = 0;
        if(actual->necesidad >= necesidadMaxima){
            disolver = 1;
        }
        if(actual->satisfaccion <= satisfaccionMinima){
            disolver = 1;
        }

        if(disolver==1){
            disolverComuna(lista, actual);
        }

        procesadas = procesadas + 1;
        if(*lista==NULL){
            return;
        }

        actual = siguiente;
    }
}
