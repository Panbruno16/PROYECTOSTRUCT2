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
#include <dirent.h>

// estructura global del juego
struct juego estado;

// Arbol AVL con los nombres de todos los libros del corpus
struct nodoAVL* catalogoLibros = NULL;

/**
 * crearComuna: Crea una nueva comuna con el nombre especificado.
 * Inicializa los indices en 0 y los punteros de la lista circular apuntan a si mismo.
 * Nace sin regalos dados ni recibidos y como comuna autonoma (administrada en 0).
 */
struct comuna* crearComuna(char* nombre){
    struct comuna* nueva = calloc(1, sizeof(struct comuna));
    strcpy(nueva->nombre, nombre);
    nueva->necesidad = 0.0;
    nueva->necesidadLocal = 0.0;
    nueva->satisfaccion = 0.0;
    nueva->pobreza = 0.0;
    nueva->reciprocidad = 0.0;
    nueva->regalosDados = 0;
    nueva->regalosRecibidos = 0;
    nueva->administrada = 0;
    nueva->cache = NULL;
    nueva->siguiente = nueva;
    nueva->anterior = nueva;
    return nueva;
}

/**
 * mostrarComuna: Muestra en pantalla la informacion de una comuna.
 * Imprime el nombre y los indices de necesidad, satisfaccion, pobreza y reciprocidad.
 * Tambien muestra la necesidad local y el balance de regalos dados y recibidos.
 */
