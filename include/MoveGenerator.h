//
// Created by Mercury on 23/02/2026.
//

#ifndef CHESS_ENGINE_MOVEGENERATOR_H
#define CHESS_ENGINE_MOVEGENERATOR_H
#include <format>
#include <random>
#include <vector>

#include "Board.h"
#include "Evaluator.h"
#include "Move.h"
#include "TranspositionTable.h"

/*
 * getBestMove returns the best move found by the engine
 *
 * generateMoves should probably evaluate moves as they are generated, as opposed
 * to to once all the moves have been generated, as otherwise we have to go
 * through another loop
 *
 * generateMoves works as follows
 * generate 1 possible move
 * generate 1 possible move from there
 * once we have reached depth
 * backpropogate the evaluator result from there
 * So we obviously need to return the evaluators result
 * But we also need to remember the first move that started it
 * getBestMove stores the current best move and its corresponding score
 * generateMoves just returns evaluator results
 */

class MoveGenerator {
    public:
        MoveGenerator();
        Move getBestMove(Board& board, int depth);
        static bool isSquareAttacked(Board& board, uint64_t bitboardPosition, Color color);
        static uint64_t getPawnAttacksForSquare(Board& board, uint64_t square, Color color);
        void generateLegalMoves(std::vector<Move>& outMoves, Board& board, bool onlyCaptures);
    private:
        TranspositionTable transpositionTable;
        std::mt19937 rng;
        int INF = 1'000'000;
        int MATE_SCORE = 900'000;
        void orderMoves(Board& board, std::vector<Move>& moves);
        void generatePseudoLegalMoves(std::vector<Move>& moves, Board& board);
        int negaMax(Board& board, int depth, int alpha, int beta);
        int quiescence(Board& board, int alpha, int beta, int depth);

        std::vector<Move> generatePawnMoves(const Board& board);

        std::vector<Move> generateKnightMoves(const Board& board);
        static uint64_t getKnightAttacksForSquare(uint64_t bitboardPosition);

        std::vector<Move> generateBishopMoves(Board& board);
        static uint64_t getBishopAttacksForSquare(const Board& board, uint64_t bishopSquare);

        std::vector<Move> generateRookMoves(Board& board);
        static uint64_t getRookAttacksForSquare(const Board& board, uint64_t rookSquare);

        std::vector<Move> generateQueenMoves(Board& board);
        static uint64_t getQueenAttacksForSquare(const Board& board, uint64_t square);

        std::vector<Move> generateKingMoves(Board& board);
        static uint64_t getKingAttacksForSquare(Board& board, uint64_t square);

        static std::vector<Move> getPawnPromotions(const Board& board, uint64_t startSquare, uint64_t endSquare);

        void addMove(const Board& board, std::vector<Move>& moves, Move move);

        Evaluator evaluator;
        static constexpr std::array<uint64_t, 64> preComputedKnightMoves= {
            132096ULL, 329728ULL, 659712ULL, 1319424ULL, 2638848ULL, 5277696ULL, 10489856ULL, 4202496ULL,
            33816580ULL, 84410376ULL, 168886289ULL, 337772578ULL, 675545156ULL, 1351090312ULL, 2685403152ULL, 1075839008ULL,
            8657044482ULL, 21609056261ULL, 43234889994ULL, 86469779988ULL, 172939559976ULL, 345879119952ULL, 687463207072ULL, 275414786112ULL,
            2216203387392ULL, 5531918402816ULL, 11068131838464ULL, 22136263676928ULL, 44272527353856ULL, 88545054707712ULL, 175990581010432ULL, 70506185244672ULL,
            567348067172352ULL, 1416171111120896ULL, 2833441750646784ULL, 5666883501293568ULL, 11333767002587136ULL, 22667534005174272ULL, 45053588738670592ULL, 18049583422636032ULL,
            145241105196122112ULL, 362539804446949376ULL, 725361088165576704ULL, 1450722176331153408ULL, 2901444352662306816ULL, 5802888705324613632ULL, 11533718717099671552ULL, 4620693356194824192ULL,
            288234782788157440ULL, 576469569871282176ULL, 1224997833292120064ULL, 2449995666584240128ULL, 4899991333168480256ULL, 9799982666336960512ULL, 1152939783987658752ULL, 2305878468463689728ULL,
            1128098930098176ULL, 2257297371824128ULL, 4796069720358912ULL, 9592139440717824ULL, 19184278881435648ULL, 38368557762871296ULL, 4679521487814656ULL, 9077567998918656ULL
        };
};


#endif //CHESS_ENGINE_MOVEGENERATOR_H