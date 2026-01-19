#include "scene.h"
#include "escape-sequences.h"
#include <math.h>

typedef struct {
  double x, y, z;
} Vector3;

Vector3 vector_sub(Vector3 a, Vector3 b) {
  return (Vector3){a.x - b.x, a.y - b.y, a.z - b.z};
}

double vector_dot(Vector3 a, Vector3 b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

double vector_len(Vector3 a) { return sqrt(vector_dot(a, a)); }

Vector3 vector_norm(Vector3 a) {
  double length = vector_len(a);
  return (Vector3){a.x / length, a.y / length, a.z / length};
}

double ray_hit_sphere(Vector3 center, double radius, Vector3 ray_origin,
                      Vector3 ray_dir) {
  Vector3 center_to_origin = vector_sub(ray_origin, center);
  double a = vector_dot(ray_dir, ray_dir);
  double b = 2.0 * vector_dot(center_to_origin, ray_dir);
  double c = vector_dot(center_to_origin, center_to_origin) - radius * radius;
  double discriminant = b * b - 4 * a * c;

  if (discriminant < 0) {
    return -1.0;
  } else {
    return (-b - sqrt(discriminant)) / (2.0 * a);
  }
}

Vector3 camera_pos = {0, 0, 0};
Vector3 sphere_pos = {0, 0, 6};
double sphere_radius = 2.0f;

void scene_init() {}

void scene_update(char **win_buff, int pos_x, int pos_y, int cam_w, int cam_h) {

  double aspect = (double)cam_w / cam_h;
  double char_aspect = 0.5;

  for (int pixel_y = pos_x; pixel_y < cam_h; pixel_y++) {
    term_move(win_buff, pixel_y + 1, 1);

    for (int pixel_x = pos_y; pixel_x < cam_w; pixel_x++) {

      double u = (double)pixel_x / cam_w * 2.0 - 1.0;
      double v = (double)pixel_y / cam_h * 2.0 - 1.0;
      u *= aspect * char_aspect;

      Vector3 ray_dir = vector_norm((Vector3){u, -v, 1.0f});
      double dist =
          ray_hit_sphere(sphere_pos, sphere_radius, camera_pos, ray_dir);

      if (dist > 0) {
        char pixel = '#';
        if (dist > 5.0)
          pixel = '+';
        if (dist > 5.5)
          pixel = '.';
        term_write_char(win_buff, pixel);
      } else {
        // MISS (Background)
        term_write_char(win_buff, ' ');
      }
    }
  }
}
