#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "./constants.h"

// Variables globales para el juego.
int juego_corre = false;
int Ultimo_frame = 0;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int puntaje_jugador1 = 0;
int puntaje_jugador2 = 0;

// Estructura para objetos del juego.
struct game_object
{
    float x;
    float y;
    float ancho;
    float alto;
    float vel_x;
    float vel_y;
} bola, rectangulo, rectangulo2, cancha_izquierda, cancha_derecha;

TTF_Font* font = NULL;

// Inicialización de la ventana y SDL.
int initialize_window(void) {
    // Inicializar SDL.
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error al inicializar SDL: %s\n", SDL_GetError());
        return false;
    }

    // Inicializar SDL_ttf para manejar fuentes de texto.
    if (TTF_Init() == -1) {
        fprintf(stderr, "Error al inicializar SDL_ttf: %s\n", TTF_GetError());
        return false;
    }

    // Crear ventana del juego.
    window = SDL_CreateWindow(
        "hokey by jero",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );
    if (!window) {
        fprintf(stderr, "Error al crear la ventana: %s\n", SDL_GetError());
        return false;
    }

    // Crear renderizador.
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "Error al crear el renderizador: %s\n", SDL_GetError());
        return false;
    }

    // Cargar la fuente.
    font = TTF_OpenFont("VINERITC.TTF", 24); // Ajusta el nombre de la fuente según tu archivo.
    if (!font) {
        fprintf(stderr, "Error al cargar la fuente: %s\n", TTF_GetError());
        return false;
    }

    return true;
}

// Procesamiento de entrada del usuario.
void process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            juego_corre = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                juego_corre = false;
            break;
        }
    }

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    // Control del primer rectángulo con las flechas.
    if (keystate[SDL_SCANCODE_W]) {
        rectangulo2.y -= 0.1;
        if (rectangulo2.y < 0) rectangulo2.y = 0;
    }
    if (keystate[SDL_SCANCODE_S]) {
        rectangulo2.y += 0.1;
        if (rectangulo2.y + rectangulo2.alto > WINDOW_HEIGHT)
            rectangulo2.y = WINDOW_HEIGHT - rectangulo2.alto;
    }
    if (keystate[SDL_SCANCODE_A]) {
        rectangulo2.x -= 0.1;
        if (rectangulo2.x < 0) rectangulo2.x = 0;
        if (rectangulo2.x + rectangulo2.ancho > WINDOW_WIDTH / 2)
            rectangulo2.x = WINDOW_WIDTH / 2 - rectangulo2.ancho - 20;
    }
    if (keystate[SDL_SCANCODE_D]) {
        rectangulo2.x += 0.1;
        if (rectangulo2.x + rectangulo2.ancho > WINDOW_WIDTH / 2)
            rectangulo2.x = WINDOW_WIDTH / 2 - rectangulo2.ancho;
    }

    // Control del segundo rectángulo con WASD.
    if (keystate[SDL_SCANCODE_UP]) {
        rectangulo.y -= 0.1;
        if (rectangulo.y < 0) rectangulo.y = 0;
    }
    if (keystate[SDL_SCANCODE_DOWN]) {
        rectangulo.y += 0.1;
        if (rectangulo.y + rectangulo.alto > WINDOW_HEIGHT)
            rectangulo.y = WINDOW_HEIGHT - rectangulo.alto;
    }
    if (keystate[SDL_SCANCODE_LEFT]) {
        rectangulo.x -= 0.1;
        if (rectangulo.x < WINDOW_WIDTH / 2) rectangulo.x = WINDOW_WIDTH / 2;
    }
    if (keystate[SDL_SCANCODE_RIGHT]) {
        rectangulo.x += 0.1;
        if (rectangulo.x + rectangulo.ancho > WINDOW_WIDTH)
            rectangulo.x = WINDOW_WIDTH - rectangulo.ancho + 20;
    }
}

