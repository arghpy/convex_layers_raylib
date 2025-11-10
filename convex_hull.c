#include <string.h>
#include <math.h>
#include <raylib.h>
#include <utils.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define FPS 60

#define BACKGROUND_COLOR ((Color) {24, 24, 24, 255}) // 0x18_18_18_FF

#define LINE_THICKNESS 5
#define CIRCLE_RADIUS 10

typedef ut_da_declare(Vector2) Points;

Points points = {0};
Points points_to_delete = {0};

Vector2 get_min_x(Points *points)
{
  Vector2 p = points->items[0];
  if (points->count > 0) {
    for (size_t i = 0; i < points->count; i++) {
      if (points->items[i].x < p.x){
        p.x = points->items[i].x;
        p.y = points->items[i].y;
      }
    }
  }
  return p;
}

Vector2 get_max_x(Points *points)
{
  Vector2 p = points->items[0];
  if (points->count > 0) {
    for (size_t i = 0; i < points->count; i++) {
      if (points->items[i].x > p.x){
        p.x = points->items[i].x;
        p.y = points->items[i].y;
      }
    }
  }
  return p;
}

bool is_vector_on_side_of_line(Vector2 p1, Vector2 p2, Vector2 p)
{
  int d = (p.x - p1.x) * (p2.y - p1.y) - (p.y - p1.y) * (p2.x - p1.x);

  // 0 -> on the line, otherwise on one side
  if (d > 0) {
    return true;
  }
  return false;
}

float distance_to_line(Vector2 p1, Vector2 p2, Vector2 p)
{
  float d = fabs((p2.y - p1.y) * p.x - (p2.x - p1.x) * p.y + p2.x * p1.y - p2.y * p1.x) / sqrt(pow(p2.y - p1.y, 2) + pow(p2.x - p1.x, 2));
  return d;
}

Vector2 most_distant_vector_to_line(Points *points, Vector2 min, Vector2 max)
{
  Vector2 v = points->items[0];
  float d1 = distance_to_line(min, max, points->items[0]);

  for (size_t i = 1; i < points->count; i++) {
    float d2 = distance_to_line(min, max, points->items[i]);
    if (d1 < d2) {
      d1 = d2;
      v = points->items[i];
    }
  }
  return v;
}

void vectors_on_side_of_line(Points *points, Points *side_points, Vector2 min, Vector2 max)
{
  for (size_t i = 0; i < points->count; i++) {
    if (is_vector_on_side_of_line(min, max, points->items[i]))
      ut_da_push(side_points, points->items[i]);
  }
}

void solve_hull(Points *points, Vector2 min_x, Vector2 max_x)
{
  Points side_points = {0};
  vectors_on_side_of_line(points, &side_points, min_x, max_x);

  if (side_points.count == 0) {
    DrawLineEx(min_x, max_x, LINE_THICKNESS, LIGHTGRAY);
    ut_da_push(&points_to_delete, min_x);
  } else {
    Vector2 p_most_distant = most_distant_vector_to_line(&side_points, min_x, max_x);

    solve_hull(&side_points, min_x, p_most_distant);
    solve_hull(&side_points, p_most_distant, max_x);
  }
  ut_da_free(&side_points);
}

bool vectors_are_equal(Vector2 v1, Vector2 v2)
{
  return v1.x == v2.x && v1.y == v2.y;
}

void delete_vector_from_array(Points *points, Vector2 v)
{
  size_t i = 0;
  while (! vectors_are_equal(points->items[i], v)) i++;
  ut_da_pop(points, i);
}

void draw_hull(Points *points)
{
  // Copy original array
  Points temp_points = {0};
  ut_da_copy(&temp_points, points);

  Vector2 min_x = get_min_x(&temp_points);
  Vector2 max_x = get_max_x(&temp_points);

  solve_hull(&temp_points, min_x, max_x);
  solve_hull(&temp_points, max_x, min_x);

  for (size_t i = 0; i < points_to_delete.count; i++)
    delete_vector_from_array(&temp_points, points_to_delete.items[i]);

  ut_da_reset(&points_to_delete);

  if (temp_points.count >= 2) {
    draw_hull(&temp_points);
  }
  ut_da_free(&temp_points);
}

int main(void)
{

  int font_size = 20;

  Rectangle button_compute = {
    .x = 0,
    .y = SCREEN_HEIGHT - font_size * 2,
    .width = 180,
    .height = font_size * 2
  };
  char *button_compute_text = "Compute layers";
  bool button_compute_pressed = false;
  Color button_compute_color = LIGHTGRAY;

  Rectangle button_reset = {
    .x = SCREEN_WIDTH - 75,
    .y = SCREEN_HEIGHT - font_size * 2,
    .width = 75,
    .height = font_size * 2
  };
  char *button_reset_text = "Reset";

  SetTraceLogLevel(LOG_WARNING);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Convex Layers");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    // Change color of button
    button_compute_color = button_compute_pressed ? RED : LIGHTGRAY;

    // Add points
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), button_compute) && !CheckCollisionPointRec(GetMousePosition(), button_reset))
        ut_da_push(&points, GetMousePosition());

    // Reset button action
    if (CheckCollisionPointRec(GetMousePosition(), button_reset) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && points.count > 0) {
      ut_da_reset(&points);
      button_compute_pressed = false;
    }

    BeginDrawing();
    {
      ClearBackground(BACKGROUND_COLOR);

      // Draw button_compute
      DrawRectangleRec(button_compute, button_compute_color);
      if (CheckCollisionPointRec(GetMousePosition(), button_compute) && points.count > 2) {
        DrawRectangleLinesEx(button_compute, 5.0, BLACK);
      }
      DrawText(button_compute_text, button_compute.x + font_size / 2, button_compute.y + font_size / 2, font_size, BLACK);

      // Draw button_reset
      DrawRectangleRec(button_reset, LIGHTGRAY);
      if (CheckCollisionPointRec(GetMousePosition(), button_reset) && points.count > 0) {
        DrawRectangleLinesEx(button_reset, 5.0, BLACK);
      }
      DrawText(button_reset_text, button_reset.x + font_size / 2, button_reset.y + font_size / 2, font_size, BLACK);

      // Draw points
      for(size_t i = 0; i < points.count; i++) {
        Color point_color = LIGHTGRAY;
        if(CheckCollisionPointCircle(GetMousePosition(), points.items[i], CIRCLE_RADIUS)) {
          if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            point_color = PURPLE;
            points.items[i] = GetMousePosition();
          }
        }
        DrawCircle(points.items[i].x, points.items[i].y, CIRCLE_RADIUS, point_color);
      }

      if (button_compute_pressed || (CheckCollisionPointRec(GetMousePosition(), button_compute) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && points.count > 2)) {
        draw_hull(&points);
        button_compute_pressed = true;
      }

    }
    EndDrawing();
  }

  CloseWindow();

  // Free dynamic arrays
  ut_da_free(&points);
  ut_da_free(&points_to_delete);
  return 0;
}
