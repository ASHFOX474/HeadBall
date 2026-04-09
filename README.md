# 🏆 HeadBall

A fast-paced **2-player local multiplayer** head soccer game built in C++ using the iGraphics (OpenGL/GLUT) framework. Two players face off on the same keyboard — jump, dash, and kick your way to victory!

---

## ✨ Features

- 🎮 **2-Player Local Multiplayer** — both players share one keyboard
- ⚽ **Realistic Ball Physics** — gravity, bounce restitution, and ground friction
- 🥅 **Goal Bar Collision** — the ball bounces off the crossbar just like the real thing
- 🏃 **Sprite Animations** — idle, run, jump, fall, and kick animations for each player
- 🕐 **Two Game Modes** — 3-minute timed match or first to 5 goals
- 📋 **Match History** — last 5 game results saved automatically to disk
- 🔊 **Sound System** — background music and in-game sound effects with adjustable volume
- ⚙️ **Settings Menu** — control BGM and SFX volume independently
- 📝 **Player Naming** — enter custom names before each match

---

## 🕹️ Controls

### Player 1
| Key | Action |
|-----|--------|
| `A` | Move Left |
| `D` | Move Right |
| `W` | Jump |
| `S` | Fast Fall (while airborne) |
| `F` | Kick |

### Player 2
| Key | Action |
|-----|--------|
| `←` Arrow | Move Left |
| `→` Arrow | Move Right |
| `↑` Arrow | Jump |
| `↓` Arrow | Fast Fall (while airborne) |
| `/` | Kick |

### Global
| Key | Action |
|-----|--------|
| `M` | Return to Main Menu |
| `End` | Quit Game |

---

## 🎯 Game Modes

| Mode | Description |
|------|-------------|
| **3-Minute Match** | The player with the most goals when time runs out wins |
| **Score Mode** | First player to score **5 goals** wins |

---

## 🚀 Installation (Windows)

No build tools or dependencies required — just download and play!

**1. Go to the Releases page**

👉 [Releases](https://github.com/ASHFOX474/HeadBall/releases/tag/v1.0)

**2. Download `Headball.zip`**

Click on `Headball.zip` and then click the **Download** button (or the raw file icon).

**3. Extract the ZIP**

Right-click `Headball.zip` → **Extract All...** → choose a destination folder.

**4. Run the game**

Open the extracted folder and double-click **`HeadBall.exe`** to launch the game.

> ⚠️ **Do not move `HeadBall.exe` out of the extracted folder.** The game loads assets (images, sounds) from relative paths — all files must stay together.

---

## 🗂️ Project Structure

```
HeadBall/
├── iMain.cpp               # Main source file
├── iGraphics.h             # iGraphics rendering library
├── iSound.h                # iSound audio library
└── assets/
    ├── images/
    │   ├── bg.png              # Game background
    │   ├── Ball 01.png         # Loading screen ball
    │   ├── Ball 02.png         # In-game ball
    │   ├── score_board1.png    # Scoreboard UI
    │   ├── Loading_Screen_1.jpg
    │   └── Goal/               # Goal animation frames (001–021)
    └── sounds/
        ├── bgm.WAV             # Background music
        ├── Goal.WAV            # Goal cheer sound effect
        └── kick.WAV            # Kick sound effect
```

---

## 🛠️ Building from Source

> Only needed if you want to modify the code.

**Requirements:**
- Windows (MSVC or MinGW/GCC)
- OpenGL & GLUT libraries (bundled with iGraphics)

**Steps:**
1. Clone the repository
   ```bash
   git clone https://github.com/ASHFOX474/HeadBall.git
   cd HeadBall
   ```
2. Open the project in your IDE (e.g., Code::Blocks or Visual Studio) and build `iMain.cpp` together with the iGraphics library files.
3. Make sure the `assets/` folder is in the same directory as the compiled executable.

---

## 👥 Authors

- **Md. Ashraf Hossain Chowdhury** — [GitHub](https://github.com/ASHFOX474)
- **Lokonath Basak Bijoy** — [GitHub](https://github.com/GodButcher-Bijoy)

---
