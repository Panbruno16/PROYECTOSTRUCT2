#ifndef SOCIEDAD_H
#define SOCIEDAD_H

/**
 * Estructura global del juego.
 * Contiene el estado general de la sociedad.
 */
struct juego{
    int personas_fuera;
    int dia_actual;
    int turnos_disponibles;
    int record_dias;
    int transferencias_bloqueadas;
    float pobreza_global;
};

extern struct juego estado;

//Moldes

/**
 * Estructura que representa un bien en la sociedad.
 */
struct bien {
    char nombre[50];
    int maximaCantidad;
    int actualCantidad;
    struct bien* siguiente;
};

/**
 * Estructura que representa un oficio que una persona sabe.
 * Es conocimiento, no un recurso, por eso solo guarda el nombre y el siguiente.
 */
struct servicio {
    char nombre[50];
    struct servicio* siguiente;
};

/**
 * Estructura que representa una persona en la sociedad.
 * Lleva su propia lista de servicios (los oficios que sabe).
 * Si la lista es NULL significa que no sabe hacer nada.
 */
struct persona {
    char nombre[50];
    struct servicio* servicios;
    struct persona* siguiente;
};

/**
 * Estructura que representa una comuna en la sociedad.
 * Tiene sus bienes, sus habitantes y los indices de la sociedad.
 * El indice de reciprocidad mide el balance entre regalos dados y recibidos.
 */
struct comuna {
    char nombre[50];
    struct bien* bienes;
    struct persona* habitantes;
    float necesidad;
    float satisfaccion;
    float pobreza;
    float reciprocidad;
    struct comuna* siguiente;
    struct comuna* anterior;
};

// Funciones
/**
 * Crea una nueva comuna con el nombre especificado.
 */
struct comuna* crearComuna(char* nombre);

/**
 * Libera la memoria de una comuna.
 */
void liberarComuna(struct comuna* comuna);

/**
 * liberarListaComunas: Libera toda la lista de comunas.
 */
void liberarListaComunas(struct comuna* lista);

/**
 * contarPersonas: Cuenta cuantas personas hay en una comuna.
 * Recorre la lista de habitantes y retorna el conteo total.
 */
int contarPersonas(struct comuna* comuna);

/**
 * contarBienes: Cuenta cuantos bienes hay en una comuna.
 * Recorre la lista de bienes y retorna el conteo total.
 */
int contarBienes(struct comuna* comuna);

/**
 * contarServicios: Cuenta cuantos oficios tienen las personas de una comuna.
 * Recorre los habitantes y suma los servicios que sabe cada uno.
 */
int contarServicios(struct comuna* comuna);

/**
 * mostrarBienes: Muestra en pantalla los bienes de una comuna.
 * Recorre la lista de bienes e imprime el nombre y las cantidades de cada uno.
 */
void mostrarBienes(struct comuna* comuna);

/**
 * mostrarServicios: Muestra en pantalla los oficios de cada persona de una comuna.
 * Recorre los habitantes e imprime los servicios que sabe cada persona.
 * Incluye un resumen con el porcentaje de personas que al menos sabe un oficio.
 */
void mostrarServicios(struct comuna* comuna);

/**
 * mostrarPersonas: Muestra en pantalla los habitantes de una comuna.
 * Recorre la lista de habitantes e imprime el nombre de cada persona.
 */
void mostrarPersonas(struct comuna* comuna);

/**
 * agregarPersonas: Agrega varias personas a una comuna desde un archivo.
 * Lee nombres del archivo y los agrega a la lista de habitantes.
 * Retorna el numero de personas agregadas.
 */
int agregarPersonas(struct comuna* comuna, char* nombreArchivo);

/**
 * agregarBienes: Agrega varios bienes a una comuna desde un archivo.
 * Lee nombres del archivo y los agrega a la lista de bienes.
 * Retorna el numero de bienes agregados.
 */
int agregarBienes(struct comuna* comuna, char* nombreArchivo);

/**
 * agregarServicioAPersona: Agrega un oficio a la lista de servicios de una persona.
 * Inserta el servicio al inicio de la lista enlazada de la persona.
 */
void agregarServicioAPersona(struct persona* persona, char* nombre);

/**
 * migrarPersona: Mueve una persona de una comuna a otra llevandose sus oficios.
 * Es una funcion auxiliar interna para la logica de disolucion de comunas.
 * Retorna 1 si la persona encontro destino, 0 si fallo.
 */
int migrarPersona(struct comuna* origen, struct comuna* destino, char* nombre);

/**
 * crearComunas: Crea varias comunas y las conecta en lista circular doble.
 * Lee nombres del archivo y crea una comuna por cada nombre.
 * Recibe la cantidad de comunas, oficios del catalogo y bienes.
 * Las 500 personas se reparten aleatoriamente entre las comunas.
 * Cada persona tiene un 30% de probabilidad de recibir un oficio al azar.
 * Retorna la lista de comunas creadas.
 */
struct comuna* crearComunas(char* nombreArchivo, int cantidad, int numServicios, int numBienes);

/**
 * agregarComuna: Agrega una comuna a la lista circular doble.
 * Inserta la comuna al final de la lista y conecta los punteros.
 */
void agregarComuna(struct comuna** lista, char* nombre);

/**
 * contarComunas: Cuenta cuantas comunas hay en la lista.
 * Recorre la lista circular y retorna el conteo total.
 */
