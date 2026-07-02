def slowRookAttacks(index, startSquare, blockerSubset):
    # Given a start square and a set of blockers
    # What squares can we actually attack
    # Recompute attack again, stopping in that direction if we see a blocker
    mask = 0
    rank = index // 8
    file = index % 8
    squaresRight = 7 - file
    squaresLeft = file
    squaresAbove = 7 - rank
    squaresBelow = rank
    for i in range(squaresRight+1):
        mask = mask ^ (startSquare << i)
        if (blockerSubset & (startSquare << i)) > 0:
            break
    for i in range(squaresLeft+1):
        mask = mask ^ (startSquare >> i)
        if (blockerSubset & (startSquare >> i)) > 0:
            break
    for i in range(squaresAbove+1):
        mask = mask ^ (startSquare << (8 * i))
        if (blockerSubset & (startSquare << (8 * i))) > 0:
            break
    for i in range(squaresBelow+1):
        mask = mask ^ (startSquare >> (8 * i))
        if (blockerSubset & (startSquare >> (8 * i))) > 0:
            break
    return mask

def generateRookMoveTable(square_index, mask, magic, shift):
    startSquare = 1 << square_index
    relevant_bits = bin(mask).count('1')
    table_size = 1 << relevant_bits
    table = [0] * table_size

    # Enumerate all subsets of the mask
    subset = mask
    while True:
        # Compute attack bitboard for this blocker subset
        attack_bb = slowRookAttacks(square_index, startSquare, subset)
        # Compute magic index
        index = ((subset * magic) >> shift) & (table_size - 1)
        table[index] = attack_bb

        # Move to next subset
        subset = (subset - 1) & mask
        if subset == mask:
            break

    return table

MASKS = [0x000101010101017E,
0x000202020202027C,
0x000404040404047A,
0x0008080808080876,
0x001010101010106E,
0x002020202020205E,
0x004040404040403E,
0x008080808080807E,
0x0001010101017E00,
0x0002020202027C00,
0x0004040404047A00,
0x0008080808087600,
0x0010101010106E00,
0x0020202020205E00,
0x0040404040403E00,
0x0080808080807E00,
0x00010101017E0100,
0x00020202027C0200,
0x00040404047A0400,
0x0008080808760800,
0x00101010106E1000,
0x00202020205E2000,
0x00404040403E4000,
0x00808080807E8000,
0x000101017E010100,
0x000202027C020200,
0x000404047A040400,
0x0008080876080800,
0x001010106E101000,
0x002020205E202000,
0x004040403E404000,
0x008080807E808000,
0x0001017E01010100,
0x0002027C02020200,
0x0004047A04040400,
0x0008087608080800,
0x0010106E10101000,
0x0020205E20202000,
0x0040403E40404000,
0x0080807E80808000,
0x00017E0101010100,
0x00027C0202020200,
0x00047A0404040400,
0x0008760808080800,
0x00106E1010101000,
0x00205E2020202000,
0x00403E4040404000,
0x00807E8080808000,
0x007E010101010100,
0x007C020202020200,
0x007A040404040400,
0x0076080808080800,
0x006E101010101000,
0x005E202020202000,
0x003E404040404000,
0x007E808080808000,
0x7E01010101010100,
0x7C02020202020200,
0x7A04040404040400,
0x7608080808080800,
0x6E10101010101000,
0x5E20202020202000,
0x3E40404040404000,
0x7E80808080808000]

MAGICS = [0x5080008011400020,
0x0140001000402000,
0x0280091000200480,
0x0700081001002084,
0x0300024408010030,
0x510004004E480100,
0x0400044128020090,
0x8080004100012080,
0x0220800480C00124,
0x0020401001C02000,
0x000A002204428050,
0x004E002040100A00,
0x0102000A00041020,
0x0A0880040080C200,
0x0002000600018408,
0x0025001200518100,
0x8900328001400080,
0x0848810020400100,
0xC001410020010153,
0x4110C90020100101,
0x00A0808004004800,
0x401080801C000601,
0x0100040028104221,
0x840002000900A054,
0x1000348280004000,
0x001000404000E008,
0x0424410300200035,
0x2008C22200085200,
0x0005304D00080100,
0x000C040080120080,
0x8404058400080210,
0x0001848200010464,
0x6000204001800280,
0x2410004003C02010,
0x0181200A80801000,
0x000C60400A001200,
0x0B00040180802800,
0xC00A000280804C00,
0x4040080504005210,
0x0000208402000041,
0xA200400080628000,
0x0021020240820020,
0x1020027000848022,
0x0020500018008080,
0x10000D0008010010,
0x0100020004008080,
0x0008020004010100,
0x12241C0880420003,
0x4000420024810200,
0x0103004000308100,
0x008C200010410300,
0x2410008050A80480,
0x0820880080040080,
0x0044220080040080,
0x2040100805120400,
0x0129000080C20100,
0x0010402010800101,
0x0648A01040008101,
0x0006084102A00033,
0x0002000870C06006,
0x0082008820100402,
0x0012008410050806,
0x2009408802100144,
0x821080440020810A]

SHIFTS = [52,
53,
53,
53,
53,
53,
53,
52,
53,
54,
54,
54,
54,
54,
54,
53,
53,
54,
54,
54,
54,
54,
54,
53,
53,
54,
54,
54,
54,
54,
54,
53,
53,
54,
54,
54,
54,
54,
54,
53,
53,
54,
54,
54,
54,
54,
54,
53,
53,
54,
54,
54,
54,
54,
54,
53,
52,
53,
53,
53,
53,
53,
53,
52]

ROOK_TABLES = []
ROOK_TABLE_FLAT = []
ROOK_OFFSETS = []

for i in range(64):
    mask = MASKS[i]
    magic = MAGICS[i]
    shift = SHIFTS[i]
    table = generateRookMoveTable(i, mask, magic, shift)
    ROOK_TABLES.append(table)

offset = 0
for table in ROOK_TABLES:
    ROOK_OFFSETS.append(offset)
    ROOK_TABLE_FLAT.extend(table)
    offset += len(table)

with open("src/rook_table.cpp", "w") as f:
    f.write("constexpr uint64_t ROOK_TABLE[] = {\n")
    for bb in ROOK_TABLE_FLAT:
        f.write(f"    0x{bb:016X}ULL,\n")
    f.write("};\n\n")

    f.write("constexpr int ROOK_OFFSETS[] = {\n")
    for off in ROOK_OFFSETS:
        f.write(f"    {off},\n")
    f.write("};\n")


