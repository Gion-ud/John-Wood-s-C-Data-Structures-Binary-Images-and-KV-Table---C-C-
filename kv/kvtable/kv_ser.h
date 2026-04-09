#pragma once

typedef struct _kvtblobj *KVTableHandle;

extern int KVTable_serialise(KVTableHandle kvt_h, int fd);
extern KVTableHandle KVTable_create_load_from_file(int fd);