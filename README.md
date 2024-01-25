# qqual
Generate quick DNA sequence quality statistics with a given FASTA file

# Output
The program analyzes a DNA FASTA file and generates the following output:

1. sequence length
2. sequence length (only contigs >10 kb)
3. contig number
4. contig number (only contigs >10 kb)
5. largest contig
6. N50
7. L50
8. GC (%)
9. Number of Ns

The program finishes quickly - it takes about 1 second to run for a 100 Mbase assembly.

# Installation
There is a generic Makefile, so it can be compiled with the following commands:

    git clone https://github.com/stephen-cohen/qqual
    cd qqual
    make

The resulting binary will be `qqual` which may be placed into an executable path (e.g. `~/bin` if it exists)

    mv qqual ~/bin

# Running
The program takes a single argument - a FASTA file.

    qqual file.fasta

The output format is tab separated values and can be redirected to a file via either of these methods (the second method will both output a file and display results in the console):

    qqual file.fasta > output.tsv
    qqual file.fasta | tee output.tsv

There is minimal error checking, and it may fail or produce unexpected results. If it fails, post an issue with as many details as possible. 

# Citing
If you use this tool, please kindly cite this repo in-text. There is no version number but you can include the date of the most current commit if you want. You can cite this repo in-text with wording such as:

> Assembly quality was assessed with qqual v2024-01-25 (available at: https://github.com/stephen-cohen/qqual).

# License
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
