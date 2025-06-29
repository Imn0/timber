typedef struct {
    char* str;
    int size;
    int capacity;
} StringBuilder;

void sb_append(StringBuilder* sb, const char* what);

void sb_nappend(StringBuilder* sb, const char* what, int n);