void mostrarComuna(struct comuna* comuna){
    if(comuna==NULL){
        return;
    }
    printf("\n");
    printf("########################################\n");
    printf("#  COMUNA: %s\n", comuna->nombre);
    printf("########################################\n");
    printf("#  Necesidad:         %.2f%%\n", comuna->necesidad);
    printf("#  Necesidad local:   %.2f%%\n", comuna->necesidadLocal);
    printf("#  Satisfaccion:      %.2f%%\n", comuna->satisfaccion);
    printf("#  Pobreza:           %.2f%%\n", comuna->pobreza);
    printf("#  Reciprocidad:      %.2f\n", comuna->reciprocidad);
    printf("#  Regalos dados:     %d\n", comuna->regalosDados);
    printf("#  Regalos recibidos: %d\n", comuna->regalosRecibidos);
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
    nuevo->servicios = NULL;
    nuevo->librosLeidos = 0;
    nuevo->aprendioLeyendo = 0;
    nuevo->libros = NULL;
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
        printf("#  %d. %s (libros leidos: %d)\n", contador, actual->nombre, actual->librosLeidos);
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
    if(total==0){
        printf("Error: el archivo %s no tiene ningun nombre de comuna\n", nombreArchivo);
        return NULL;
    }
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
    if(lista==NULL){
        printf("Error: no se pudo crear ninguna comuna\n");
        return NULL;
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
 * El jugador empieza con 3 y siempre se le reasigna otra cuando pierde una.
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
 * calcularNecesidadLocal: Calcula la necesidad que sale solo de los bienes.
 * Recorre los bienes y calcula el promedio de (actual/maxima) de llenado.
 * La necesidad local es 100 menos ese llenado: alto = le falta recursos,
 * bajo = tiene recursos. Esta es la parte local del indice de necesidad.
 */
float calcularNecesidadLocal(struct comuna* comuna){
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
 * calcularReciprocidad: Calcula el balance entre regalos recibidos y dados.
 * Se calcula como: (recibidos - dados) / (recibidos + dados) * 100.
 * El resultado va de -100 (solo regala) a 100 (solo recibe).
 * Si no ha habido ningun regalo el indice queda en 0 (neutral).
 */
float calcularReciprocidad(struct comuna* comuna){
    if(comuna==NULL){
        return 0.0;
    }
    int dados = comuna->regalosDados;
    int recibidos = comuna->regalosRecibidos;
    int total = dados + recibidos;
    if(total==0){
        return 0.0;
    }
    float balance = ((float)(recibidos - dados) / (float)total) * 100.0;
    return balance;
}

/**
 * calcularNecesidad: Calcula el indice final de necesidad de una comuna.
 * Junta 5 elementos, cada uno con su peso:
 *   40% necesidad local (sus bienes)
 *   15% necesidad de las comunas circundantes
 *   15% 100 menos la satisfaccion de las comunas circundantes
 *   15% 100 menos el apoyo recibido en regalos
 *   15% presion por el indice de reciprocidad
 * Las vecinas que estan mal no pueden ayudar y suben la necesidad.
 * Recibir regalos cubre carencias y baja la necesidad.
 * Regalar sin recibir deja la comuna mas necesitada.
 * Para no usar valores a medias se toma la necesidad local de las vecinas.
 * Retorna un valor entre 0 y 100.
 */
float calcularNecesidad(struct comuna* comuna){
    if(comuna==NULL){
        return 0.0;
    }

    // 1. Necesidad local: solo sus bienes
    float local = comuna->necesidadLocal;

    // 2. Necesidades de las comunas circundantes
    //    y 3. su satisfaccion. Si no hay vecinas se usan los valores propios.
    float necesidadVecinas = local;
    float satisfaccionVecinas = comuna->satisfaccion;
    if(comuna->siguiente!=comuna){
        if(comuna->siguiente==comuna->anterior){
            // Solo hay dos comunas en toda la sociedad
            necesidadVecinas = comuna->siguiente->necesidadLocal;
            satisfaccionVecinas = comuna->siguiente->satisfaccion;
        }
        else{
            necesidadVecinas = (comuna->siguiente->necesidadLocal + comuna->anterior->necesidadLocal) / 2.0;
            satisfaccionVecinas = (comuna->siguiente->satisfaccion + comuna->anterior->satisfaccion) / 2.0;
        }
    }

    // 4. Apoyo recibido en regalos: cuanto cubrio sus carencias
    int totalRegalos = comuna->regalosDados + comuna->regalosRecibidos;
    float apoyoRecibido = 50.0;
    if(totalRegalos>0){
        apoyoRecibido = ((float)comuna->regalosRecibidos / (float)totalRegalos) * 100.0;
    }

    // 5. Presion del balance de reciprocidad: -100 (solo regala) presiona 100
    float presionReciprocidad = (100.0 - comuna->reciprocidad) / 2.0;

    float necesidad = (local * 0.40)
                    + (necesidadVecinas * 0.15)
                    + ((100.0 - satisfaccionVecinas) * 0.15)
                    + ((100.0 - apoyoRecibido) * 0.15)
                    + (presionReciprocidad * 0.15);

    if(necesidad>100.0){
        necesidad = 100.0;
    }
    if(necesidad<0.0){
        necesidad = 0.0;
    }
    return necesidad;
}

/**
 * calcularSatisfaccion: Calcula el indice de satisfaccion de una comuna.
 * Se calcula aparte de la necesidad, solo con lo local:
 * el llenado de bienes (100 - necesidad local) y el porcentaje de personas con oficio.
 * Mas recursos y mas gente con oficio = mas satisfecha.
 * No depende de las comunas vecinas.
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
    float satisfaccion = ((100.0 - comuna->necesidadLocal)*0.6)+(ratioServicios*40.0);
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
 * Primero la necesidad local y la reciprocidad de cada comuna.
 * Luego la satisfaccion de cada comuna con su necesidad local.
 * Al final la necesidad final usando tambien a las comunas vecinas.
 * Se hacen en ese orden para no usar valores a medias.
 */
void recalcularIndices(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    struct comuna* actual = lista;
    do{
        actual->necesidadLocal = calcularNecesidadLocal(actual);
        actual->reciprocidad = calcularReciprocidad(actual);
        actual = actual->siguiente;
    }while(actual!=lista);

    actual = lista;
    do{
        actual->satisfaccion = calcularSatisfaccion(actual);
        actual = actual->siguiente;
    }while(actual!=lista);

    actual = lista;
    do{
        actual->necesidad = calcularNecesidad(actual);
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
 * bajarBienes: Reduce los bienes de todas las comunas en un porcentaje.
 * Se usa en las emergencias que anuncian perdida de bienes (temblor e
 * inundacion). Aunque el porcentaje redondee a cero, si la comuna tiene
 * bienes pierde al menos una unidad, para que la emergencia siempre se note.
 */
void bajarBienes(struct comuna* lista, int porcentaje){
    if(lista==NULL){
        return;
    }
    struct comuna* actual = lista;
    do{
        struct bien* bien = actual->bienes;
        while(bien!=NULL){
            int perdida = (bien->actualCantidad * porcentaje) / 100;
            if(perdida<1 && bien->actualCantidad>0){
                perdida = 1;
            }
            bien->actualCantidad = bien->actualCantidad - perdida;
            if(bien->actualCantidad<0){
                bien->actualCantidad = 0;
            }
            bien = bien->siguiente;
        }
        actual = actual->siguiente;
    }while(actual!=lista);
}

/**
 * perderOficios: Quita un oficio a una parte de las personas de la sociedad.
 * Se usa en el temblor: la gente perdio sus herramientas y su trabajo.
 * Cada persona tiene un 20% de probabilidad de perder su primer oficio.
 * Las personas que no tienen oficio no se ven afectadas.
 */
void perderOficios(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    struct comuna* comunaActual = lista;
    do{
        struct persona* personaActual = comunaActual->habitantes;
        while(personaActual!=NULL){
            int azar = rand() % 5;
            if(azar==0 && personaActual->servicios!=NULL){
                struct servicio* perdido = personaActual->servicios;
                personaActual->servicios = perdido->siguiente;
                free(perdido);
            }
            personaActual = personaActual->siguiente;
        }
        comunaActual = comunaActual->siguiente;
    }while(comunaActual!=lista);
}

/**
 * verificarEmergencias: Verifica y aplica emergencias aleatorias.
 * Huelga: probabilidad base 10% + pobreza * 0.3%. Bloquea los intercambios
 *         del dia siguiente, igual que la presidencia.
 * Vandalos: probabilidad base 5% + pobreza * 0.4%. No hace nada, como dice
 *           el mensaje, porque no es dia de revision.
 * Temblor: probabilidad base 3%. Baja los bienes 25% y deja a una parte de
 *          las personas sin oficio.
 * Inundacion: probabilidad base 2%. Baja los bienes 40%.
 * Feriado: probabilidad base 4%. Deja el consumo pausado para la noche
 *          siguiente y bloquea los intercambios del dia siguiente.
 * Presidencia: probabilidad base 3%. Bloquea los intercambios del dia
 *              siguiente.
 */
void verificarEmergencias(struct comuna* lista){
    if(lista==NULL){
        return;
    }
    float pobreza = estado.pobreza_global;

    // Huelga: probabilidad base 10% + pobreza * 0.3%
    float probabilidadHuelga = 0.10+(pobreza*0.003);
    if((float)rand()/RAND_MAX < probabilidadHuelga){
        estado.transferencias_bloqueadas = 1;
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
        bajarBienes(lista, 25);
        perderOficios(lista);
        printf("TEMBLOR! Los servicios y bienes bajaron, debido a que casi se nos va Panama.\n");
    }

    // Inundacion: probabilidad base 2%
    float probabilidadInundacion = 0.02;
    if((float)rand()/RAND_MAX < probabilidadInundacion){
        bajarBienes(lista, 40);
        printf("INUNDACION! Los bienes bajaron, ahora resulta que vivimos como sirenas.\n");
    }

    // Feriado: probabilidad base 4%
    float probabilidadFeriado = 0.04;
    if((float)rand()/RAND_MAX < probabilidadFeriado){
        estado.transferencias_bloqueadas = 1;
        estado.consumo_pausado = 1;
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
        liberarLibrosLeidos(personaActual);
        struct persona* temporal = personaActual;
        personaActual = personaActual->siguiente;
        free(temporal);
        temporal = NULL;
    }
    liberarSplay(comuna->cache);
    comuna->cache = NULL;
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
    printf("#  [5] Ir a la biblioteca\n");
    printf("########################################\n");
    printf("\n");
    printf("Selecciona una opcion: ");
}

/**
 * alturaAVL: Devuelve la altura de un nodo del arbol AVL.
 * Un nodo vacio tiene altura 0.
 */
int alturaAVL(struct nodoAVL* nodo){
    if(nodo==NULL){
        return 0;
    }
    return nodo->altura;
}

/**
 * maximoEntero: Devuelve el mayor de dos numeros enteros.
 */
int maximoEntero(int primero, int segundo){
    if(primero>segundo){
        return primero;
    }
    return segundo;
}

/**
 * factorBalance: Devuelve la diferencia entre la altura del hijo izquierdo
 * y la del hijo derecho. Si es menor que -1 o mayor que 1 el subarbol
 * esta desbalanceado y hay que rotarlo.
 */
int factorBalance(struct nodoAVL* nodo){
    if(nodo==NULL){
        return 0;
    }
    return alturaAVL(nodo->izquierda) - alturaAVL(nodo->derecha);
}

/**
 * rotarDerecha: Rota hacia la derecha un subarbol que quedo mas alto por la
 * izquierda. El hijo izquierdo sube a ser la raiz y el nodo actual baja a
 * ser su hijo derecho.
 * Retorna la nueva raiz del subarbol.
 */
struct nodoAVL* rotarDerecha(struct nodoAVL* nodo){
    struct nodoAVL* centro = nodo->izquierda;
    struct nodoAVL* temporal = centro->derecha;
    centro->derecha = nodo;
    nodo->izquierda = temporal;
    nodo->altura = 1 + maximoEntero(alturaAVL(nodo->izquierda), alturaAVL(nodo->derecha));
    centro->altura = 1 + maximoEntero(alturaAVL(centro->izquierda), alturaAVL(centro->derecha));
    return centro;
}

/**
 * rotarIzquierda: Rota hacia la izquierda un subarbol que quedo mas alto por
 * la derecha. El hijo derecho sube a ser la raiz y el nodo actual baja a
 * ser su hijo izquierdo.
 * Retorna la nueva raiz del subarbol.
 */
struct nodoAVL* rotarIzquierda(struct nodoAVL* nodo){
    struct nodoAVL* centro = nodo->derecha;
    struct nodoAVL* temporal = centro->izquierda;
    centro->izquierda = nodo;
    nodo->derecha = temporal;
    nodo->altura = 1 + maximoEntero(alturaAVL(nodo->izquierda), alturaAVL(nodo->derecha));
    centro->altura = 1 + maximoEntero(alturaAVL(centro->izquierda), alturaAVL(centro->derecha));
    return centro;
}

/**
 * insertarAVL: Inserta el nombre de un libro en el arbol AVL y lo deja
 * balanceado con las rotaciones que hagan falta.
 * Si el nombre ya estaba en el arbol no lo vuelve a insertar.
 * Retorna la nueva raiz del arbol.
 */
struct nodoAVL* insertarAVL(struct nodoAVL* raiz, char* nombre){
    if(raiz==NULL){
        struct nodoAVL* nuevo = calloc(1, sizeof(struct nodoAVL));
        strcpy(nuevo->nombre, nombre);
        nuevo->altura = 1;
        nuevo->izquierda = NULL;
        nuevo->derecha = NULL;
        return nuevo;
    }

    int comparacion = strcmp(nombre, raiz->nombre);
    if(comparacion<0){
        raiz->izquierda = insertarAVL(raiz->izquierda, nombre);
    }
    else if(comparacion>0){
        raiz->derecha = insertarAVL(raiz->derecha, nombre);
    }
    else{
        return raiz;
    }

    raiz->altura = 1 + maximoEntero(alturaAVL(raiz->izquierda), alturaAVL(raiz->derecha));
    int balance = factorBalance(raiz);

    if(balance>1 && strcmp(nombre, raiz->izquierda->nombre)<0){
        return rotarDerecha(raiz);
    }
    if(balance<-1 && strcmp(nombre, raiz->derecha->nombre)>0){
        return rotarIzquierda(raiz);
    }
    if(balance>1 && strcmp(nombre, raiz->izquierda->nombre)>0){
        raiz->izquierda = rotarIzquierda(raiz->izquierda);
        return rotarDerecha(raiz);
    }
    if(balance<-1 && strcmp(nombre, raiz->derecha->nombre)<0){
        raiz->derecha = rotarDerecha(raiz->derecha);
        return rotarIzquierda(raiz);
    }
    return raiz;
}

/**
 * buscarAVL: Busca un libro en el arbol AVL por coincidencia EXACTA del
 * nombre del archivo. Retorna el nodo si lo encuentra y NULL si no.
 */
struct nodoAVL* buscarAVL(struct nodoAVL* raiz, char* nombre){
    if(raiz==NULL){
        return NULL;
    }
    int comparacion = strcmp(nombre, raiz->nombre);
    if(comparacion==0){
        return raiz;
    }
    if(comparacion<0){
        return buscarAVL(raiz->izquierda, nombre);
    }
    return buscarAVL(raiz->derecha, nombre);
}

/**
 * liberarAVL: Libera todos los nodos de un arbol AVL.
 * Va primero por los hijos y al final libera el nodo actual.
 */
void liberarAVL(struct nodoAVL* raiz){
    if(raiz==NULL){
        return;
    }
    liberarAVL(raiz->izquierda);
    liberarAVL(raiz->derecha);
    free(raiz);
}

/**
 * cargarCatalogoLibros: Recorre la carpeta del juego y mete en el arbol AVL
 * todos los archivos .txt del corpus, menos el record.txt que es de uso
 * interno del programa. Se llama al arrancar el programa.
 * Retorna la cantidad de libros cargados.
 */
int cargarCatalogoLibros(){
    if(catalogoLibros!=NULL){
        liberarAVL(catalogoLibros);
        catalogoLibros = NULL;
    }
    DIR* carpeta = opendir(".");
    if(carpeta==NULL){
        printf("Error: no se pudo abrir la carpeta del juego\n");
        return 0;
    }
    struct dirent* entrada;
    int cargados = 0;
    while((entrada = readdir(carpeta))!=NULL){
        char* nombre = entrada->d_name;
        int largo = strlen(nombre);
        if(largo>4 && strcmp(nombre+largo-4, ".txt")==0 && strcmp(nombre, "record.txt")!=0){
            catalogoLibros = insertarAVL(catalogoLibros, nombre);
            cargados = cargados + 1;
        }
    }
    closedir(carpeta);
    return cargados;
}

/**
 * liberarCatalogoLibros: Libera el arbol AVL del catalogo de libros.
 * Se llama al terminar el juego.
 */
void liberarCatalogoLibros(){
    liberarAVL(catalogoLibros);
    catalogoLibros = NULL;
}

/**
 * pasarMinusculas: Pasa todo el texto a minusculas.
 * Sirve para que "Quijote" y "quijote" se busquen igual y compartan la
 * misma entrada en el arbol chaflaneado.
 */
void pasarMinusculas(char* texto){
    int i = 0;
    while(texto[i]!=0){
        if(texto[i]>='A' && texto[i]<='Z'){
            texto[i] = texto[i] + ('a' - 'A');
        }
        i = i + 1;
    }
}

/**
 * rotarDerechaSplay: Rota hacia la derecha el subarbol chaflaneado.
 * El hijo izquierdo sube a ser la raiz y el nodo actual baja a ser su
 * hijo derecho. Imprime la rotacion para que se vea el chaflaneo.
 * Retorna la nueva raiz del subarbol.
 */
struct nodoSplay* rotarDerechaSplay(struct nodoSplay* nodo){
    struct nodoSplay* centro = nodo->izquierda;
    struct nodoSplay* temporal = centro->derecha;
    centro->derecha = nodo;
    nodo->izquierda = temporal;
    printf("     rotacion zig a la derecha: %s sube a la raiz\n", centro->texto);
    return centro;
}

/**
 * rotarIzquierdaSplay: Rota hacia la izquierda el subarbol chaflaneado.
 * El hijo derecho sube a ser la raiz y el nodo actual baja a ser su
 * hijo izquierdo. Imprime la rotacion para que se vea el chaflaneo.
 * Retorna la nueva raiz del subarbol.
 */
struct nodoSplay* rotarIzquierdaSplay(struct nodoSplay* nodo){
    struct nodoSplay* centro = nodo->derecha;
    struct nodoSplay* temporal = centro->izquierda;
    centro->izquierda = nodo;
    nodo->derecha = temporal;
    printf("     rotacion zig a la izquierda: %s sube a la raiz\n", centro->texto);
    return centro;
}

/**
 * chaflanear: Sube hasta la raiz el nodo con el texto buscado.
 * Si el texto ya esta en el cache solo se reacomoda el arbol, si no esta
 * el arbol queda como estaba. Baja por el arbol comparando y despues
 * sube con las rotaciones zig (simple), zig-zig y zig-zag.
 * Cada rotacion que hace la imprime, para que se vea el chaflaneo.
 * Retorna la nueva raiz del arbol.
 */
struct nodoSplay* chaflanear(struct nodoSplay* raiz, char* texto){
    if(raiz==NULL){
        return NULL;
    }
    int comparacion = strcmp(texto, raiz->texto);
    if(comparacion==0){
        return raiz;
    }

    if(comparacion<0){
        if(raiz->izquierda==NULL){
            return raiz;
        }
        int interna = strcmp(texto, raiz->izquierda->texto);
        if(interna<0){
            raiz->izquierda->izquierda = chaflanear(raiz->izquierda->izquierda, texto);
            raiz = rotarDerechaSplay(raiz);
        }
        else if(interna>0){
            raiz->izquierda->derecha = chaflanear(raiz->izquierda->derecha, texto);
            if(raiz->izquierda->derecha!=NULL){
                raiz->izquierda = rotarIzquierdaSplay(raiz->izquierda);
            }
        }
        if(raiz->izquierda==NULL){
            return raiz;
        }
        return rotarDerechaSplay(raiz);
    }

    if(raiz->derecha==NULL){
        return raiz;
    }
    int interna = strcmp(texto, raiz->derecha->texto);
    if(interna>0){
        raiz->derecha->derecha = chaflanear(raiz->derecha->derecha, texto);
        raiz = rotarIzquierdaSplay(raiz);
    }
    else if(interna<0){
        raiz->derecha->izquierda = chaflanear(raiz->derecha->izquierda, texto);
        if(raiz->derecha->izquierda!=NULL){
            raiz->derecha = rotarDerechaSplay(raiz->derecha);
        }
    }
    if(raiz->derecha==NULL){
        return raiz;
    }
    return rotarIzquierdaSplay(raiz);
}

/**
 * guardarEnCache: Agrega el resultado de una busqueda al arbol chaflaneado
 * y lo deja en la raiz, asi la proxima vez se encuentra en el primer
 * intento. Si el texto ya estaba no lo vuelve a agregar.
 * Retorna la nueva raiz del arbol.
 */
struct nodoSplay* guardarEnCache(struct nodoSplay* raiz, char* texto, char archivos[][50], int cantidad){
    struct nodoSplay* nuevo = calloc(1, sizeof(struct nodoSplay));
    strcpy(nuevo->texto, texto);
    nuevo->cantidadArchivos = cantidad;
    int i = 0;
    while(i<cantidad && i<50){
        strcpy(nuevo->archivos[i], archivos[i]);
        i = i + 1;
    }
    nuevo->izquierda = NULL;
    nuevo->derecha = NULL;

    if(raiz==NULL){
        return nuevo;
    }
    if(strcmp(texto, raiz->texto)<0){
        nuevo->izquierda = raiz->izquierda;
        nuevo->derecha = raiz;
        raiz->izquierda = NULL;
    }
    else{
        nuevo->derecha = raiz->derecha;
        nuevo->izquierda = raiz;
        raiz->derecha = NULL;
    }
    return nuevo;
}

/**
 * buscarTextoEnArchivos: Recorre todos los .txt de la carpeta (el corpus)
 * y junta los nombres de los archivos que contienen el texto.
 * La comparacion no distingue mayusculas de minusculas y se hace linea por
 * linea. Retorna la cantidad de archivos encontrados, hasta el maximo.
 */
int buscarTextoEnArchivos(char* texto, char archivos[][50], int maximo){
    DIR* carpeta = opendir(".");
    if(carpeta==NULL){
        printf("Error: no se pudo abrir la carpeta del juego\n");
        return 0;
    }
    struct dirent* entrada;
    int encontrados = 0;
    while((entrada = readdir(carpeta))!=NULL && encontrados<maximo){
        char* nombre = entrada->d_name;
        int largo = strlen(nombre);
        if(largo>4 && strcmp(nombre+largo-4, ".txt")==0 && strcmp(nombre, "record.txt")!=0){
            FILE* libro = fopen(nombre, "r");
            if(libro!=NULL){
                char linea[500];
                int contiene = 0;
                while(fgets(linea, 500, libro) && contiene==0){
                    pasarMinusculas(linea);
                    if(strstr(linea, texto)!=NULL){
                        contiene = 1;
                    }
                }
                fclose(libro);
                if(contiene==1){
                    strcpy(archivos[encontrados], nombre);
                    encontrados = encontrados + 1;
                }
            }
        }
    }
    closedir(carpeta);
    return encontrados;
}

/**
 * mostrarArchivos: Imprime la lista de nombres de archivos que contienen
 * el texto buscado.
 */
void mostrarArchivos(char archivos[][50], int cantidad){
    if(cantidad==0){
        printf("  Ningun libro contiene ese texto\n");
        return;
    }
    printf("  Libros que lo contienen:\n");
    int i = 0;
    while(i<cantidad){
        printf("    %d. %s\n", i+1, archivos[i]);
        i = i + 1;
    }
}

/**
 * leerLinea: Lee una linea completa desde el teclado, hasta el salto de
 * linea. Sirve para buscar textos con espacios.
 * Descarta el salto de linea que dejo la lectura anterior del menu.
 * Retorna 1 si leyo algo y 0 si se acabo la entrada (EOF).
 */
int leerLinea(char* destino, int maximo){
    int c = getchar();
    while(c==' ' || c=='\t' || c=='\r' || c=='\n'){
        c = getchar();
    }
    if(c==EOF){
        destino[0] = 0;
        return 0;
    }
    int posicion = 0;
    while(c!='\n' && c!='\r' && c!=EOF && posicion<maximo-1){
        destino[posicion] = (char)c;
        posicion = posicion + 1;
        c = getchar();
    }
    destino[posicion] = 0;
    while(c!='\n' && c!='\r' && c!=EOF){
        c = getchar();
    }
    return 1;
}

/**
 * agregarLibroLeido: Anota un libro en la lista de libros leidos de la
 * persona. Inserta el libro al inicio de la lista enlazada y sube su
 * contador de libros leidos.
 */
void agregarLibroLeido(struct persona* persona, char* nombre){
    if(persona==NULL){
        return;
    }
    struct libroLeido* nuevo = calloc(1, sizeof(struct libroLeido));
    strcpy(nuevo->nombre, nombre);
    nuevo->siguiente = persona->libros;
    persona->libros = nuevo;
    persona->librosLeidos = persona->librosLeidos + 1;
}

/**
 * personaYaLeyo: Revisa si la persona ya leyo ese libro antes.
 * Recorre la lista de libros leidos comparando los nombres.
 * Retorna 1 si ya lo leyo y 0 si todavia no.
 */
int personaYaLeyo(struct persona* persona, char* nombre){
    if(persona==NULL){
        return 0;
    }
    struct libroLeido* actual = persona->libros;
    while(actual!=NULL){
        if(strcmp(actual->nombre, nombre)==0){
            return 1;
        }
        actual = actual->siguiente;
    }
    return 0;
}

/**
 * liberarLibrosLeidos: Libera la lista de libros leidos de una persona.
 * Va primero por el siguiente libro y al final libera el actual.
 * Se llama cuando se libera la comuna y sus habitantes.
 */
void liberarLibrosLeidos(struct persona* persona){
    if(persona==NULL){
        return;
    }
    struct libroLeido* actual = persona->libros;
    while(actual!=NULL){
        struct libroLeido* temporal = actual;
        actual = actual->siguiente;
        free(temporal);
        temporal = NULL;
    }
    persona->libros = NULL;
}

/**
 * personaTieneServicio: Revisa si la persona ya sabe ese oficio.
 * Recorre la lista de servicios comparando los nombres.
 * Retorna 1 si lo sabe y 0 si no.
 */
int personaTieneServicio(struct persona* persona, char* nombre){
    if(persona==NULL){
        return 0;
    }
    struct servicio* actual = persona->servicios;
    while(actual!=NULL){
        if(strcmp(actual->nombre, nombre)==0){
            return 1;
        }
        actual = actual->siguiente;
    }
    return 0;
}

/**
 * aprenderServicioPorLectura: La persona aprende un servicio nuevo por
 * haber leido libros en la biblioteca.
 * Mira la carpeta de servicios, junta los oficios que la persona todavia
 * no sabe y elige uno al azar entre esos. Si ya sabe todos no aprende nada.
 * Retorna 1 si aprendio algo y 0 si no quedan oficios por aprender.
 */
int aprenderServicioPorLectura(struct persona* persona){
    if(persona==NULL){
        return 0;
    }
    FILE* archivo = fopen("servicios", "r");
    if(archivo==NULL){
        printf("  Aviso: no se pudo abrir la carpeta de servicios\n");
        return 0;
    }
    char disponibles[20][50];
    int total = 0;
    char linea[50];
    while(fgets(linea, 50, archivo)!=NULL && total<20){
        linea[strcspn(linea, "\n")] = 0;
        if(linea[0]!=0 && personaTieneServicio(persona, linea)==0){
            strcpy(disponibles[total], linea);
            total = total + 1;
        }
    }
    fclose(archivo);
    if(total==0){
        printf("  %s ya sabe todos los servicios del catalogo\n", persona->nombre);
        return 0;
    }
    int azar = rand() % total;
    agregarServicioAPersona(persona, disponibles[azar]);
    printf("  %s aprendio el servicio: %s\n", persona->nombre, disponibles[azar]);
    return 1;
}

/**
 * anotarLectura: Anota los libros que acaba de leer la persona.
 * Cada archivo que muestra la busqueda es un libro leido, pero si la
 * persona ya lo habia leido antes no se vuelve a contar.
 * Cuando llega a 3 libros o mas aprende un servicio nuevo, y eso pasa
 * una sola vez.
 * Imprime cuantos libros llevo y si aprendio algo.
 */
void anotarLectura(struct persona* lector, char archivos[][50], int cantidad){
    if(lector==NULL){
        return;
    }
    int nuevos = 0;
    int i = 0;
    while(i<cantidad){
        if(personaYaLeyo(lector, archivos[i])==0){
            agregarLibroLeido(lector, archivos[i]);
            nuevos = nuevos + 1;
        }
        i = i + 1;
    }
    if(nuevos==0){
        printf("  %s no encontro libros nuevos: esos ya los habia leido\n", lector->nombre);
        return;
    }
    printf("  %s leyo %d libro(s) nuevo(s), ya lleva %d leido(s)\n", lector->nombre, nuevos, lector->librosLeidos);
    if(lector->librosLeidos>=3 && lector->aprendioLeyendo==0){
        lector->aprendioLeyendo = 1;
        aprenderServicioPorLectura(lector);
    }
}

/**
 * buscarTextoEnLibros: Pide un texto al jugador y busca en los libros que
 * lo contienen.
 * Primero mira el arbol chaflaneado de esa biblioteca (cache): si el texto
 * ya se busco antes lo trae hasta la raiz con rotaciones y no vuelve a
 * recorrer los archivos. Si no esta, recorre los archivos, guarda el
 * resultado en el arbol y lo deja en la raiz.
 * Cada archivo que muestra lo anota como leido por la persona que hizo la
 * busqueda, y con 3 libros o mas esa persona aprende un servicio nuevo.
 */
void buscarTextoEnLibros(struct comuna* biblioteca, struct persona* lector){
    if(biblioteca==NULL){
        return;
    }
    printf("Texto a buscar (puede llevar espacios): ");
    char texto[50];
    if(leerLinea(texto, 50)==0){
        printf("\nSe acabo la entrada.\n");
        return;
    }
    if(texto[0]==0){
        printf("No escribiste ningun texto.\n");
        return;
    }
    pasarMinusculas(texto);

    printf("Buscando en el arbol chaflaneado de %s...\n", biblioteca->nombre);
    biblioteca->cache = chaflanear(biblioteca->cache, texto);

    if(biblioteca->cache!=NULL && strcmp(biblioteca->cache->texto, texto)==0){
        printf("ESTABA EN EL CACHE: no se tocaron los archivos, solo se chaflaneo el arbol.\n");
        mostrarArchivos(biblioteca->cache->archivos, biblioteca->cache->cantidadArchivos);
        anotarLectura(lector, biblioteca->cache->archivos, biblioteca->cache->cantidadArchivos);
        return;
    }

    printf("No estaba en el cache: se buscaron los archivos del corpus.\n");
    char archivos[50][50];
    int encontrados = buscarTextoEnArchivos(texto, archivos, 50);
    biblioteca->cache = guardarEnCache(biblioteca->cache, texto, archivos, encontrados);
    printf("Resultado guardado en el arbol chaflaneado, quedo en la raiz.\n");
    mostrarArchivos(archivos, encontrados);
    anotarLectura(lector, archivos, encontrados);
}

/**
 * liberarSplay: Libera todos los nodos de un arbol chaflaneado.
 * Va primero por los hijos y al final libera el nodo actual.
 */
void liberarSplay(struct nodoSplay* raiz){
    if(raiz==NULL){
        return;
    }
    liberarSplay(raiz->izquierda);
    liberarSplay(raiz->derecha);
    free(raiz);
}

/**
 * mostrarMenuBiblioteca: Muestra el menu de la biblioteca comunitaria.
 */
void mostrarMenuBiblioteca(){
    printf("\n");
    printf("########################################\n");
    printf("#  BIBLIOTECA COMUNITARIA\n");
    printf("########################################\n");
    printf("#  [1] Buscar texto en los libros\n");
    printf("#  [2] Buscar libro por nombre\n");
    printf("#  [3] Volver al menu\n");
    printf("########################################\n");
    printf("\n");
    printf("Selecciona una opcion: ");
}

/**
 * buscarLibroPorNombre: Pide el nombre de un libro al jugador y lo busca
 * en el arbol AVL por coincidencia exacta.
 * Si no existe ningun libro con ese nombre lo dice claramente.
 */
void buscarLibroPorNombre(){
    printf("Nombre del libro (ejemplo: moby_dick.txt): ");
    char nombre[50];
    if(scanf("%49s", nombre)!=1){
        if(limpiarEntrada()==0){
            printf("\nSe acabo la entrada.\n");
            return;
        }
        printf("Entrada invalida, escribe el nombre del libro.\n");
        return;
    }
    struct nodoAVL* encontrado = buscarAVL(catalogoLibros, nombre);
    if(encontrado!=NULL){
        printf("Libro encontrado en el catalogo: %s\n", encontrado->nombre);
    }
    else{
        printf("No existe ningun libro con el nombre %s\n", nombre);
    }
}

/**
 * irALaBiblioteca: Maneja la visita del jugador a la biblioteca de una comuna.
 * Elige la comuna (solo puede usar las suyas) y una persona de esa comuna
 * y despues muestra el menu de la biblioteca.
 * Retorna 1 si la entrada sigue viva y 0 si se acabo la entrada (EOF).
 */
int irALaBiblioteca(struct comuna* lista){
    if(lista==NULL){
        return 1;
    }
    printf("\nSelecciona comuna (solo tus comunas): ");
    int idxComuna;
    if(scanf("%d", &idxComuna)!=1){
        if(limpiarEntrada()==0){
            return 0;
        }
        printf("Entrada invalida, escribe un numero.\n");
        return 1;
    }
    if(idxComuna<1 || idxComuna>contarComunas(lista)){
        printf("Indice invalido.\n");
        return 1;
    }
    struct comuna* biblioteca = lista;
    for(int i=1; i<idxComuna; i++){
        biblioteca = biblioteca->siguiente;
    }
    if(biblioteca->administrada==0){
        printf("Solo puedes usar la biblioteca de tus comunas.\n");
        return 1;
    }

    mostrarPersonas(biblioteca);
    printf("Selecciona persona: ");
    int idxPersona;
    if(scanf("%d", &idxPersona)!=1){
        if(limpiarEntrada()==0){
            return 0;
        }
        printf("Entrada invalida, escribe un numero.\n");
        return 1;
    }
    if(idxPersona<1 || idxPersona>contarPersonas(biblioteca)){
        printf("Indice invalido.\n");
        return 1;
    }
    struct persona* lectora = biblioteca->habitantes;
    for(int i=1; i<idxPersona; i++){
        lectora = lectora->siguiente;
    }
    printf("\n%s entra a la biblioteca de %s\n", lectora->nombre, biblioteca->nombre);

    int seguir = 1;
    while(seguir==1){
        mostrarMenuBiblioteca();
        int opcion;
        if(scanf("%d", &opcion)!=1){
            if(limpiarEntrada()==0){
                return 0;
            }
            printf("Entrada invalida, escribe un numero.\n");
            continue;
        }
        if(opcion==1){
            buscarTextoEnLibros(biblioteca, lectora);
        }
        else if(opcion==2){
            buscarLibroPorNombre();
        }
        else if(opcion==3){
            seguir = 0;
        }
        else{
            printf("Opcion invalida.\n");
        }
    }
    return 1;
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
 * limpiarEntrada: Deja el teclado listo para la siguiente lectura.
 * Se llama cuando scanf no logra leer un numero: descarta todo lo que
 * quede en el buffer hasta el salto de linea.
 * Retorna 1 si quedo algo por leer, 0 si se acabo la entrada (EOF).
 */
int limpiarEntrada(){
    int caracter = getchar();
    while(caracter!='\n' && caracter!=EOF){
        caracter = getchar();
    }
    if(caracter==EOF){
        return 0;
    }
    return 1;
}

/**
 * turnoDia: Maneja el turno del dia (jugador realiza transferencias).
 * Valida cada lectura del teclado para no quedarse en un bucle infinito
 * si se escribe algo que no es un numero.
 * Retorna 1 si el dia sigue normal y 0 si se acabo la entrada.
 */
int turnoDia(struct comuna* lista){
    if(lista==NULL){
        return 1;
    }
    int turnos = calcularTurnosPorDia(contarComunas(lista));
    printf("\nTienes %d turnos para realizar transferencias.\n", turnos);
    while(turnos>0){
        mostrarResumen(lista);
        mostrarMenu();
        int opcion;
        if(scanf("%d", &opcion)!=1){
            if(limpiarEntrada()==0){
                printf("\nSe acabo la entrada, se termina el dia.\n");
                return 0;
            }
            printf("Entrada invalida, escribe un numero.\n");
            continue;
        }
        if(opcion==1){
            mostrarResumen(lista);
        }
        else if(opcion==2){
            printf("Selecciona comuna origen: ");
            int idxOrigen;
            if(scanf("%d", &idxOrigen)!=1){
                if(limpiarEntrada()==0){
                    printf("\nSe acabo la entrada, se termina el dia.\n");
                    return 0;
                }
                printf("Entrada invalida, escribe un numero.\n");
                continue;
            }
            printf("Selecciona comuna destino: ");
            int idxDestino;
            if(scanf("%d", &idxDestino)!=1){
                if(limpiarEntrada()==0){
                    printf("\nSe acabo la entrada, se termina el dia.\n");
                    return 0;
                }
                printf("Entrada invalida, escribe un numero.\n");
                continue;
            }
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
            if(scanf("%d", &idxBien)!=1){
                if(limpiarEntrada()==0){
                    printf("\nSe acabo la entrada, se termina el dia.\n");
                    return 0;
                }
                printf("Entrada invalida, escribe un numero.\n");
                continue;
            }
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
            if(scanf("%d", &cantidad)!=1){
                if(limpiarEntrada()==0){
                    printf("\nSe acabo la entrada, se termina el dia.\n");
                    return 0;
                }
                printf("Entrada invalida, escribe un numero.\n");
                continue;
            }
            int exito = realizarTransferencia(origen, destino, nombreBien, cantidad);
            if(exito==1){
                aumentarValores(origen, cantidad);
                aumentarValores(destino, cantidad);
                // Los indices cambian al instante: la reciprocidad y la necesidad
                // dependen de los regalos y de las comunas vecinas
                recalcularIndices(lista);
                turnos = turnos - 1;
            }
        }
        else if(opcion==3){
            printf("Selecciona comuna: ");
            int idx;
            if(scanf("%d", &idx)!=1){
                if(limpiarEntrada()==0){
                    printf("\nSe acabo la entrada, se termina el dia.\n");
                    return 0;
                }
                printf("Entrada invalida, escribe un numero.\n");
                continue;
            }
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
            return 1;
        }
        else if(opcion==5){
            if(irALaBiblioteca(lista)==0){
                printf("\nSe acabo la entrada, se termina el dia.\n");
                return 0;
            }
        }
        else{
            printf("Opcion invalida.\n");
        }
    }
    return 1;
}

/**
 * turnoNoche: Maneja el turno de la noche (consumo, emergencias, disoluciones).
 * Libera al inicio el bloqueo de intercambios de la noche anterior, para que
 * una emergencia solo detenga los intercambios del dia siguiente.
 * Si hubo feriado, la noche que viene no hay consumo de bienes.
 */
void turnoNoche(struct comuna** lista){
    printf("\n");
    printf("########################################\n");
    printf("#  NOCHE\n");
    printf("########################################\n");
    estado.transferencias_bloqueadas = 0;
    if(estado.consumo_pausado==1){
        printf("Feriado: no hay consumo de bienes esta noche.\n");
        estado.consumo_pausado = 0;
    }
    else{
        aplicarConsumoNocturno(*lista);
    }
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
 * El origen queda con mas regalos dados y el destino con mas regalos recibidos,
 * que es lo que alimenta el indice de reciprocidad.
 * Retorna 1 si fue exitosa, 0 si fallo.
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
            origen->regalosDados = origen->regalosDados + cantidad;
            destino->regalosRecibidos = destino->regalosRecibidos + cantidad;
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
 * Si la comuna era del jugador se le asigna otra comuna autonoma en su lugar.
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

    // Si era comuna del jugador se le asigna otra en su lugar.
    // Primero se miran las vecinas autonomas y despues toda la lista.
    int eraTuya = comuna->administrada;
    struct comuna* reemplazo = NULL;
    if(eraTuya==1){
        if(vecinaIzquierda!=comuna && vecinaIzquierda->administrada==0){
            reemplazo = vecinaIzquierda;
        }
        else if(vecinaDerecha!=comuna && vecinaDerecha->administrada==0){
            reemplazo = vecinaDerecha;
        }
        else{
            struct comuna* busqueda = *lista;
            if(busqueda!=NULL){
                do{
                    if(busqueda!=comuna && busqueda->administrada==0 && reemplazo==NULL){
                        reemplazo = busqueda;
                    }
                    busqueda = busqueda->siguiente;
                }while(busqueda!=*lista);
            }
        }
        if(reemplazo!=NULL){
            reemplazo->administrada = 1;
        }
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
    if(eraTuya==1){
        int restantes = contarAdministradas(*lista) - 1;
        printf("#  Era una comuna de tu gestion\n");
        if(reemplazo!=NULL){
            printf("#  Se te asigna la comuna %s\n", reemplazo->nombre);
        }
        else{
            printf("#  No hay comuna disponible para reemplazarla\n");
        }
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
 * Se disuelve si la necesidad final es muy alta o la satisfaccion es muy baja.
 * La necesidad final ya trae lo de las comunas vecinas y la reciprocidad.
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
