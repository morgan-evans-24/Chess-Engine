//
// Created by Mercury on 24/02/2026.
//

#include "../include/MoveParser.h"

#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>

#include "../include/Zobrist.h"
#include "../Enums/Piece.h"
#include "../include/BoardManager.h"
#include "../include/DebugUtils.h"

using std::string, std::stringstream;

std::unordered_map<char, Piece> MoveParser::charToPieceMap {
        {'P', Piece::WHITE_PAWN},
        {'N', Piece::WHITE_KNIGHT},
        {'B', Piece::WHITE_BISHOP},
        {'R', Piece::WHITE_ROOK},
        {'Q', Piece::WHITE_QUEEN},
        {'K', Piece::WHITE_KING},
        {'p', Piece::BLACK_PAWN},
        {'n', Piece::BLACK_KNIGHT},
        {'b', Piece::BLACK_BISHOP},
        {'r', Piece::BLACK_ROOK},
        {'q', Piece::BLACK_QUEEN},
        {'k', Piece::BLACK_KING},
    };

void MoveParser::parseFenString(Board& board, const string& FENString) {

    board.clearBoard();

    stringstream ss = stringstream(FENString);
    string temp;
    char delimiter = ' ';

    // Board Position
    getline(ss, temp, delimiter);
    int numThingsInLine = 0;
    int boardPos = 56;
    for (char c : temp) {
        if (isdigit(c)) {
            boardPos += c - '0';
            numThingsInLine += c - '0';
        } else if (c == '/') {
            numThingsInLine = 0;
            continue;
        } else {
            Piece piece = charToPieceMap.at(c);
            board.setBoardSquare(boardPos, piece);
            board.zobristHash ^= Zobrist::pieceKeys[static_cast<int>(piece)][boardPos];
            boardPos++;
            numThingsInLine++;
        }
        if (numThingsInLine == 8) {
            boardPos -= 16;
        }
    }

    // active colour
    getline(ss, temp, delimiter);
    if (temp == "w") {
        board.whoseTurn = Color::WHITE;
    } else if (temp == "b") {
        board.whoseTurn = Color::BLACK;
        board.zobristHash ^= Zobrist::blackToMoveKey;
    }

    // castling availability
    int& castlingRights = board.castlingRights;
    castlingRights = 0;
    getline(ss, temp, delimiter);
    for (char c : temp) {
        switch (c) {
            case 'K': castlingRights |= 1 << 3; break;
            case 'Q': castlingRights |= 1 << 2; break;
            case 'k': castlingRights |= 1 << 1; break;
            case 'q': castlingRights |= 1; break;
            default: break;
        }
    }
    board.zobristHash ^= board.castlingRights;

    // En-passant target square
    getline(ss, temp, delimiter);
    int file = temp[0] - 'a';
    int rank = temp[1] - '0' - 1;
    uint64_t enPassantSquare = 1ULL;
    enPassantSquare = enPassantSquare << file;
    enPassantSquare = enPassantSquare << (8 * rank);
    board.enPassantSquare = enPassantSquare;
    board.zobristHash ^= Zobrist::enPassantKeys[8 * rank + file];

    // Halfmove clock
    getline(ss, temp, delimiter);
    BoardManager::halfMoveClock = std::stoi(temp);
    // Full move clock
    getline(ss, temp, delimiter);
    BoardManager::fullMoveClock = std::stoi(temp);

    board.zobristHistory.push_back(board.zobristHash);
}

void MoveParser::parseMove(Board& board, const std::string& moveString) {
    // Step one, split string
    char startFile = moveString[0];
    int startRank = moveString[1] - '0';
    int startSquare = (startRank-1) * 8 + startFile - 'a';
    uint64_t startBB = 1ULL << startSquare;

    char endFile = moveString[2];
    int endRank = moveString[3] - '0';
    int endSquare = (endRank-1) * 8 + endFile - 'a';
    uint64_t endBB = 1ULL << endSquare;

    Piece movePiece = board.getPieceAtSquare(startBB);
    Piece promotionPiece;

    auto move = Move(startBB, endBB, board.getPieceAtSquare(startBB));



    // Move is a promotion
    if (moveString.length() > 4) {
        move.isPromotion = true;

        const bool isWhite = board.whoseTurn == Color::WHITE;

        switch (moveString[4]) {
            case 'q':
                move.promotionType = isWhite ? Piece::WHITE_QUEEN : Piece::BLACK_QUEEN;
                break;
            case 'r':
                move.promotionType = isWhite ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;
                break;
            case 'b':
                move.promotionType = isWhite ? Piece::WHITE_BISHOP : Piece::BLACK_BISHOP;
                break;
            case 'n':
                move.promotionType = isWhite ? Piece::WHITE_KNIGHT : Piece::BLACK_KNIGHT;
                break;
            default:
                throw std::runtime_error("Invalid promotion piece");
        }
    }

    if (isPawn(movePiece) & (endBB == board.enPassantSquare)) {
        move.enPassant = true;
        move.isCapture = true;
    }

    // Move is a capture
    if ((board.getAllPieces() & endBB) > 0) {
        move.isCapture = true;
    }

    if ((moveString == "e1g1" || moveString == "e8g8") & isKing(board.getPieceAtSquare(startBB))) {
        move.isShortCastling = true;
    }

    if ((moveString == "e1c1" || moveString == "e8c8") & isKing(board.getPieceAtSquare(startBB))) {
        move.isLongCastling = true;
    }

    board.makeMove(move);
    board.zobristHistory.push_back(board.zobristHash);
    DebugUtils::printBoardState(board);
}

