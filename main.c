#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loadfasta.h"

struct output_info {
	struct fasta_file *fa;
	char *f_name;
	uint64_t s_len;
	uint64_t s_len_10k;
	uint32_t c_num;
	uint32_t c_num_10k;
	uint64_t *c_len;
	uint64_t largest_c;
	uint64_t AT_count;
	uint64_t GC_count;
	uint64_t N_count;
	double GC_p;
	uint64_t N50;
	uint64_t L50;
	int outp;
};

void	usage(char *argv);
void	count_seq(struct output_info *output);
void	calc_N50_L50(struct output_info *output);
int	qsort_cmpfunc (const void * a, const void * b);
void	calc_nucleotides(struct output_info *output);
void	output_results(struct output_info *output);
void	free_output(struct output_info *output);

int
main(int argc, char *argv[])
{
	struct output_info output;

	/* Make sure there the number of arguments is correct. */
	if (argc != 2) usage(argv[0]);

	/* Load the fasta file. */
	output.f_name = argv[1];
	fprintf(stderr, "# Loading file %s\n", output.f_name);
	output.fa = load_fasta(output.f_name);
	if (output.fa == NULL) usage(argv[0]);
	fprintf(stderr, "# Counting sequences\n");
	/* Count total sequence size and lengths */
	count_seq(&output);
	/* Calculate N50 L50 GC% and N% */
	fprintf(stderr, "# Calculating values\n");
	calc_N50_L50(&output);
	calc_nucleotides(&output);
	/* Output results and free */
	output_results(&output);
	free_output(&output);
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
count_seq(struct output_info *output)
{
	int i;

	/* We already know the number of contigs */
	output->c_num = output->fa->n_contigs;
	output->c_len = calloc(output->c_num, sizeof(*output->c_len));
	/* We don't know this stuff yet */
	output->s_len = 0;
	output->s_len_10k = 0;
	output->c_num_10k = 0;
	output->largest_c = 0;
	/* Let's figure it out */
	for (i = 0; i < output->c_num; i += 1) {
		*(output->c_len + i) = strlen(*(output->fa->seq + i));
		output->s_len += *(output->c_len + i);
		if (*(output->c_len + i) > 10000) {
			output->s_len_10k += *(output->c_len + i);
			output->c_num_10k += 1;
		}
		if (*(output->c_len + i) > output->largest_c) output->largest_c = *(output->c_len + i);
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
	int i;
	char *c;

	output->GC_count = output->AT_count = output->N_count = 0;
	for (i = 0; i < output->c_num; i += 1) {
		c = *(output->fa->seq + i);
		while (*c != '\0') {
			switch(*c) {
				case 'a':
				case 'A':
				case 't':
				case 'T':
					output->AT_count += 1;
					break;
				case 'c':
				case 'C':
				case 'g':
				case 'G':
					output->GC_count += 1;
					break;
				default:
					output->N_count += 1;
					break;
			}
			c += 1;
		}
	}
	output->GC_p = (double) (output->GC_count) / (double) (output->GC_count + output->AT_count);
	output->GC_p *= 100;
}

void
output_results(struct output_info *output)
{
	printf("File name\t%s\n", output->f_name);
	printf("Seq length\t%ld\n", output->s_len);
	printf("Seq length >10k\t%ld\n", output->s_len_10k);
	printf("Contig num\t%ld\n", output->c_num);
	printf("Contig num >10k\t%ld\n", output->c_num_10k);
	printf("Largest contig\t%ld\n", output->largest_c);
	printf("N50      \t%ld\n", output->N50);
	printf("L50      \t%ld\n", output->L50);
	printf("GC percentage\t%.2lf\n", output->GC_p);
	printf("Number of Ns\t%ld\n", output->N_count);
}

void
free_output(struct output_info *output)
{
	free(output->c_len);
}
