#include "scene.h"
#include "escape-sequences.h"

static int cam_x, cam_y, cam_z;
static int obj_x, obj_y, obj_z;

void scene_init() {
  cam_x = 0;
  cam_y = 0;
  cam_z = 0;
  obj_x = 10;
  obj_y = 0;
  obj_z = 0;
}

void scene_update(char **win_buff, int pos_x, int pos_y, int cam_w, int cam_h) {

  for (int pixel_x = pos_x; pixel_x < cam_w; pixel_x++) {

    for (int pixel_y = pos_y; pixel_y < cam_h; pixel_y++) {

      term_move(win_buff, pixel_y, pixel_x);
      term_write_char(win_buff, 'X');
    }
  }
}
