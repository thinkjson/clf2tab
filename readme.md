# clf2tab

Convert logs in Apache Combined Log Format to tab delimited for use with unix text streaming tools

## Usage

clf2tab <log >outfile
clf2tab <log | cut -f2,6 | sort | uniq