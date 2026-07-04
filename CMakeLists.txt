cmake_minimum_required(VERSION 4.0)
project(Chess_Engine)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")

add_executable(Chess_Engine src/main.cpp
        src/UCIManager.cpp
        include/UCIManager.h
        src/BoardManager.cpp
        include/BoardManager.h
        src/Move.cpp
        include/Move.h
        Enums/Piece.h
        Enums/Color.h
        src/MoveGenerator.cpp
        include/MoveGenerator.h
        src/Board.cpp
        include/Board.h
        src/Evaluator.cpp
        include/Evaluator.h
        src/RuleChecker.cpp
        include/RuleChecker.h
        src/MoveParser.cpp
        include/MoveParser.h
        src/DebugUtils.cpp
        include/DebugUtils.h
        include/rook_table.h
        src/rook_table.cpp
        include/bishop_table.h
        src/bishop_table.cpp
        src/UCILogger.cpp
        include/UCILogger.h
        src/Zobrist.cpp
        include/Zobrist.h
        src/TranspositionTable.cpp
        include/TranspositionTable.h
        src/OpeningBook.cpp
        include/OpeningBook.h
)