// Configuración inicial del juego.
void setup(void) {
    bola.x = 10;
    bola.y = 20;
    bola.ancho = 20;
    bola.alto = 20;
    bola.vel_x = 380;
    bola.vel_y = 340;

    rectangulo2.x = 40;
    rectangulo2.y = 300;
    rectangulo2.alto = 100;
    rectangulo2.ancho = 20;

    rectangulo.x = 730;
    rectangulo.y = 300;
    rectangulo.alto = 100;
    rectangulo.ancho = 20;

    // Configuración de las canchas.
    int ancho_cancha = 30;
    int altura_cancha = 200;
    cancha_izquierda.x = 0;
    cancha_izquierda.y = (WINDOW_HEIGHT - altura_cancha) / 2;
    cancha_izquierda.ancho = ancho_cancha;
    cancha_izquierda.alto = altura_cancha;

    cancha_derecha.x = WINDOW_WIDTH - ancho_cancha;
    cancha_derecha.y = (WINDOW_HEIGHT - altura_cancha) / 2;
    cancha_derecha.ancho = ancho_cancha;
    cancha_derecha.alto = altura_cancha;
}

// Actualización de la lógica del juego.
void update(void) {
    float delta_time = (SDL_GetTicks() - Ultimo_frame) / 1000.0;
    Ultimo_frame = SDL_GetTicks();

    bola.x += bola.vel_x * delta_time;
    bola.y += bola.vel_y * delta_time;

    // Colisiones con las canchas.
    if (bola.x < cancha_izquierda.x + cancha_izquierda.ancho && // Si la bola toca el pedacito de cancha.
        bola.y + bola.alto > cancha_izquierda.y &&
        bola.y < cancha_izquierda.y + cancha_izquierda.alto) {
        // Jugador de la derecha gana.
        puntaje_jugador2++;
        printf("¡Gol del jugador 2! Puntaje: %d\n", puntaje_jugador2);
        bola.x = WINDOW_WIDTH / 2 - bola.ancho / 2;
        bola.y = WINDOW_HEIGHT / 2 - bola.alto / 2;
    }
    if (bola.x + bola.ancho > cancha_derecha.x && // Si la bola toca el pedacito de cancha.
        bola.y + bola.alto > cancha_derecha.y &&
        bola.y < cancha_derecha.y + cancha_derecha.alto) {
        // Jugador de la izquierda gana.
        puntaje_jugador1++;
        printf("¡Gol del jugador 1! Puntaje: %d\n", puntaje_jugador1);
        bola.x = WINDOW_WIDTH / 2 - bola.ancho / 2;
        bola.y = WINDOW_HEIGHT / 2 - bola.alto / 2;
    }

    // Colisión con rectángulo 1
    if (bola.x + bola.ancho > rectangulo.x && bola.x < rectangulo.x + rectangulo.ancho &&
        bola.y + bola.alto > rectangulo.y && bola.y < rectangulo.y + rectangulo.alto) {

        // Ajustar la posición de la bola para evitar que quede atrapada dentro del rectángulo.
        if (bola.vel_x > 0) {
            bola.x = rectangulo.x - bola.ancho;
        }
        else {
            bola.x = rectangulo.x + rectangulo.ancho;
        }

        // Cambiar la dirección de la bola dependiendo de la posición de colisión y la velocidad del rectángulo.
        if (bola.vel_x * rectangulo.vel_x >= 0) {
            bola.vel_x = -bola.vel_x;
        }
        if (bola.vel_y * rectangulo.vel_y >= 0) {
            bola.vel_y = +bola.vel_y;
        }
    }

    // Colisión con rectángulo 2
    if (bola.x + bola.ancho > rectangulo2.x && bola.x < rectangulo2.x + rectangulo2.ancho &&
        bola.y + bola.alto > rectangulo2.y && bola.y < rectangulo2.y + rectangulo2.alto) {

        // Ajustar la posición de la bola para evitar que quede atrapada dentro del rectángulo.
        if (bola.vel_x > 0) {
            bola.x = rectangulo2.x - bola.ancho;
        }
        else {
            bola.x = rectangulo2.x + rectangulo2.ancho;
        }

        // Cambiar la dirección de la bola dependiendo de la posición de colisión y la velocidad del rectángulo.
        if (bola.vel_x * rectangulo2.vel_x >= 0) {
            bola.vel_x = -bola.vel_x;
        }
        if (bola.vel_y * rectangulo2.vel_y >= 0) {
            bola.vel_y = +bola.vel_y;
        }
    }


    // Verificar si alguno de los jugadores ha llegado a 10 puntos para terminar el juego.
    if (puntaje_jugador1 >= 10 || puntaje_jugador2 >= 10) {
        juego_corre = false;
    }

    // Colisiones con los bordes de la ventana.
    if (bola.x < 0) {
        bola.x = 0;
        bola.vel_x = -bola.vel_x;
    }
    if (bola.x + bola.ancho > WINDOW_WIDTH) {
        bola.x = WINDOW_WIDTH - bola.ancho;
        bola.vel_x = -bola.vel_x;
    }
    if (bola.y < 0) {
        bola.y = 0;
        bola.vel_y = -bola.vel_y;
    }
    if (bola.y + bola.alto > WINDOW_HEIGHT) {
        bola.y = WINDOW_HEIGHT - bola.alto;
        bola.vel_y = -bola.vel_y;
    }
}

