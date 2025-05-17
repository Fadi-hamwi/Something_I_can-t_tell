/*
 * Word count application with one thread per input file.
 *
 * You may modify this file in any way you like, and are expected to modify it.
 * Your solution must read each input file from a separate thread. We encourage
 * you to make as few changes as necessary.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#include "word_count.h"
#include "word_helpers.h"

/*
 * main - handle command line, spawning one thread per file.
 */
typedef struct args {
      word_count_list_t* wcs;
      FILE* infile;
} args;

void* count_words_helper(void* a) {
    args* args_ptr = (args*)a;
    count_words(args_ptr->wcs, args_ptr->infile);
    fclose(args_ptr->infile);
    free(args_ptr);
    return NULL;
}

int main(int argc, char* argv[]) {
    word_count_list_t word_counts;
    init_words(&word_counts); // Ensure this initializes a mutex

    if (argc <= 1) {
        count_words(&word_counts, stdin);
    } else {
        pthread_t threads[argc - 1];
        for (int i = 1; i < argc; i++) {
            args* a = malloc(sizeof(args));
            if (a == NULL) { /* handle error */ continue; }
            a->wcs = &word_counts; // Share the same list
            a->infile = fopen(argv[i], "r");
            if (a->infile == NULL) {
                fprintf(stderr, "Failed to open %s\n", argv[i]);
                free(a);
                continue;
            }
            int rc = pthread_create(&threads[i - 1], NULL, count_words_helper, (void*)a);
            if (rc) { /* handle error */ 
              printf("ERROR; Couldn't create a thread for %s", argv[i]);
              fclose(a->infile);
              free(a);
              continue;
            }
        }
        // Join all threads
        for (int i = 0; i < argc - 1; i++) {
            pthread_join(threads[i], NULL);
        }
    }
    // Output results
    wordcount_sort(&word_counts, less_count);
    fprint_words(&word_counts, stdout);
    return 0;
}