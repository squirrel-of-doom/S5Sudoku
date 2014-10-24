###
# make_bitcount_header.py
#
# Creates a C++ header file bitcount.h
# This header defines the array BITCOUNT,
# which includes the number of bits set
# for the bitmasks represented by 0..511
###

# Number of bits (for Sudoku: 9)
N_BITS = 9

# Array of powers of two
POWERS_OF_TWO = [2**n for n in reversed(range(N_BITS))]

# Function count_bits
# IN: bitmask -- OUT: Number of bits set
def count_bits(bitmask):
    count = 0
    dummy = bitmask
    for power in POWERS_OF_TWO:
        if dummy >= power:
            count += 1
            dummy -= power
    return count

def make_bitcount_dict():
    return {bitmask: count_bits(bitmask) for bitmask in range(2**N_BITS)}
    
def make_bitcount_list():
    return [count_bits(bitmask) for bitmask in range(2**N_BITS)]
    
def make_header_file():
    filelines = []
    filelines.append("/*****************************************\n")
    filelines.append(" * bitcount.h\n")
    filelines.append(" * \n")
    filelines.append(" * Generated by make_bitcount_header.py\n")
    filelines.append(" * This header defines the array BITCOUNT,\n")
    filelines.append(" * which includes the number of bits set\n")
    filelines.append(" * for the bitmasks represented by 0..511\n")
    filelines.append(" ****************************************/\n")

    filelines.append("const int BITCOUNT[] = {%s};\n\n" % repr(make_bitcount_list()).strip('[]'))
    
    with open('bitcount.h', 'w') as headerfile:
        headerfile.writelines(filelines)
    
    return 0
    
if __name__ == '__main__':
    make_header_file()