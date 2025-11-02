#include <string.h>
#include <math.h>
#include "raylib.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define FPS 60

#define BACKGROUND_COLOR ((Color) {24, 24, 24, 255}) // 0x18_18_18_FF

#define MAX_POINTS 100

#define LINE_THICKNESS 2.5
#define CIRCLE_RADIUS 5

Vector2 points_to_delete[MAX_POINTS] = {0};
int points_to_delete_count = 0;


Vector2 get_min_x(Vector2 *points, int nr_points)
{
  Vector2 p = points[0];
  if (nr_points > 0) {
    for (int i = 0; i < nr_points; i++) {
      if (points[i].x < p.x){
        p.x = points[i].x;
        p.y = points[i].y;
      }
    }
  }
  return p;
}

Vector2 get_max_x(Vector2 *points, int nr_points)
{
  Vector2 p = points[0];
  if (nr_points > 0) {
    for (int i = 0; i < nr_points; i++) {
      if (points[i].x > p.x){
        p.x = points[i].x;
        p.y = points[i].y;
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

Vector2 most_distant_vector_to_line(Vector2 *points, int n, Vector2 min, Vector2 max)
{
  Vector2 v = points[0];
  float d1 = distance_to_line(min, max, points[0]);

  for (int i = 1; i < n; i++) {
    float d2 = distance_to_line(min, max, points[i]);
    if (d1 < d2) {
      d1 = d2;
      v = points[i];
    }
  }
  return v;
}

void vectors_on_side_of_line(Vector2 *points, int count, Vector2 *side_points, int *side_points_count, Vector2 min, Vector2 max)
{
  for (int i = 0; i < count; i++) {
    if (is_vector_on_side_of_line(min, max, points[i])) side_points[(*side_points_count)++] = points[i];
  }
}

void solve_hull(Vector2 *points, int count, Vector2 min_x, Vector2 max_x)
{
  Vector2 side_points[MAX_POINTS] = {0};
  int side_points_count = 0;
  vectors_on_side_of_line(points, count, side_points, &side_points_count, min_x, max_x);

  if (side_points_count == 0) {
    DrawLineEx(min_x, max_x, LINE_THICKNESS, LIGHTGRAY);
    points_to_delete[points_to_delete_count++] = min_x;
  } else {
    Vector2 p_most_distant = most_distant_vector_to_line(side_points, side_points_count, min_x, max_x);

    solve_hull(side_points, side_points_count, min_x, p_most_distant);
    solve_hull(side_points, side_points_count, p_most_distant, max_x);
  }
}

bool vectors_are_equal(Vector2 v1, Vector2 v2)
{
  return v1.x == v2.x && v1.y == v2.y;
}

void delete_vector_from_array(Vector2 *points, int *count, Vector2 v)
{
  int i = 0;
  while (! vectors_are_equal(points[i], v)) i++;

  if (i != *count) {
    for (int k = i; k < *count - 1; k++) {
      points[k] = points[k + 1];
    }
  }
  (*count)--;
}

void draw_hull(Vector2 *points, int count)
{
  // Copy original array
  int temp_count = count;
  Vector2 temp_points[MAX_POINTS] = {0};
  for (int i = 0; i < temp_count; i++) {
    temp_points[i] = points[i];
  }

  Vector2 min_x = get_min_x(temp_points, temp_count);
  Vector2 max_x = get_max_x(temp_points, temp_count);

  solve_hull(temp_points, temp_count, min_x, max_x);
  solve_hull(temp_points, temp_count, max_x, min_x);

  for (int i = 0; i < points_to_delete_count; i++) {
    delete_vector_from_array(temp_points, &temp_count, points_to_delete[i]);
  }

  points_to_delete_count = 0;
  memset(points_to_delete, 0, sizeof(points_to_delete));

  if (temp_count >= 2) {
    draw_hull(temp_points, temp_count);
  }
}

int main(void)
{
  Vector2 points[MAX_POINTS] = {0};
  int points_count = 0;

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

  while (!WindowShouldClose()) {
    // Change color of button
    button_compute_color = button_compute_pressed ? RED : LIGHTGRAY;

    // Add points
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !CheckCollisionPointRec(GetMousePosition(), button_compute) && !CheckCollisionPointRec(GetMousePosition(), button_reset)) {
      if (points_count >= MAX_POINTS) {
        points_count = 0;
        memset(points, 0, sizeof(points));
      }
      if (points_count < MAX_POINTS) {
        points[points_count++] = GetMousePosition();
      }
    }

    // Reset button action
    if (CheckCollisionPointRec(GetMousePosition(), button_reset) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && points_count > 0) {
      points_count = 0;
      memset(points, 0, sizeof(points));
      button_compute_pressed = false;
    }

    BeginDrawing();
    {
      ClearBackground(BACKGROUND_COLOR);

      // Draw button_compute
      DrawRectangleRec(button_compute, button_compute_color);
      if (CheckCollisionPointRec(GetMousePosition(), button_compute) && points_count > 2) {
        DrawRectangleLinesEx(button_compute, 5.0, BLACK);
      }
      DrawText(button_compute_text, button_compute.x + font_size / 2, button_compute.y + font_size / 2, font_size, BLACK);

      // Draw button_reset
      DrawRectangleRec(button_reset, LIGHTGRAY);
      if (CheckCollisionPointRec(GetMousePosition(), button_reset) && points_count > 0) {
        DrawRectangleLinesEx(button_reset, 5.0, BLACK);
      }
      DrawText(button_reset_text, button_reset.x + font_size / 2, button_reset.y + font_size / 2, font_size, BLACK);

      // Draw points
      for(int i = 0; i < points_count; i++) {
        Color point_color = LIGHTGRAY;
        if(CheckCollisionPointCircle(GetMousePosition(), points[i], CIRCLE_RADIUS)) {
          if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            point_color = PURPLE;
            points[i] = GetMousePosition();
          }
        }
        DrawCircle(points[i].x, points[i].y, CIRCLE_RADIUS, point_color);
      }

      if (button_compute_pressed || (CheckCollisionPointRec(GetMousePosition(), button_compute) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && points_count > 2)) {
        draw_hull(points, points_count);
        button_compute_pressed = true;
      }

    }
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