int contarComunas(struct comuna* lista);

/**
 * inicializarSociedad: Inicializa todas las comunas con personas, bienes y servicios.
 * Carga los datos desde los archivos y los asigna a cada comuna.
 */
void inicializarSociedad(struct comuna* lista);

/**
 * calcularNecesidad: Calcula el indice de necesidad de una comuna.
 * Recorre los bienes y calcula el promedio de (actual/maxima) de llenado.
 * La necesidad es 100 menos ese llenado: alto = le falta recursos,
 * bajo = tiene recursos. Asi la comuna se disuelve cuando la necesidad es muy alta.
 */
float calcularNecesidad(struct comuna* comuna);

/**
 * calcularSatisfaccion: Calcula el indice de satisfaccion de una comuna.
 * Considera el llenado de bienes y el porcentaje de personas con oficio.
 * Mas recursos y mas gente con oficio = mas satisfecha.
 */
float calcularSatisfaccion(struct comuna* comuna);

/**
 * recalcularIndices: Recalcula los indices de todas las comunas.
 */
void recalcularIndices(struct comuna* lista);

/**
 * aplicarConsumoNocturno: Reduce los bienes de todas las comunas.
 * Cada persona consume 0.1 bienes por dia (minimo 7, maximo 10).
 */
void aplicarConsumoNocturno(struct comuna* lista);

/**
 * calcularPobrezaGlobal: Calcula la pobreza global del juego.
 * Se calcula como: (personas_fuera / 500) * 100.
 */
float calcularPobrezaGlobal();

/**
 * actualizarPobreza: Actualiza la pobreza global y asigna el valor a todas las comunas.
 */
void actualizarPobreza(struct comuna* lista);

/**
 * verificarEmergencias: Verifica y aplica emergencias aleatorias.
 * Huelga: probabilidad base 10% + pobreza * 0.3%
 * Vandalos: probabilidad base 5% + pobreza * 0.4%
 */
void verificarEmergencias(struct comuna* lista);

/**
 * realizarTransferencia: Transfiere un bien de una comuna a otra vecina.
 * Verifica que sean vecinas, que haya suficiente cantidad y descuenta del origen.
 * Retorna 1 si fue exitosa, 0 si fallo.
 */
int realizarTransferencia(struct comuna* origen, struct comuna* destino, char* bien, int cantidad);

/**
 * disolverComuna: Disuelve una comuna de la sociedad.
 * Parte de sus personas migra a comunas vecinas con sus oficios.
 * La minoria se queda sin hogar y sube la pobreza global.
 * Los bienes de la comuna se pierden. La quita de la lista circular.
 */
void disolverComuna(struct comuna** lista, struct comuna* comuna);

/**
 * verificarDisoluciones: Verifica si alguna comuna debe disolverse.
 * Se disuelve si la necesidad es muy alta o la satisfaccion es muy baja.
 * Sus personas se reparten entre migracion y pobreza.
 * Si la lista queda vacia se vuelve NULL.
 */
void verificarDisoluciones(struct comuna** lista);

/**
 * calcularTurnosPorDia: Calcula los turnos disponibles por dia.
 * Se calcula como: totalComunas / 10, con minimo 2 y maximo 10.
 */
int calcularTurnosPorDia(int totalComunas);

/**
 * guardarRecord: Guarda el record de dias en un archivo.
 */
void guardarRecord();

/**
 * cargarRecord: Carga el record de dias desde un archivo.
 */
void cargarRecord();

/**
 * aumentarValores: Aumenta los bienes y servicios de una comuna por transferencia.
 * El aumento es proporcional a la cantidad transferida.
 * Bienes: cantidad * 0.1, Servicios: cantidad * 0.15
 */
void aumentarValores(struct comuna* comuna, int cantidad);

/**
 * mostrarResumen: Muestra un resumen de todas las comunas.
 * Muestra nombre, indices y cantidad de personas de cada comuna.
 */
void mostrarResumen(struct comuna* lista);

/**
 * mostrarMenu: Muestra el menu de opciones del jugador.
 */
void mostrarMenu();

/**
 * turnoDia: Maneja el turno del dia (jugador realiza transferencias).
 */
void turnoDia(struct comuna* lista);

/**
 * turnoNoche: Maneja el turno de la noche (consumo, emergencias, disoluciones).
 */
void turnoNoche(struct comuna** lista);

/**
 * Agrega un nuevo bien a la comuna.
 */
void agregarBien(struct comuna* comuna, char* nombre, int maximaCantidad, int actualCantidad);

/**
 * Agrega una nueva persona a la comuna.
 */
void agregarPersona(struct comuna* comuna, char* nombre);

/**
 * Agrega los índices a la comuna.
 */
void agregarindices(struct comuna* comuna, float necesidad, float satisfaccion, float pobreza);

/**
 * Muestra la información de la comuna.
 */
void mostrarComuna(struct comuna* comuna);
/**
 * Muestra los bienes de la comuna.
 */
void mostrarBienes(struct comuna* comuna);

/**
 * Muestra los servicios de la comuna.
 */
void mostrarServicios(struct comuna* comuna);

/**
 * Muestra los habitantes de la comuna.
 */
void mostrarHabitantes(struct comuna* comuna);

/**
 * Muestra los índices de la comuna.
 */
void mostrarIndices(struct comuna* comuna);
















#endif 