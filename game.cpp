/**
 *
 * Przykładowy szkielet gry w języku C++.
 * 
 * Aktualnie robi on tylko ladny obrazek na ekranie
 *
 * */
#include <SDL.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <random>
#include <thread>
#include <tuple>
#include <vector>

using namespace std;

enum game_state { FINISH, IN_GAME };

struct sdl_objects_t {
  SDL_Window *window;
  SDL_Renderer *renderer;
};

class game_objects_c {
public:
  game_state next_game_state;
  sdl_objects_t sdl_objects;
  std::mt19937_64 random_engine; // dla liczb pseudolosowych

  int x, y;   // punkcik na ekranie, będzie się śmiesznie odbijał
  int dx, dy; // predkosci po wspolrzednych x oraz y
  int color;
  void input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        next_game_state = FINISH;
        break;
      }
    }
  }

  void physics(int dt_miliseconds) {
    std::uniform_int_distribution<int> uniform_dist(-1, 1);
    x = x + dx + uniform_dist(random_engine);
    y = y + dy + uniform_dist(random_engine);
    if (x > 320)
      dx = (dx > 0) ? -dx : dx;
    if (y > 240)
      dy = (dy > 0) ? -dy : dy;
    if (x < 0)
      dx = (dx > 0) ? dx : -dx;
    if (y < 0)
      dy = (dy > 0) ? dy : -dy;
  }

  void video() {
    auto renderer = sdl_objects.renderer;
    // SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
    // SDL_RenderClear( renderer );
    SDL_SetRenderDrawColor(renderer, 23, (sin((double)x/10.0)*127+128), (sin((double)y/10.0)*127+128), 51);
    SDL_RenderDrawPoint(renderer, x, y);
    SDL_RenderPresent(renderer);
  }

  // konstruktor - taka funkcja ktora jest uruchamiana przy tworzeniu obiektu
  // typu game_objects_c
  game_objects_c(sdl_objects_t sdl_objects_,
                 game_state next_game_state_ = IN_GAME)
      : random_engine([]() { // inicjalizacja generatora liczb pseudolosowych
          static std::random_device r;
          return r();
        }()) {
    next_game_state = next_game_state_;
    sdl_objects = sdl_objects_;
    x = 10;
    y = 100;
    dx = 2;
    dy = 0;
  }
};

int main(int argc, char *args[]) {
  using namespace std::literals::chrono_literals;
  // przygotowujemy okienko SDL
  sdl_objects_t sdl_objects;
  SDL_Init(SDL_INIT_EVERYTHING);
  sdl_objects.window = SDL_CreateWindow("Gra nasza", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320*3, 240*3, SDL_WINDOW_SHOWN);
  sdl_objects.renderer = SDL_CreateRenderer(sdl_objects.window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetScale(sdl_objects.renderer, 10,10);

  // przygotowujemy obiekt gry
  game_objects_c game_objects(sdl_objects);

  // pętla gry
  auto next_frame_time = std::chrono::high_resolution_clock::now();
  while (game_objects.next_game_state != FINISH) {
    game_objects.input();
    game_objects.physics(10);
    game_objects.video();
    next_frame_time = next_frame_time + 10ms;
    std::this_thread::sleep_until(next_frame_time);
  }

  // czyścimy wszystko na koniec
  SDL_DestroyRenderer(sdl_objects.renderer);
  SDL_DestroyWindow(sdl_objects.window);
  SDL_Quit();
  return 0;
}
