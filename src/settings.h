#ifndef SETTINGS_H
#define SETTINGS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
#ifndef EXTERN
#define EXTERN extern
#endif

EXTERN struct {
  unsigned int grid_x, grid_y;
  float scale;
  char vertex_shader_path[400];
  char fragment_shader_path[400];
} settings;

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define TYPE_DEC_FMT(x)                                                        \
  _Generic((x),                                                                \
      char: "%c",                                                              \
      signed char: "%hhd",                                                     \
      unsigned char: "%hhu",                                                   \
      signed short: "%hd",                                                     \
      unsigned short: "%hu",                                                   \
      signed int: "%d",                                                        \
      unsigned int: "%u",                                                      \
      long int: "%ld",                                                         \
      unsigned long int: "%lu",                                                \
      long long int: "%lld",                                                   \
      unsigned long long int: "%llu",                                          \
      float: "%f",                                                             \
      double: "%f",                                                            \
      long double: "%Lf",                                                      \
      char *: "%s",                                                            \
      void *: "%p")

#define LOAD_SETTING(name, default)                                            \
  {                                                                            \
    settings.name = default;                                                   \
    char *word = strstr(settingsData, #name);                                  \
    if (word != NULL) {                                                        \
      char *lineEnd = strstr(word, "\n");                                      \
      if (lineEnd != NULL) {                                                   \
        *lineEnd = '\0';                                                       \
        char *arg = strstr(word, "=") + 1;                                     \
        if (arg != NULL && arg < lineEnd) {                                    \
          sscanf(arg, TYPE_DEC_FMT(settings.name), &settings.name);            \
        }                                                                      \
        *lineEnd = '\n';                                                       \
      } else {                                                                 \
        char *arg = strstr(word, "=") + 1;                                     \
        if (arg != NULL && arg < settingsData + settingsDataLength) {          \
          sscanf(arg, TYPE_DEC_FMT(settings.name), &settings.name);            \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }

#define LOAD_SETTING_S(name, default)                                          \
  {                                                                            \
    memcpy(settings.name, default,                                             \
           min(strlen(default), sizeof(settings.name)));                       \
    settings.name[min(strlen(default), sizeof(settings.name))] = '\0'; \
    char *word = strstr(settingsData, #name);                                  \
    if (word != NULL) {                                                        \
      char *lineEnd = strstr(word, "\n");                                      \
      if (lineEnd != NULL) {                                                   \
        *lineEnd = '\0';                                                       \
        char *arg = strstr(word, "=") + 1;                                     \
        if (arg != NULL && arg < lineEnd) {                                    \
          sscanf(arg, TYPE_DEC_FMT((char *)settings.name), settings.name);     \
        }                                                                      \
        *lineEnd = '\n';                                                       \
      } else {                                                                 \
        char *arg = strstr(word, "=") + 1;                                     \
        if (arg != NULL && arg < settingsData + settingsDataLength) {          \
          sscanf(arg, TYPE_DEC_FMT((char *)settings.name), settings.name);     \
        }                                                                      \
      }                                                                        \
    }                                                                          \
  }

static char *loadFile(const char *path, unsigned int *size) {
  int string_size, read_size;
  FILE *file = fopen(path, "rb"); // rb to prevent windows issues

  if (file) {
    fseek(file, 0, SEEK_END);
    string_size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(sizeof(char) * (string_size + 1));
    read_size = fread(buffer, sizeof(char), string_size, file);

    buffer[string_size] = '\0';

    if (string_size != read_size) {
      // Something went wrong
      free(buffer);
      buffer = NULL;
    }

    if (buffer) {
      *size = string_size;
      return buffer;
    }

    fclose(file);
  }
  perror("File opening failed");
  return NULL;
}

static void loadSettings() {
  unsigned int settingsDataLength;
  char *settingsData = loadFile("settings.conf", &settingsDataLength);

  if (settingsData == NULL) {
    fprintf(stderr, "[ERROR] could not read settings file");
    return;
  }

  char *currentChar = settingsData;

  LOAD_SETTING(grid_x, 30);
  LOAD_SETTING(grid_y, 30);
  LOAD_SETTING(scale, 2.0f);
  LOAD_SETTING_S(vertex_shader_path, "vertex_shader.glsl");
  LOAD_SETTING_S(fragment_shader_path, "fragment_shader.glsl");

  free(settingsData);
}

#endif
