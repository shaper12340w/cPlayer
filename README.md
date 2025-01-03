

# cPlayer - Console-based MP3 Music Player

**cPlayer** is a console-based MP3 music player built using C++.  
It provides a simple user interface and essential playback features, allowing you to load and play MP3 files from individual files or entire folders.

---

## Features

- **MP3 Playback**: Load MP3 files from individual files or entire folders for seamless playback.
- **Basic Controls**:
  - Play/Pause, Next Track, Previous Track.
  - Shuffle mode and Repeat mode support.
- **User Input**:
  - Mouse support for progress bar control and track selection.
  - Keyboard shortcuts for fast navigation and control.
- **File Management**:
  - ID3 tag removal from MP3 files.
  - Temporary file creation and hidden file handling.

---

## Usage

1. **Run the Program**  
   Execute the built executable file.

2. **Add Files or Folders**  
   - Press `o`: Open the file dialog to add MP3 files.
   - Press `p`: Open the folder dialog to add all MP3 files in a folder.

3. **Playback Control**  
   - Press the Spacebar: Play/Pause the current track.
   - Press `,`: Play the previous track.
   - Press `.`: Play the next track.
   - Press `s`: Toggle Shuffle mode.
   - Press `r`: Toggle Repeat mode.

4. **Exit**  
   The program automatically cleans up temporary files on exit.

---

## Development Environment

- **Language**: C++
- **Platform**: Windows
- **Build Tools**: Supports CLion, MinGW, and Visual Studio
- **Required Libraries**:
  - `winmm.lib`: Windows Multimedia API
  - `windows.h`, `mmsystem.h`: Windows API headers

---

## Project Structure

```
cPlayer/
│
├── src/
│   ├── main.cpp            # Program entry point
│   ├── musicPlayer.cpp     # Music playback management (using MCI)
│   ├── fileManager.cpp     # File and folder management
│   ├── playerUI.cpp        # Console UI handling
│   ├── manageConsole.cpp   # Console window and input management
│   ├── dynamicArray.cpp    # Dynamic array utilities
│
├── include/
│   ├── musicPlayer.h       # Music playback header
│   ├── fileManager.h       # File management header
│   ├── playerUI.h          # UI management header
│   ├── manageConsole.h     # Console management header
│   ├── dynamicArray.h      # Dynamic array header
│
└── README.md               # Project documentation
```

---

## Keyboard Shortcuts

| Key           | Action                  |
|---------------|-------------------------|
| `o`           | Open file dialog        |
| `p`           | Open folder dialog      |
| `Spacebar`    | Play/Pause playback     |
| `,`           | Play the previous track |
| `.`           | Play the next track     |
| `s`           | Toggle Shuffle mode     |
| `r`           | Toggle Repeat mode      |

---

## Contribution

This project is open-source. If you'd like to contribute by reporting issues, improving the code, or adding features, please use [Issues](https://github.com/USERNAME/cPlayer/issues) or [Pull Requests](https://github.com/USERNAME/cPlayer/pulls).

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

---

### Enhancements

1. **Add Visuals**: Include a logo or ASCII art banner at the top.
2. **Include GIF or Screenshot**: Showcase the console UI and the player in action.
3. **Provide Example Commands**: For developers, add examples on how to compile and run the program.
