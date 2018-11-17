#ifndef VBUFFER_H
#define VBUFFER_H

#define INITIAL_BUFFER_CAPACITY 8

struct vbuffer {
    char *data;
    int length;
    int capacity;
};

struct vbuffer* vbuffer_new(void);
void vbuffer_free(struct vbuffer *buffer);
int vbuffer_pushchar(struct vbuffer *buffer, char c);
int vbuffer_readfile(struct vbuffer *buffer, const char *filename);

#endif