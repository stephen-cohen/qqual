#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct output_info {
	char *f_name;
	uint64_t s_len;
	uint64_t s_len_10k;
	uint64_t c_num;
	uint64_t c_num_10k;
	uint64_t *c_len;
	uint64_t largest_c;
	uint64_t AT_count;
	uint64_t GC_count;
	uint64_t N_count;
	double GC_p;
	uint64_t N50;
	uint64_t L50;
	double auN;
};

void			usage(char *argv);
void			error_parse(int err);
void			calc_N50_L50(struct output_info *output);
int			qsort_cmpfunc (const void * a, const void * b);
void			calc_nucleotides(struct output_info *output);
void			output_results(struct output_info *output);
struct output_info *	init_info(char *f_name);
void			kill_info(struct output_info *info);
int			fasta_count(struct output_info *info);

enum STATUS { ERR_MEM, ERR_FILE, ERR_FASTA_HEADER, ERR_UNK_CHAR, STATUS_COMPLETE };

int
main(int argc, char *argv[])
{
	int err;
	struct output_info *info;

	/* Make sure there the number of arguments is correct */
	if (argc != 2) usage(argv[0]);
	/* Allocate memory for structure */
	info = init_info(argv[1]);
	/* Load the sequence information from a FASTA file */
	fprintf(stderr, "# Loading file %s\n", info->f_name);
	err = fasta_count(info);
	if (err != STATUS_COMPLETE) {
		error_parse(err);
		exit(1);
	}
	/* Calculate N50 L50 GC% and N% */
	fprintf(stderr, "# Calculating values\n");
	calc_N50_L50(info);
	calc_nucleotides(info);
	/* Output results and free */
	output_results(info);
	kill_info(info);
	fprintf(stderr, "# Finished\n");
	return 0;
}

void
usage(char *argv)
{
	printf("# Usage:\n");
	printf("# \t%s file.fasta\n", argv);
	exit(1);
}

void
error_parse(int err)
{
	switch(err) {
		case ERR_MEM:
			printf("# ERROR: Could not allocate memory\n");
			break;
		case ERR_FILE:
			printf("# ERROR: Could not open file\n");
			break;
		case ERR_FASTA_HEADER:
			printf("# ERROR: File did not contain expected > in a sequence header line\n");
			break;
		case ERR_UNK_CHAR:
			printf("# ERROR: Unknown character in a sequence line\n");
			break;
	}
}

void
calc_N50_L50(struct output_info *output)
{
	uint64_t i, top_count;
	/* Sort sizes first */
	qsort(output->c_len, output->c_num, sizeof(uint64_t), qsort_cmpfunc);
	/* Find how many contigs needed for half total size */
	for (i = output->c_num - 1, top_count = 0, output->L50 = 0; top_count < (output->s_len / 2); i -= 1) {
		top_count += *(output->c_len + i);
		output->L50 += 1;
	}
	/* What was the smallest contig we added to get there? */
	output->N50 = *(output->c_len + i + 1);
	/* Now calculate auN */
	for (i = 0; i < output->c_num; i += 1) {
		output->auN += ((float) *(output->c_len + i) * (float) *(output->c_len + i));
	}
	output->auN /= output->s_len;
}

int
qsort_cmpfunc(const void * a, const void * b)
{
	if (*(uint64_t *)a < *(uint64_t *)b) {
		return -1;
	} else if (*(uint64_t *)a > *(uint64_t *)b) {
		return 1;
	} else {
		return 0;
	}
}

void
calc_nucleotides(struct output_info *output)
{
	output->GC_p = (double) (output->GC_count) / (double) (output->GC_count + output->AT_count);
	output->GC_p *= 100;
}

void
output_results(struct output_info *output)
{
	printf("File name\t%s\n", output->f_name);
	printf("Seq length\t%" PRIu64 "\n", output->s_len);
	printf("Seq length >10k\t%" PRIu64 "\n", output->s_len_10k);
	printf("Contig num\t%" PRIu64 "\n", output->c_num);
	printf("Contig num >10k\t%" PRIu64 "\n", output->c_num_10k);
	printf("Largest contig\t%" PRIu64 "\n", output->largest_c);
	printf("N50      \t%" PRIu64 "\n", output->N50);
	printf("L50      \t%" PRIu64 "\n", output->L50);
	printf("auN      \t%.2lf\n", output->auN);
	printf("GC percentage\t%.2lf\n", output->GC_p);
	printf("Number of Ns\t%" PRIu64 "\n", output->N_count);
}

struct output_info *
init_info(char *f_name)
{
	struct output_info *info;
	info = malloc(sizeof(*info));
	info->f_name = f_name;
	info->s_len = 0;
	info->s_len_10k = 0;
	info->c_num = 0;
	info->c_num_10k = 0;
	info->c_len = NULL;
	info->largest_c = 0;
	info->AT_count = 0;
	info->GC_count = 0;
	info->N_count = 0;
	info->auN = 0.0;
	return info;
}

void
kill_info(struct output_info *info)
{
	if (info->c_len != NULL) free(info->c_len);
	free(info);
}

int
fasta_count(struct output_info *info)
{
	char c;
	int break_loop;
	uint64_t max_num_records;
	uint64_t cur_seq_len;
	FILE *f;

	/* Try to open the file */
	f = fopen(info->f_name, "r");
	if (f == NULL) return ERR_FILE;
	/* Allocate memory for sequence lengths */
	max_num_records = 1000;
	info->c_len = malloc(sizeof(uint64_t) * max_num_records);
	if (info->c_len == NULL) return ERR_MEM;
	while (1) {
		/* Make sure first character is a >, then scan to end of line */
		c = fgetc(f);
		if (c != '>') return ERR_FASTA_HEADER;
		while ((c = fgetc(f)) != '\n');
		/* Do we need to reallocate memory? */
		if (info->c_num > (max_num_records - 1)) {
			max_num_records += 1000;
			info->c_len = realloc(info->c_len, sizeof(uint64_t) * max_num_records);
			if (info->c_len == NULL) return ERR_MEM;
		}
		/* Generate stats for current sequence */
		cur_seq_len = 0;
		break_loop = 0;
		while (break_loop == 0) {
			c = fgetc(f);
			/* Ignore newlines */
			if (c == '\r') continue;
			/* Count nucleotides */
			if (c == 'A' || c == 'a' || c == 'T' || c == 't') {
				info->AT_count += 1;
				cur_seq_len += 1;
			} else if (c == 'C' || c == 'c' || c == 'G' || c == 'g') {
				info->GC_count +=1;
				cur_seq_len += 1;
			} else if (c == 'N' || c == 'n') {
				info->N_count += 1;
				cur_seq_len += 1;
			/* See if you need to break out of this loop */
			} else if (c == '\n') {
				c = fgetc(f);
				fseek(f, -1, SEEK_CUR);
				if (c == '>' || c == EOF) {
					break_loop = 1;
				}
			/* Unrecognized character in file */
			} else {
				return ERR_UNK_CHAR;
			}
		}
		/* Add sequence length to information */
		info->c_len[info->c_num] = cur_seq_len;
		info->s_len += cur_seq_len;
		info->c_num += 1;
		if (cur_seq_len > info->largest_c) info->largest_c = cur_seq_len;
		if (cur_seq_len > 10000) {
			info->s_len_10k += cur_seq_len;
			info->c_num_10k += 1;
		}
		/* If it's the end of file, exit */
		if (c == EOF) {
			return STATUS_COMPLETE;
		}
	}
}