// Renderizado de los objetos del juego.
void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255); // Color verde oscuro para la cancha.
    SDL_RenderClear(renderer);

    // Renderizar la decoración del nivel.
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Color blanco para la decoración.
    SDL_Rect decoracion_rect1 = { 0, 0, WINDOW_WIDTH, 10 }; // Rectángulo superior.
    SDL_Rect decoracion_rect2 = { 0, WINDOW_HEIGHT - 10, WINDOW_WIDTH, 10 }; // Rectángulo inferior.
    SDL_RenderFillRect(renderer, &decoracion_rect1);
    SDL_RenderFillRect(renderer, &decoracion_rect2);

    // Renderizar las canchas.
    SDL_Rect cancha_izquierda_rect = { cancha_izquierda.x, cancha_izquierda.y, cancha_izquierda.ancho, cancha_izquierda.alto };
    SDL_Rect cancha_derecha_rect = { cancha_derecha.x, cancha_derecha.y, cancha_derecha.ancho, cancha_derecha.alto };
    SDL_RenderFillRect(renderer, &cancha_izquierda_rect);
    SDL_RenderFillRect(renderer, &cancha_derecha_rect);

    // Renderizar línea central de la cancha
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Color blanco para la línea central.
    SDL_Rect linea_central = { WINDOW_WIDTH / 2 - 5, 0, 10, WINDOW_HEIGHT }; // Línea central de 10 píxeles de ancho.
    SDL_RenderFillRect(renderer, &linea_central);

    // Renderizar los rectángulos y la bola.
    SDL_Rect rectangulo_rect = { rectangulo.x, rectangulo.y, rectangulo.ancho, rectangulo.alto };
    SDL_Rect rectangulo2_rect = { rectangulo2.x, rectangulo2.y, rectangulo2.ancho, rectangulo2.alto };
    SDL_Rect ball_rect = { (int)bola.x, (int)bola.y, (int)bola.ancho, (int)bola.alto };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rectangulo_rect);
    SDL_RenderFillRect(renderer, &rectangulo2_rect);
    SDL_RenderFillRect(renderer, &ball_rect);

    // Renderizar puntajes en pantalla.
    SDL_Color color = { 255, 255, 255 }; // Color blanco para el texto.
    char puntaje_str[32]; // String para almacenar el puntaje.
    sprintf_s(puntaje_str, sizeof(puntaje_str), "P1: %d   P2: %d", puntaje_jugador1, puntaje_jugador2); // Formatear el puntaje.
    SDL_Surface* surface = TTF_RenderText_Solid(font, puntaje_str, color); // Crear superficie de texto.
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); // Crear textura de la superficie.
    int text_width, text_height;
    SDL_QueryTexture(texture, NULL, NULL, &text_width, &text_height); // Obtener dimensiones del texto.
    SDL_Rect text_rect = { WINDOW_WIDTH / 2 - text_width / 2, 10, text_width, text_height }; // Posición del texto.
    SDL_RenderCopy(renderer, texture, NULL, &text_rect); // Renderizar texto en pantalla.
    SDL_DestroyTexture(texture); // Liberar textura.
    SDL_FreeSurface(surface); // Liberar superficie.

    SDL_RenderPresent(renderer);
}


// Destrucción de la ventana y liberación de recursos.
void destruir_w(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font); // Cerrar la fuente.
    TTF_Quit(); // Cerrar SDL_ttf.
    SDL_Quit();
}

// Función principal del programa.
int main(int argc, char* args[]) {
    juego_corre = initialize_window();
    setup();

    while (juego_corre) {
        process_input();
        update();
        render();
    }

    destruir_w();
    return 0;
}
