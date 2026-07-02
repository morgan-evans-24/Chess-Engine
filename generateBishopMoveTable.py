def slowBishopAttacks(index, startSquare, blockerSubset):
    mask = 0
    rank = index // 8
    file = index % 8
    squaresRight = 7 - file
    squaresLeft = file
    squaresAbove = 7 - rank
    squaresBelow = rank
    # North-East
    for i in range(1, min(squaresRight, squaresAbove)+1):
        targetSquare = startSquare << (9*i)
        mask = mask | targetSquare
        if (blockerSubset & targetSquare) > 0:
            break
    # South-East
    for i in range(1, min(squaresRight, squaresBelow)+1):
        targetSquare = startSquare >> (7*i)
        mask = mask | targetSquare
        if (blockerSubset & targetSquare) > 0:
            break
    # South-West
    for i in range(1, min(squaresLeft, squaresBelow)+1):
        targetSquare = startSquare >> (9*i)
        mask = mask | targetSquare
        if (blockerSubset & targetSquare) > 0:
            break
    # North-West
    for i in range(1, min(squaresLeft, squaresAbove)+1):
        targetSquare = startSquare << (7*i)
        mask = mask | targetSquare
        if (blockerSubset & targetSquare) > 0:
            break
    return mask

def generateBishopMoveTable(square_index, mask, magic, shift):
    startSquare = 1 << square_index
    relevant_bits = bin(mask).count('1')
    table_size = 1 << relevant_bits
    table = [0] * table_size

    # Enumerate all subsets of the mask
    subset = mask
    while True:
        # Compute attack bitboard for this blocker subset
        attack_bb = slowBishopAttacks(square_index, startSquare, subset)
        # Compute magic index
        index = ((subset * magic) >> shift) & (table_size - 1)
        table[index] = attack_bb

        # Move to next subset
        subset = (subset - 1) & mask
        if subset == mask:
            break

    return table


MASKS = [0x0040201008040200,
0x0000402010080400,
0x0000004020100A00,
0x0000000040221400,
0x0000000002442800,
0x0000000204085000,
0x0000020408102000,
0x0002040810204000,
0x0020100804020000,
0x0040201008040000,
0x00004020100A0000,
0x0000004022140000,
0x0000000244280000,
0x0000020408500000,
0x0002040810200000,
0x0004081020400000,
0x0010080402000200,
0x0020100804000400,
0x004020100A000A00,
0x0000402214001400,
0x0000024428002800,
0x0002040850005000,
0x0004081020002000,
0x0008102040004000,
0x0008040200020400,
0x0010080400040800,
0x0020100A000A1000,
0x0040221400142200,
0x0002442800284400,
0x0004085000500800,
0x0008102000201000,
0x0010204000402000,
0x0004020002040800,
0x0008040004081000,
0x00100A000A102000,
0x0022140014224000,
0x0044280028440200,
0x0008500050080400,
0x0010200020100800,
0x0020400040201000,
0x0002000204081000,
0x0004000408102000,
0x000A000A10204000,
0x0014001422400000,
0x0028002844020000,
0x0050005008040200,
0x0020002010080400,
0x0040004020100800,
0x0000020408102000,
0x0000040810204000,
0x00000A1020400000,
0x0000142240000000,
0x0000284402000000,
0x0000500804020000,
0x0000201008040200,
0x0000402010080400,
0x0002040810204000,
0x0004081020400000,
0x000A102040000000,
0x0014224000000000,
0x0028440200000000,
0x0050080402000000,
0x0020100804020000,
0x0040201008040200
]

MAGICS = [0x0040420094048181,
0x0224714811010008,
0x12300C0040400020,
0x0028087100805200,
0x8202021000002221,
0x8203880440608005,
0x2084151422201208,
0x0001010110010400,
0x1020089103082102,
0x0100041054084082,
0x02E42105010A0000,
0x0021A40401820000,
0x0002011041280080,
0x2000120110B8C100,
0x0100040402021201,
0x2080108048021000,
0x01200E28881040C0,
0x0808002002408220,
0x8001004608110100,
0x8442002022004011,
0x1001001820181400,
0x3A010010300E1000,
0x2481104A01012000,
0x80006A010092101C,
0x000C040010103001,
0x4118020008500322,
0x0004040002002400,
0x812B280024004010,
0x0001010000104008,
0x1010010400814112,
0x002101414C024808,
0x02004201028A0102,
0x05100210004830A0,
0x0082101080040100,
0x0004020800010040,
0x00004018210A0200,
0x0201100401008020,
0x0020041302402080,
0x3808018408090505,
0x0C04040040902500,
0x4118025010008400,
0x0002008420091402,
0x2200840402000902,
0x8000004208010080,
0x0014041102108400,
0xB020081000201110,
0x0CF0100080A02510,
0x80140C0403420124,
0x040101284A400008,
0xC5010908020A0001,
0x0140051581900002,
0x0000220042020008,
0x8C00021002160000,
0x8D030982081200E0,
0x10511C101420401A,
0x204818010400B202,
0x5560340228040408,
0x1004010402020210,
0x0000000084140600,
0x0081402000840400,
0x00000480200A048E,
0x0000180810300080,
0x00401014C808C400,
0x0004183001102100
]

SHIFTS = [58,
59,
59,
59,
59,
59,
59,
58,
59,
59,
59,
59,
59,
59,
59,
59,
59,
59,
57,
57,
57,
57,
59,
59,
59,
59,
57,
55,
55,
57,
59,
59,
59,
59,
57,
55,
55,
57,
59,
59,
59,
59,
57,
57,
57,
57,
59,
59,
59,
59,
59,
59,
59,
59,
59,
59,
58,
59,
59,
59,
59,
59,
59,
58
]

BISHOP_TABLES = []
BISHOP_TABLE_FLAT = []
BISHOP_OFFSETS = []

for i in range(64):
    mask = MASKS[i]
    magic = MAGICS[i]
    shift = SHIFTS[i]
    table = generateBishopMoveTable(i, mask, magic, shift)
    BISHOP_TABLES.append(table)

offset = 0
for table in BISHOP_TABLES:
    BISHOP_OFFSETS.append(offset)
    BISHOP_TABLE_FLAT.extend(table)
    offset += len(table)

with open("src/bishop_table.cpp", "w") as f:
    f.write("constexpr uint64_t BISHOP_TABLE[] = {\n")
    for bb in BISHOP_TABLE_FLAT:
        f.write(f"  0x{bb:016X}ULL,\n")
    f.write("};\n\n")

    f.write("constexpr int BISHOP_OFFSETS[] = {\n")
    for off in BISHOP_OFFSETS:
        f.write(f"    {off},\n")
    f.write("};\n")

