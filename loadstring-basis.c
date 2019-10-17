
int handle_buffer(const char *s, int (*handler)(const char *)) {
    char local_buffer[0x10];
    int i = 0;
    for (; *s; s++) {
        local_buffer[i++] = *s;
    }
    local_buffer[i] = 0;
    return handler(local_buffer);
}
