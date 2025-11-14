# Chapter 7 Exercises (CPP implementations)

Files added under `chap7/`:

- `ex12_gradebook.cpp` — Grade Book (5 students x 4 tests). Prompts for names and scores, prints averages and letter grades.
- `ex16_worldseries.cpp` — Counts World Series wins for a team using `chap7/data_files/Teams.txt` and `WorldSeriesWinners.txt`.
- `ex18_tictactoe.cpp` — Two-player Tic-Tac-Toe on a 3x3 board (console input).
- `ex19_magic8ball.cpp` — Magic 8-Ball reading responses from `chap7/data_files/8_ball_responses.txt`.
- `ex20_gas1994.cpp` — Reads 52 weekly averages from `chap7/data_files/1994_Weekly_Gas_Averages.txt` and reports min/max/monthly averages.
- `ex22_theater.cpp` — Theater seating skeleton (15 rows x 30 seats) with row pricing and seat purchases.
- `DisplayMenu.cpp` and `ex102_menu.cpp` — simple menu driver (illustrative) to show how to run exercises.

Build with this:

```bash
g++ -std=c++17 ex12_gradebook.cpp -o ex12
g++ -std=c++17 ex16_worldseries.cpp -o ex16
g++ -std=c++17 ex18_tictactoe.cpp -o ex18
g++ -std=c++17 ex19_magic8ball.cpp -o ex19
g++ -std=c++17 ex20_gas1994.cpp -o ex20
g++ -std=c++17 ex22_theater.cpp -o ex22
g++ -std=c++17 ex102_menu.cpp DisplayMenu.cpp -o menu
```

```bash
./menu
```