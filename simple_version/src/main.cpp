#include <Arduino.h>
#include <math.h>

#include "eye.hpp"

#define TFT_BL 10

const int image_width = 139;
const int image_height = 139;

const char path_image_outline[] = "/outline.jpg";
const char path_image_iris[] = "/iris.jpg";
const char path_image_pupil[] = "/pupil.jpg";
const char path_image_reflex[] = "/reflex.jpg";
const char path_image_upperlid[] = "/upperlid.jpg";

const char path_image_outline_surprised[] = "/outline.jpg";
const char path_image_iris_surprised[] = "/iris_surprised.jpg";
const char path_image_pupil_surprised[] = "/pupil_surprised.jpg";
const char path_image_reflex_surprised[] = "/reflex_surprised.jpg";
const char path_image_upperlid_surprised[] = "/upperlid.jpg";

const char path_image_outline_angry[] = "/outline.jpg";
const char path_image_iris_angry[] = "/iris.jpg";
const char path_image_pupil_angry[] = "/pupil.jpg";
const char path_image_reflex_angry[] = "/reflex.jpg";
const char path_image_upperlid_angry[] = "/upperlid_angry.jpg";

const char path_image_outline_sad[] = "/outline.jpg";
const char path_image_iris_sad[] = "/iris.jpg";
const char path_image_pupil_sad[] = "/pupil.jpg";
const char path_image_reflex_sad[] = "/reflex.jpg";
const char path_image_upperlid_sad[] = "/upperlid_sad.jpg";

const char path_image_outline_happy[] = "/outline.jpg";
const char path_image_iris_happy[] = "/white.jpg";
const char path_image_pupil_happy[] = "/white.jpg";
const char path_image_reflex_happy[] = "/reflex_happy.jpg";
const char path_image_upperlid_happy[] = "/white.jpg";

static Eye eye;

void setup()
{
  Serial.begin(115200);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  SPIFFS.begin();
  Serial.println("Initialized.");

  delay(5000);

  eye.init(path_image_outline, path_image_iris, path_image_pupil, path_image_reflex,
          path_image_upperlid, image_width, image_height, 1);
  eye.update_look();

  Serial.println("Start.");
}

void loop()
{
  bool success;

  // 通常
  Serial.println("Normal");
  eye.load_eye_images(path_image_outline, path_image_iris, path_image_pupil, path_image_reflex, path_image_upperlid);
  float look_x, look_y;
  for (int i=0; i<50; i++) {
      delay(20);
      look_x = 1.0 * sin(i * 0.1);
      look_y = 1.0 * cos(i * 0.1) ;
      eye.update_look(look_x, look_y);
  }
  look_x = 0;
  look_y = 0;
  eye.update_look(look_x, look_y);
  delay(1000);

  // まばたき
  Serial.println("Blink");
  eye.load_eye_images(path_image_outline, path_image_iris, path_image_pupil, path_image_reflex, path_image_upperlid);
  int blink_level = 0;
  for (int i=0; i<50; i++) {
      delay(20);
      blink_level = i % Eye::max_blink_level;
      eye.blink_eye(look_x, look_y, blink_level);
  }
  delay(1000);

  // 驚き
  Serial.println("Surprised");
  eye.load_eye_images(path_image_outline_surprised, path_image_iris_surprised,
          path_image_pupil_surprised, path_image_reflex_surprised, path_image_upperlid_surprised);
  int surprised_level = 0;
  for (int i=0; i<50; i++) {
      delay(20);
      surprised_level = i % Eye::max_surprised_level;
      eye.surprised(look_x, look_y, surprised_level);
  }
  delay(1000);

  // 眠い
  Serial.println("Sleepy");
  eye.load_eye_images(path_image_outline, path_image_iris, path_image_pupil, path_image_reflex, path_image_upperlid);
  int sleepy_level = 0;
  for (int i=0; i<50; i++) {
      delay(20);
      sleepy_level = i & Eye::max_sleepy_level;
      eye.sleepy(look_x, look_y, sleepy_level);
  }
  delay(1000);

  // 怒り
  Serial.println("Angry");
  eye.load_eye_images(path_image_outline_angry, path_image_iris_angry,
          path_image_pupil_angry, path_image_reflex_angry, path_image_upperlid_angry);
  int angry_level = 0;
  for (int i=0; i<50; i++) {
      delay(20);
      angry_level = i % Eye::max_angry_level;
      eye.angry(look_x, look_y, angry_level);
  }
  delay(1000);

  // 哀しい
  Serial.println("Sad");
  eye.load_eye_images(path_image_outline_sad, path_image_iris_sad,
          path_image_pupil_sad, path_image_reflex_sad, path_image_upperlid_sad);
  int sad_level = 0;
  for (int i=0; i<50; i++) {
      delay(20);
      sad_level = i % Eye::max_sad_level;
      eye.sad(look_x, look_y);
  }
  delay(1000);

  // 楽しい
  Serial.println("Happy");
  eye.load_eye_images(path_image_outline_happy, path_image_iris_happy,
          path_image_pupil_happy, path_image_reflex_happy, path_image_upperlid_happy);
  int happy_level = 0;
  for (int i=0; i<50; i++) {
      delay(20);
      happy_level = i % Eye::max_happy_level;
      eye.happy(look_x, look_y);
  }
}
