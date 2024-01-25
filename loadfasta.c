/*
 * loadfasta by Stephen P. Cohen
 * See https://github.com/stephen-cohen/loadfasta
 */

#include <stdio.h>
#include <stdlib.h>
#include "loadfasta.h"

struct fasta_file *
load_fasta(char *file)
{
	char c;
	long file_size;
	long i;
	FILE *f;
	struct fasta_file *fasta;
	int n_contigs;
	char *data = NULL;
	char **name = NULL;
	char **seq = NULL;

	/* Find file size to figure out required buffer size */
	f = fopen(file, "r");
	if (f == NULL) return NULL;
	fseek(f, 0, SEEK_END);
	file_size = ftell(f);
	rewind(f);

	/* Allocate space for buffer and load in data byte by byte */
	data = malloc(sizeof(*data)*file_size);
	for (i = 0, n_contigs = 0; (c = fgetc(f)) != EOF; i++) {
		*(data+i) = c;
		if (c == '>') {
			n_contigs++;
			if (n_contigs == 1) {
				name = malloc(n_contigs*sizeof(*name));
				*(name+n_contigs-1) = data+i;
				seq = malloc(n_contigs*sizeof(*seq));
			} else {
				*(data+i) = '\0';
				i++;
				name = realloc(name, n_contigs*sizeof(*name));
				*(name+n_contigs-1) = data+i;
				seq = realloc(seq, n_contigs*sizeof(*seq));
			}
			while ((c = fgetc(f)) != '\n') {
				*(data+i) = c;
				i++;
			}
			*(data+i) = '\0';
			*(seq+n_contigs-1) = data+i+1;
		} else if (c == '\n') {
			i--;
		}
	}
	*(data+i) = '\0';
	data = realloc(data, (i+1)*sizeof(*data));
 	fclose(f);

 	/* Put it in a structure to return */
	fasta = malloc(sizeof(*fasta));
 	fasta->n_contigs = n_contigs;
 	/* Try to shrink the memory reserved due to newlines in the file */
	fasta->data = realloc(data, (i+1)*sizeof(*fasta->data));
	if (fasta->data == NULL) fasta->data = data;
	fasta->name = name;
	fasta->seq = seq;
	return fasta;
}

void
del_fasta(struct fasta_file *fasta)
{
	if (fasta != NULL) {
		if (fasta->name != NULL) free(fasta->name);
		if (fasta->seq != NULL) free(fasta->seq);
		if (fasta->data != NULL) free(fasta->data);
		free(fasta);
	}
}
