#ifndef STRINGS_H_
#define STRINGS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "macros.h"

typedef struct {
    char *data;
    size_t lenght;
} String_View;

typedef struct {
    char *data;
    size_t lenght;
    size_t capacity;
} String_Builder;

/*
    crea una String View partendo dai suoi componenti
*/
String_View sv_from_parts(char *data, size_t lenght);
/*
    crea una String View che rappresenta una Cstr
*/
String_View sv_from_cstr(Cstr *data);
/*
    crea una String View sul puntatore di sb
*/
String_View sv_from_sb(String_Builder *sb);

/*
    crea una String Builder partendo dai sui componeti
*/
String_Builder sb_from_parts(char *data, size_t lenght, size_t capacity);
/*
    duplica una String_View
*/
String_Builder sb_from_sv(String_View *sv);
/*
    duplica una Cstr
*/
String_Builder sb_from_cstr(Cstr *data);
/*
    clona uno String_Builder
*/
String_Builder sb_clone(String_Builder *sb);
/*
    leggi intero file e salva contenuto dentro sb
    @param sb String_Builder dove risiederà il contenuto del file
    @param path path verso il file da leggere

    @return true se è andato tutto bene, false se è capitato un errore, strerr(errno) per leggere l'errore
*/
bool sb_read_entire_file(String_Builder *sb, Cstr *path);

/*
    Appendi una cstr alla fine di una String_Builder
*/
void sb_append_cstr(String_Builder *sb, Cstr *data);
/*
    rendi tutto minuscolo
*/
void sb_to_lowercase(String_Builder *sb);
/*
    rendi tutto maiuscolo
*/
void sb_to_uppercase(String_Builder *sb);
/*
    rendi il puntatore sb.data compatibile con le funzioni che richiedono Cstr
*/
void sb_to_cstr(String_Builder *sb);

/*
    Confrosta due String_View
*/
bool sv_eq(String_View x, String_View y);
/*
    Ritaglia sv in modo tale che punti al prossimo delimitatore
    e ritorna come String_View la parte ritagliata prima del delim.

    @note utile per fare parsing di testo

    @param sv input da ritagliare, viene modificato in modo da puntare dopo il primo delimitatore incontrato
    @param delim il delimitarore che separa le parti di testo

    @return Porzione iniziale del testo prima del delimitatore rappresentato come String_View
*/
String_View sv_chop_by_delim(String_View *sv, char delim);
/*
    salva il contenuto di sv nel file path, se il file esiste
    appende il contenuto di sv alla fine del file
*/
bool sv_save_in_file(String_View *sv, Cstr *path);

#ifdef STRINGS_IMPLEMENTATION

String_View sv_from_parts(char *data, size_t lenght) {
    String_View sv;
    sv.data = data;
    sv.lenght = lenght;
    return sv;
}

String_View sv_from_cstr(Cstr *data) {
    String_View sv;
    size_t cstr_len = strlen(data);
    sv.lenght = cstr_len;
    sv.data = (char*)data;
    return sv;
}

String_View sv_from_sb(String_Builder *sb) {
    String_View sv;
    sv.data = sb->data;
    sv.lenght = sb->lenght;
    return sv;
}

String_Builder sb_from_parts(char *data, size_t lenght, size_t capacity) {
    String_Builder sb;
    sb.data = data;
    sb.lenght = lenght;
    sb.capacity = capacity;
    return sb;
}
String_Builder sb_from_sv(String_View *sv) {
    String_Builder sb;
    size_t n_bytes = INIT_CAP > sv->lenght ? INIT_CAP : sv->lenght;
    sb.data = malloc(n_bytes);
    assert(sb.data != NULL && "Memory full, buy more RAM");
    sb.capacity = n_bytes;
    sb.lenght = sv->lenght;
    memcpy(sb.data, sv->data, sv->lenght);
    return sb;
}

String_Builder sb_clone(String_Builder *sb) {
    String_View sv = sv_from_sb(sb);
    return sb_from_sv(&sv);
}

String_Builder sb_from_cstr(Cstr *data) {
    String_Builder sb = {0};
    sb_append_cstr(&sb, data);
    return sb;
}

bool sb_read_entire_file(String_Builder *sb, Cstr *path) {
    bool result = true;
    
    size_t buf_size = 32*1024;
    char *buf = malloc(buf_size);
    assert(buf != NULL && "Memory full, buy more RAM");
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return_defer(false);
    }

    size_t n = fread(buf, 1, buf_size, f);
    while (n > 0) {
        append_many(sb, buf, n);
        n = fread(buf, 1, buf_size, f);
    }
    if (ferror(f)) {
        return_defer(false);
    }

defer:
    free(buf);
    if (f) fclose(f);
    return result;
}

void sb_append_cstr(String_Builder *sb, Cstr *data) {
    size_t cstr_len = strlen(data);
    append_many(sb, data, cstr_len);
}

void sb_to_lowercase(String_Builder *sb) {
    for(char *it=sb->data; it < sb->lenght + sb->data; ++it)
        *it = tolower(*it);
}

void sb_to_uppercase(String_Builder *sb) {
    for(char *it=sb->data; it < sb->lenght + sb->data; ++it)
        *it = toupper(*it);
}

void sb_to_cstr(String_Builder *sb) {
    append(sb, '\0');
    sb->lenght--;
}

bool sv_eq(String_View x, String_View y) {
    if(x.lenght != y.lenght) return false;
    return memcmp(x.data, y.data, x.lenght) == 0;
}

String_View sv_chop_by_delim(String_View *sv, char delim) {
    size_t i = 0;
    while (i < sv->lenght && sv->data[i] != delim) {
        i += 1;
    }

    String_View result = sv_from_parts(sv->data, i);

    if (i < sv->lenght) {
        sv->lenght -= i + 1;
        sv->data  += i + 1;
    } else {
        sv->lenght -= i;
        sv->data  += i;
    }

    return result;
}

bool sv_save_in_file(String_View *sv, Cstr *path) {
    bool result = true;

    FILE *f = fopen(path,"ab");
    if (f == NULL)
        return_defer(false);
    fwrite(sv->data,1,sv->lenght,f);
    if (ferror(f))
        return_defer(false);
defer:
    if (f) fclose(f);
    return result;
}

#endif // STRINGS_IMPLEMENTATION


#endif // STRINGS_H_
