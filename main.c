#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

// Sta³e okreœlaj¹ce parametry okna
#define WINDOW_TITLE "super kartka 3000"
#define WINDOW_WIDTH 750
#define WINDOW_HEIGHT 550
#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // 0. Konfiguracja kodowania UTF-8 dla konsoli w systemie Windows (polskie znaki)
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif

    // Poprawne skalowanie na ekranach High-DPI (musi byæ ustawione PRZED SDL_Init)
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

    // 1. Inicjalizacja podsystemu wideo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "B³¹d inicjalizacji SDL: %s\n", SDL_GetError());
        return 1;
    }

    // 2. Utworzenie okna
    SDL_Window* window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI
    );

    if (!window) {
        fprintf(stderr, "B³¹d tworzenia okna: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // 3. Utworzenie renderera (akceleracja sprzêtowa + DX9 + VSync)
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d");
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    // Fallback 1: Domyœlny sterownik sprzêtowy (np. Direct3D 11 lub OpenGL)
    if (!renderer) {
        fprintf(stderr, "Ostrze¿enie: Nie uda³o siê uruchomiæ DX9 (%s). Próba u¿ycia domyœlnego sterownika...\n", SDL_GetError());
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "");
        renderer = SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );
    }

    // Fallback 2: Ostatecznoœæ — renderer programowy (CPU) bez VSync
    if (!renderer) {
        fprintf(stderr, "Ostrze¿enie: Brak akceleracji sprzêtowej. Prze³¹czanie na tryb programowy.\n");
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (!renderer) {
        fprintf(stderr, "Krytyczny b³¹d tworzenia renderera: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Ustawienie logicznej rozdzielczoœci (rozwi¹zuje problemy ze skalowaniem DPI)
    SDL_RenderSetLogicalSize(renderer, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Weryfikacja aktywnego sterownika oraz rzeczywistego stanu VSync w konsoli
    bool hasVSync = false;
    SDL_RendererInfo info;
    if (SDL_GetRendererInfo(renderer, &info) == 0) {
        hasVSync = (info.flags & SDL_RENDERER_PRESENTVSYNC) != 0;
        printf("Aktywny sterownik graficzny: %s (VSync: %s)\n", info.name, hasVSync ? "TAK" : "NIE");
    }

    // 4. G³ówna pêtla aplikacji
    bool isRunning = true;
    SDL_Event event;
    Uint64 lastTime = SDL_GetTicks64();

    while (isRunning) {
        Uint64 frameStart = SDL_GetTicks64();
        float deltaTime = (frameStart - lastTime) / 1000.0f;
        lastTime = frameStart;
        (void)deltaTime;

        // Obs³uga zdarzeñ
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            else if (event.type == SDL_RENDER_TARGETS_RESET || event.type == SDL_RENDER_DEVICE_RESET) {
                // Miejsce na prze³adowanie tekstur typu SDL_TEXTUREACCESS_TARGET
            }
        }

        // --- LOGIKA GRY / APLIKACJI ---


        // --- RENDEROWANIE ---
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255);
        SDL_RenderClear(renderer);

        // Tutaj rysujesz swoje obiekty na wirtualnej siatce 750x550...

        SDL_RenderPresent(renderer);

        // Zabezpieczenie CPU: Jeœli brak VSync (tryb programowy lub wymuszony brak VSync w sterowniku)
        if (!hasVSync) {
            Uint64 frameTime = SDL_GetTicks64() - frameStart;
            if (frameTime < FRAME_DELAY) {
                SDL_Delay((Uint32)(FRAME_DELAY - frameTime));
            }
        }
    }

    // 5. Sprz¹tanie i zwalnianie zasobów
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
