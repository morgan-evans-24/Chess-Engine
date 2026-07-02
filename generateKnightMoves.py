def isSquareValid(square):
    # is a square on the virtual chessboard
    if square > 20 and square < 99 and square % 10 != 0 and square % 10 != 9:
        return True
    else:
        return False

def convertToRealSquare(fakeSquare):
    # Convert a square from the 0..119 square space to 0..64
    rank = fakeSquare // 10 - 2
    return (fakeSquare - 21) - (rank * 2)

def convertListSquaresToBinary(moveList):
    # Given a list of squares, convert that list to a bit map
    # i.e., set everything to 0, flip those bits
    output = 0
    for move in moveList:
        output |= (1 << move)
    print(output,"ULL,", end=" ", sep="")

moveDirections = [12, 21, 19, 8, -12, -21, -19, -8]

currentMoves = []
newLineCounter = 0
for i in range(120):
    if isSquareValid(i):
        for j in moveDirections:
            if isSquareValid(i+j):
                currentMoves.append(convertToRealSquare(i+j))
        convertListSquaresToBinary(currentMoves)
        newLineCounter += 1
        if newLineCounter == 8:
            print()
            newLineCounter = 0
        currentMoves = []
