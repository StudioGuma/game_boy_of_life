/*
Game (Boy) of Life — Game Boy port of Conway's Game of Life
Copyright (C) 2024 StudioGuma

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <gb/gb.h>

#include "hUGEDriver.h"

#include "bkg_tiles.h"

#define DEAD 0
#define ALIVE 1

const uint8_t INIT_SCREEN[0b10100][0b10010] = {
	{0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0},
	{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0},
	{0,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,0},
	{0,0,0,0,0,1,0,1,0,1,1,0,0,1,1,0,1,0},
	{0,0,0,0,0,1,0,1,0,1,1,1,1,1,1,0,1,0},
	{0,0,0,0,0,1,0,1,0,1,1,1,1,1,1,0,1,0},
	{0,0,0,0,0,0,0,1,0,1,1,0,0,1,1,0,1,0},
	{0,1,0,0,0,1,0,1,0,1,1,1,1,1,1,0,1,0},
	{0,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,1,0},
	{0,1,0,0,0,1,0,0,1,1,1,1,1,1,1,1,0,0},
	{0},
	{0,1,1,1,1,1,0,0,0,0,0,1,0,0,1,0,0,0},
	{0,1,0,1,0,0,0,0,0,0,1,0,1,0,1,0,0,0},
	{0,1,0,1,0,0,0,0,0,0,1,0,1,0,1,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0},
	{0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0},
	{0,1,0,1,0,1,0,0,0,0,0,1,1,1,0,0,0,0},
	{0,1,0,1,0,1,0,0,0,0,1,0,0,0,1,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,0,0,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0}
};

struct cell {
	uint8_t state, neighbors;
};

struct cell board[0b10100][0b10010];
uint8_t joy, joy_prev, select_x, select_y, automate, next[0b10100][0b10010], timer;

extern const hUGESong_t MUS_LIFE;

void step(void)
{
	for (uint8_t x = 0; x < 0b10100; x++) {
		for (uint8_t y = 0; y < 0b10010; y++) {
			switch (x) {
			case 0:
				switch (y) {
				case 0: // Top left corner
					board[x][y].neighbors = board[x + 1][y + 1].state + board[x + 1][y].state +
					board[x + 1][0b10001].state + board[x][0b10001].state +
					board[0b10011][0b10001].state + board[0b10011][y].state +
					board[0b10011][y + 1].state + board[x][y + 1].state;
					break;

				case 0b10001: // Bottom left corner
					board[x][y].neighbors = board[x + 1][0].state + board[x + 1][y].state +
					board[x + 1][y - 1].state + board[x][y - 1].state +
					board[0b10011][y - 1].state + board[0b10011][y].state +
					board[0b10011][0].state + board[x][0].state;
					break;

				default: // Left edge
					board[x][y].neighbors = board[x + 1][y + 1].state + board[x + 1][y].state +
					board[x + 1][y - 1].state + board[x][y - 1].state +
					board[0b10011][y - 1].state + board[0b10011][y].state +
					board[0b10011][y + 1].state + board[x][y + 1].state;
				}
				break;

			case 0b10011:
				switch (y) {
				case 0: // Top right corner
					board[x][y].neighbors = board[0][y + 1].state + board[0][y].state +
					board[0][0b10001].state + board[x][0b10001].state +
					board[x - 1][0b10001].state + board[x - 1][y].state +
					board[x - 1][y + 1].state + board[x][y + 1].state;
					break;

				case 0b10001: // Botoom right corner
					board[x][y].neighbors = board[0][0].state + board[0][y].state +
					board[0][y - 1].state + board[x][y - 1].state +
					board[x - 1][y - 1].state + board[x - 1][y].state +
					board[x - 1][0].state + board[x][0].state;
					break;

				default: // Right edge
					board[x][y].neighbors = board[0][y + 1].state + board[0][y].state +
					board[0][y - 1].state + board[x][y - 1].state +
					board[x - 1][y - 1].state + board[x - 1][y].state +
					board[x - 1][y + 1].state + board[x][y + 1].state;
				}
				break;

			default:
				switch (y) {
				case 0: // Top edge
					board[x][y].neighbors = board[x + 1][y + 1].state + board[x + 1][y].state +
					board[x + 1][0b10001].state + board[x][0b10001].state +
					board[x - 1][0b10001].state + board[x - 1][y].state +
					board[x - 1][y + 1].state + board[x][y + 1].state;
					break;

				case 0b10001: // Bottom edge
					board[x][y].neighbors = board[x + 1][0].state + board[x + 1][y].state +
					board[x + 1][y - 1].state + board[x][y - 1].state +
					board[x - 1][y - 1].state + board[x - 1][y].state +
					board[x - 1][0].state + board[x][0].state;
					break;

				default: // All other cells
					board[x][y].neighbors = board[x + 1][y + 1].state + board[x + 1][y].state +
					board[x + 1][y - 1].state + board[x][y - 1].state +
					board[x - 1][y - 1].state + board[x - 1][y].state +
					board[x - 1][y + 1].state + board[x][y + 1].state;
				}
			}

			if (board[x][y].neighbors == 0b11 || (board[x][y].state == ALIVE && board[x][y].neighbors == 0b10))
				next[x][y] = ALIVE;
			else
				next[x][y] = DEAD;
		}
	}

	for (uint8_t x = 0; x < 0b10100; x++) {
		for (uint8_t y = 0; y < 0b10010; y++) {
			if (board[x][y].state != next[x][y]) {
				board[x][y].state = next[x][y];
				set_bkg_tile_xy(x, y, board[x][y].state);
			}
		}
	}
}

uint8_t select_x_update(uint8_t sx)
{
	if (sx > 0b10000000)
		sx = 0b10011;
	if (sx > 0b10011 && sx < 0b10000000)
		sx = 0;

	return sx;
}

uint8_t select_y_update(uint8_t sy)
{
	if (sy > 0b10000000)
		sy = 0b10001;
	if (sy > 0b10001 && sy < 0b10000000)
		sy = 0;

	return sy;
}

void main(void)
{
	rAUDENA = AUDENA_ON;
	rAUDTERM = 0b11111111;
	rAUDVOL = 0b1110111;

	set_bkg_1bpp_data(0, 0b100, BKG_TILES);

	for (uint8_t x = 0; x < 0b10100; x++) {
		for (uint8_t y = 0; y < 0b10010; y++) {
			board[x][y].state = INIT_SCREEN[x][y];
			set_bkg_tile_xy(x, y, board[x][y].state);
		}
	}

	set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state + 0b10);

	hUGE_init(&MUS_LIFE);

	SHOW_BKG;

	for (;;) { // Forever
		joy_prev = joy;
		joy = joypad();

		if ((joy & J_B) && !(joy_prev & J_B)) {
			automate ^= 1; // Toggle auto mode

			if (automate)
				set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state);
			else
				set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state + 0b10);
			
			rAUDTERM ^= 0b11111111; // Toggle sound
		}

		if (automate) {
			step();
		} else {
			hUGE_dosound();

			if (joy & J_RIGHT || joy & J_LEFT || joy & J_UP || joy & J_DOWN) {
				if (!(joy_prev & J_RIGHT || joy_prev & J_LEFT || joy_prev & J_UP || joy_prev & J_DOWN))
					timer = 0;

				if (timer > 0) {
					timer--;
				} else {
					timer = 0b100;

					if (joy & J_RIGHT) {
						set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state);
						select_x = select_x_update(++select_x);
						set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state + 0b10);
					}
					if (joy & J_LEFT) {
						set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state);
						select_x = select_x_update(--select_x);
						set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state + 0b10);
					}
					if (joy & J_UP) {
						set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state);
						select_y = select_y_update(--select_y);
						set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state + 0b10);
					}
					if (joy & J_DOWN) {
						set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state);
						select_y = select_y_update(++select_y);
						set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state + 0b10);
					}
				}
			}

			if ((joy & J_A) && !(joy_prev & J_A)) {
				board[select_x][select_y].state ^= 1; // Toggle selected cell
				set_bkg_tile_xy(select_x, select_y, board[select_x][select_y].state + 0b10);

				if (board[select_x][select_y].state) { // Dead to alive
					rAUD1SWEEP = 0b100101;
					rAUD1LEN = 0b1000000;
					rAUD1ENV = 0b11110100;
					rAUD1LOW = 0b10000000;
					rAUD1HIGH = 0b10000110;
				} else { // Alive to dead
					rAUD1SWEEP = 0b101101;
					rAUD1LEN = 0b1000000;
					rAUD1ENV = 0b11110010;
					rAUD1LOW = 0b100000;
					rAUD1HIGH = 0b10000111;
				}
			}

			if ((joy & J_START) && !(joy_prev & J_START)) {
				for (uint8_t x = 0; x < 0b10100; x++) {
					for (uint8_t y = 0; y < 0b10010; y++) {
						if (board[x][y].state == ALIVE) {
							board[x][y].state = DEAD; // Reset board
							set_bkg_tile_xy(x, y, board[x][y].state);
						}
					}
				}

				rAUD1SWEEP = 0b110111;
				rAUD1LEN = 0b1010;
				rAUD1ENV = 0b11111011;
				rAUD1LOW = 0b1000000;
				rAUD1HIGH = 0b10000110;
			}
		}

		vsync();
	}
}
