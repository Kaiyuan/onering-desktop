#ifndef ONERINGAPP_H
#define ONERINGAPP_H

#include <onering.h>

int onering_app_init(void);
onering_response_handle_t onering_app(const char *appname, const char *method, const char *path, const char *body, const char **response, int *response_len);
void onering_app_free_response(const char *appname, onering_response_handle_t response_handle);

#endif
