#include "game.hpp"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <time.h>

Game::Game(GameIO* game_io) 
    : io(game_io), current_player(BLACK), game_over(false) {
    board.initialize();
    strcpy(error_message, "No error.");
}

void Game::initializeGame() {
    current_player = BLACK; 
    game_over = false;
    move_history.clear();
}

Color Game::determineWinner() const {
    if (board.getRedCount() == 0) return BLACK;
    if (board.getBlackCount() == 0) return RED;
    
    return BLACK;
}

bool Game::isGameOver() const {
    if (board.getRedCount() == 0 || board.getBlackCount() == 0) {
        return true;
    }
    return false;
}

bool Game::handlePlayerTurn() {
    Move m = {};
    strcpy(error_message, "No error.");
    Position multi_jump_source = {-1, -1};
    
    while (true) {
        if (multi_jump_source.is_valid()) {
            io->displayBoard(board, "Multi-jump required!", error_message);
        } else {
            io->displayBoard(board, "Player's turn.", error_message);
        }
        
        if (!io->getPlayerMove(current_player, &m)) {
            strcpy(error_message, "Invalid input format or position. Use 'B3 to A4'.");
            continue;
        }

        if (m.from.row == -1) { 
            return false;
        }

        if (multi_jump_source.is_valid()) {
            if (!(m.from == multi_jump_source)) {
                strcpy(error_message, "You must continue jumping with the active piece.");
                continue;
            }
        }

        MoveError error = board.isLegalMove(m);
        if (error == NO_ERROR) {
            bool is_jump = (abs(m.to.row - m.from.row) == 2);

            if (multi_jump_source.is_valid()) {
                m.is_multijump = true;
            }

            if (board.movePiece(m)) {
                move_history.push_back(m);

                if (is_jump && board.canJump(m.to)) {
                    multi_jump_source = m.to;
                    strcpy(error_message, "Multi-jump available! You must jump again.");
                    move_history.back().is_multijump = true;
                    continue;
                }

                return true;
            } else {
                strcpy(error_message, "Internal error during move execution.");
                continue;
            }
        } else {
            m.to_char();
            snprintf(error_message, sizeof(error_message), "Move %s is illegal (Error %d). Try again.", m.move_string, error);
            continue;
        }
    }
}

bool Game::handleMachineTurn() {
    strcpy(error_message, "No error.");

    
    io->displayBoard(board, "RED's turn.", error_message);
    printf("\nRED is thinking...\n");
    
    
    clock_t start = clock();
    double delay_seconds = 2.0;
    while ((double)(clock() - start) / CLOCKS_PER_SEC < delay_seconds) {
        io->processEvents();
    }
    
    Position current_piece_pos = {-1, -1};
    bool turn_finished = false;
    bool made_move = false;

    while (!turn_finished) {
        bool move_found = false;

        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                Position from = {r, c};
                
                if (current_piece_pos.is_valid()) {
                    if (!(from == current_piece_pos)) continue;
                }

                const Piece* piece = board.getPiece(from);

                if (piece != NULL && piece->getColor() == current_player) {
                    for (int dr_sign = -1; dr_sign <= 1; dr_sign += 2) {
                        for (int dc_sign = -1; dc_sign <= 1; dc_sign += 2) {
                            
                            int steps[] = {2, 1};
                            for (int step : steps) {
                                int dr = dr_sign * step;
                                int dc = dc_sign * step;
                                Move m = {from, {r + dr, c + dc}, current_player, false, ""};

                                if (m.to.is_valid() && board.isLegalMove(m) == NO_ERROR) {
                                    if (board.movePiece(m)) {
                                        if (current_piece_pos.is_valid()) {
                                            m.is_multijump = true;
                                        }
                                        move_history.push_back(m);
                                        made_move = true;
                                        move_found = true;

                                        if (step == 2 && board.canJump(m.to)) {
                                            current_piece_pos = m.to;
                                            move_history.back().is_multijump = true;
                                        } else {
                                            turn_finished = true;
                                        }
                                        goto end_search;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        end_search:
        if (!move_found) {
             turn_finished = true;
             if (!made_move) game_over = true;
        }
    }
    return true; 
}

void Game::run() {
    initializeGame();
    printf("Game started. BLACK moves first. Type 'stop' to exit.\n");

    while (!game_over) {
        bool continue_game = true;
        
        if (current_player == BLACK) {
            continue_game = handlePlayerTurn();
        } else {
            continue_game = handleMachineTurn();
        }

        if (!continue_game) {
            break;
        }

        io->displayHistory(move_history);
        
        if (game_over) {
            continue;
        }
        
        if (isGameOver()) {
            game_over = true;
            continue;
        }

        current_player = (current_player == BLACK) ? RED : BLACK;
    }

    io->displayBoard(board, "Game Over!", error_message);
    
    if (board.getRedCount() == 0) {
        printf("\nGAME OVER: BLACK Wins!\n");
    } else if (board.getBlackCount() == 0) {
        printf("\nGAME OVER: RED Wins!\n");
    } else {
        printf("\nGAME ENDED EARLY\n");
    }

    io->displayHistory(move_history);
    io->waitForContinuation();
}