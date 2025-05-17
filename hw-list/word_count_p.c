#include "word_count.h"


// Helper function without locking
static word_count_t* find_word_unsafe(word_count_list_t* wclist, char* word) {
    if (word == NULL || list_empty(&wclist->lst)) {
        return NULL;
    }

    for (struct list_elem* e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)) {
        word_count_t* wc = list_entry(e, word_count_t, elem);
        if (strcmp(wc->word, word) == 0) {
            return wc;
        }
    }
    return NULL;
}

void init_words(word_count_list_t* wclist) {
    list_init(&wclist->lst);
    pthread_mutex_init(&wclist->lock, NULL); // Initialize mutex
}

size_t len_words(word_count_list_t* wclist) {
    pthread_mutex_lock(&wclist->lock);
    size_t len = list_size(&wclist->lst);
    pthread_mutex_unlock(&wclist->lock);
    return len;
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
    pthread_mutex_lock(&wclist->lock);
    word_count_t* result = find_word_unsafe(wclist, word);
    pthread_mutex_unlock(&wclist->lock);
    return result;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
    pthread_mutex_lock(&wclist->lock);

    word_count_t* wc = find_word_unsafe(wclist, word);
    if (wc != NULL) {
        wc->count++;
        pthread_mutex_unlock(&wclist->lock);
        return wc;
    }

    // Allocate new entry
    wc = malloc(sizeof(word_count_t));
    if (wc == NULL) {
        pthread_mutex_unlock(&wclist->lock);
        return NULL; // Handle error appropriately
    }
    wc->word = strdup(word);
    if (wc->word == NULL) {
        free(wc);
        pthread_mutex_unlock(&wclist->lock);
        return NULL;
    }
    wc->count = 1;
    list_push_front(&wclist->lst, &wc->elem); // Correctly use elem member

    pthread_mutex_unlock(&wclist->lock);
    return wc;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) {
    if (outfile == NULL) return;

    pthread_mutex_lock(&wclist->lock);
    for (struct list_elem* e = list_begin(&wclist->lst); e != list_end(&wclist->lst); e = list_next(e)) {
        word_count_t* wc = list_entry(e, word_count_t, elem);
        fprintf(outfile, "%i\t%s\n", wc->count, wc->word);
    }
    pthread_mutex_unlock(&wclist->lock);
}

static bool list_less(const struct list_elem* a, const struct list_elem* b, void* aux) {
    word_count_t* wc_a = list_entry(a, word_count_t, elem);
    word_count_t* wc_b = list_entry(b, word_count_t, elem);
    bool (*less)(const word_count_t*, const word_count_t*) = aux;
    return less(wc_a, wc_b);
}

void wordcount_sort(word_count_list_t* wclist, bool less(const word_count_t*, const word_count_t*)) {
    pthread_mutex_lock(&wclist->lock);
    list_sort(&wclist->lst, list_less, less);
    pthread_mutex_unlock(&wclist->lock);
}