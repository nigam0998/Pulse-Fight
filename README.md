# Pulse-Fight (OpenFight)

Pulse-Fight is a fast-paced, open-source 2D fighting game written in C++ utilizing SDL and OpenGL. Inspired by classic arcade fighting games, it features a custom game engine with smooth animations, hitbox-based collision detection, basic AI opponents, and an engaging robust fighting system.

![openfight](./images/openfight-1.png)
*(Screenshots represent the ongoing development of the game)*

## 🎮 Features
- **Custom Game Engine:** Built from scratch in C++ for maximum performance.
- **Fluid Combat:** Implemented jumping, crouching, dashing, blocking, and specialized moves.
- **AI Opponents:** Features basic AI logic for challenging single-player modes.
- **YAML Configurations:** Easy tweaking of character stats, hitboxes, and environments using YAML files.
- **Cross-Platform Potential:** Relies on CMake and SDL, abstracting operating system layers.

## 🛠️ Requirements
Ensure you have the following packages installed before building the project:
* `cmake`
* `libyaml-cpp-dev`
* `libsdl2-dev`, `libsdl2-image-dev`
* `libglu1-mesa-dev`, `libglew-dev`

## 🚀 How to Run

The project can be built and executed easily. On Linux/Ubuntu, simply run the following script:

```bash
make ubuntu-install-deps build
./openfight
```

For Windows build, make sure CMake and a C++ compiler (like MSVC or MinGW) are properly configured.

## 👥 Team Members & Roles

* **Devyansh Nigam (23BCS13022)** - Core Developer & Project Lead (Engine Architecture, Game Mechanics, collision detection)
* **Gautam Kumar (23BCS13028)** - Core Developer (Graphics Rendering, Animation Management, Input integration)

## 🤝 Contributing
Pull requests are welcome! If you find a bug or have a feature suggestion, feel free to open an issue.

## 🛣️ Roadmap
Check out the [ROADMAP](./ROADMAP.md) for planned features and upcoming improvements.
