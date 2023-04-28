#include "game.h"
#include <iostream>
#include "SDL.h"

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFoodPoisonDietpill();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();
  Uint32 frame_start;
  Uint32 frame_end;
  Uint32 frame_duration;
  int frame_count = 0;
  bool running = true;

  while (running) {
    frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    Update();
    renderer.Render(snake, food, poison, dietpill);

    frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    frame_count++;
    frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (frame_end - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = frame_end;
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::PlaceFoodPoisonDietpill() {
  int x, y, w, z, u, v;
  while (true) {
    // food
    x = random_w(engine);
    y = random_h(engine);

    // poison
    w = random_w(engine);
    z = random_h(engine);

    //diet pill
    u = random_w(engine);
    v = random_h(engine);
    // Check that the location is not occupied by a snake item before placing, and that locations of food, poison and diet pills are not overlapping
    if ((!snake.SnakeCell(x, y)) && (!snake.SnakeCell(w, z)) && (!snake.SnakeCell(u, v)) && ((x != w) || (y != z)) && ((x != u) || (y != v)) && ((u != w) || (v != z))) {
      food.x = x;
      food.y = y;

      poison.x = w;
      poison.y = z;

      dietpill.x = u;
      dietpill.y = v;
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);
  // check if poison is taken
  if (poison.x == new_x && poison.y == new_y) {
  	snake.alive = false;
    return;
  }
  // Check if there's food or diet pill over here
  if (food.x == new_x && food.y == new_y) {
    score++;
    PlaceFoodPoisonDietpill();
    // Grow snake and increase speed.
    snake.GrowBody();
    snake.speed += 0.002;
    return;
  }
  if (dietpill.x == new_x && dietpill.y == new_y) {
    // deduct score only when snake's body size is larger than 2
    if (snake.body.size() > 2) score--;
    PlaceFoodPoisonDietpill();
    snake.ShrinkBody();
    snake.speed -= 0.002;
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }
