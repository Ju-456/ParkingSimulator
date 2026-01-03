# ParkingSimulator
As its name suggests, this project is a parking simulator. Three modes will be developed: a random mode, a chosen mode, and a surprise mode !

# 🚗 Parking Simulator (C / Raylib)

Parking Simulator is a graphical parking simulation game written in **C**, using the **Raylib** library for rendering and **Parson** for JSON parsing.  
The project simulates a multi-floor parking lot with different game modes, barriers, timers, AI-controlled cars, and replayable simulations.

## Tree 
```
C:.
│   .clang-format
│   .DS_Store
│   .gitignore
│   main.c
│   makefile
│   parkingSimulator
│   README.md
│
├───Assets
│   │   .DS_Store
│   │   background.png
│   │   barrier_wall.png
│   │   busy_parking_place.png
│   │   cars.png
│   │   car_preview.png
│   │   DejaVuSansMono.ttf
│   │   entrance_barrier.png
│   │   entrance_ticket_dispenser.png
│   │   exit_barrier.png
│   │   exit_pay_station.png
│   │   floor_exit.png
│   │   floor_indicator0.png
│   │   floor_indicator1.png
│   │   floor_indicator2.png
│   │   free_parking_place.png
│   │   panel_menu.png
│   │   parking_place.png
│   │   PC.png
│   │
│   ├───black_car
│   │       blackFront.png
│   │       blackRight.png
│   │       blackTop.png
│   │
│   ├───blue_car
│   │       blueFront.png
│   │       blueRight.png
│   │       blueTop.png
│   │
│   ├───gray_car
│   │       grayFront.png
│   │       grayRight.png
│   │       grayTop.png
│   │
│   ├───pink_car
│   │       pinkFront.png
│   │       pinkRight.png
│   │       pinkTop.png
│   │
│   ├───red_car
│   │       redFront.png
│   │       redRight.png
│   │       redTop.png
│   │
│   └───yellow_car
│           yellowFront.png
│           yellowRight.png
│           yellowTop.png
│
├───general
│       parking_algo.c
│       parking_algo.h
│       parson.c
│       parson.h
│       utils.c
│       utils.h
│
├───graphic
│   │   barriers.c
│   │   buttons.c
│   │   floor.c
│   │   hard_and_manual_mode.c
│   │   menu.c
│   │   parking_graphic.c
│   │   random_mode.c
│   │   texture_gestion.c
│   │   window.c
│   │   window.h
│   │
│   └───simdata
│       ├───hard_mode
│       │       simulation_data_0.txt
│       │       simulation_data_1.txt
│       │       simulation_data_2.txt
│       │       simulation_data_3.txt
│       │       simulation_data_4.txt
│       │       simulation_data_5.txt
│       │
│       └───rand_mode
│               simulation_data_0.txt
│               simulation_data_1.txt
│               simulation_data_2.txt
│               simulation_data_3.txt
│               simulation_data_4.txt
│               simulation_data_5.txt
│
└───graph_json
        graph_floor_0.json
        graph_floor_1.json
        graph_floor_2.json
        
```

## 📌 Features

- Multi-floor parking system (3 floors)
- Entrance and exit barriers with automatic opening/closing
- Multiple game modes:
  - **Random Mode** – automatic replay-based simulation 
  - **Manual Mode** – player-controlled parking
  - **Hard Mode** – manual driving with AI cars and traffic
- Countdown timer with penalties and bonuses
- AI cars driven by prerecorded simulations
- Simulation recording & replay system
- Multiple car models and colors
- Parking layout loaded from JSON files
- Floor navigation system
- Ticket & payment system
- Collision detection with walls, cars, and parking places

---

## 🛠 Technologies Used

- **Language**: C
- **Graphics**: [Raylib](https://www.raylib.com/)
- **JSON Parsing**: [Parson](https://github.com/kgabis/parson)
- **Build System**: Makefile
- **Platform**: macOS / Linux (Raylib required)

---


## 🎮 Game Modes

### 🔁 Random Mode
- Automatically replays a previously recorded parking simulation
- Random car selection
- Fixed parked cars per floor
- Timer starts automatically
- Ends when the replay finishes

### 🕹 Manual Mode
- Choose your car
- Drive using arrow keys
- Park manually in a free spot
- Minimum parking time required
- Pay before exiting
- Timer-based challenge

### 🔥 Hard Mode
- Same as Manual Mode
- Additional **AI-controlled cars**
- Increased difficulty with traffic
- AI cars follow prerecorded simulations
- Fixed and dynamic obstacles


---

## 🧠 Parking Logic

- Parking places are loaded from JSON files
- Each place has:
  - Position (x, y)
  - Direction (left/right)
  - Occupation state
- A car is considered parked **only if fully inside** a parking spot
- Collision detection prevents invalid moves

---

## 🕒 Timer System

- Starts automatically in driving modes
- Initial duration: **20 seconds**
- +10 seconds when going up a floor
- Game Over if timer reaches zero
- Stops when the game is finished

---

## 📦 Build & Run

### 🔧 Requirements
- Raylib installed
- GCC compiler
- Make

### ▶️ Compile
```bash
make
```
### ▶️ Run
```bash
./parkingSimulator
```
---
## Conclusion

This project demonstrates:

- Real-time graphics programming in C

- Modular game architecture

- Data-driven design using JSON

- Collision detection and animation handling

- Replay-based simulation and AI behavior

Parking Simulator is designed to be easily extensible, allowing new floors, cars, or game modes to be added with minimal changes.

---
# Enjoy parking! 🚘
