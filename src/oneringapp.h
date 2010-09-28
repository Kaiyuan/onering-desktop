#ifndef ONERINGAPP_H
#define ONERINGAPP_H

int onering_app_init(void);
void onering_app(const char *method, const char *path, const char *body, const char **response, int *response_len);

#endif
