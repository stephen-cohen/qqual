# qqual
Generate quick DNA sequence quality statistics with a given FASTA file

# output
The program generates output including:

1. sequence length
2. sequence length (only contigs >10 kb)
3. contig number
4. contig number (only contigs >10 kb)
5. largest contig
6. N50
7. L50
8. GC (%)
9. Number of Ns

And it should be quick - it takes about 1 second to run for a 100 Mbase assembly on my computer.

# compiling
I have included a generic Makefile so you should be able to install via:

    git clone https://github.com/stephen-cohen/qqual
    cd qqual
    make

The resulting binary will be `qqual` which I recommend putting into your executable path (e.g. `~/bin`)

# running
The program takes a single argument - a FASTA file.

    qqual file.fasta

The output format is tab separated values and can be redirected to a file via either of these methods (the second method will both output a file and display results in the console):

    qqual file.fasta > output.tsv
    qqual file.fasta | tee output.tsv

There is minimal error checking, and it may fail or produce unexpected results. If it fails, post an issue with as many details as possible and I can help you troubleshoot. 

# citing
Just cite this repo. You can use text such as:

> Assembly quality was assessed with qqual (available at: https://github.com/stephen-cohen/qqual).

# license
MIT License

Copyright (c) 2024 Stephen P. Cohen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
