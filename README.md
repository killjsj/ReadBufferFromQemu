# ReadBufferFromQEMU
Warn:This Readme is made by ai
Warn:You need to use a speical qemu located at:`https://github.com/killjsj/qemu` to have a ui called `sharedbuffer`

path for place custom qemu: in build version, create a folder called 'bin' in the game folder and throw all qemu build inside,
in editor,create a folder called 'bin' in the project folder and throw all qemu build inside

A Godot 4.x C++ extension that captures QEMU virtual machine screen output via shared memory.

## Features

- Reads screen buffer directly from QEMU's shared memory display
- Supports multiple virtual screens
- Cross-platform (Windows, Linux)
- Real-time frame updating via threading
- RGBA format conversion for various pixel formats

## Requirements

- Godot 4.x with C++ support
- QEMU built with `--display sharedbuffer` support
- For Windows: Visual Studio 2017+ or MinGW-w64
- For Linux: GCC with pthread support

## Project Structure

```
ReadBufferFromQemu/
├── src/
│   ├── read.h          # Main ReaderClass header
│   ├── read.cpp       # Implementation
│   ├── utils.h       # Shared memory utilities
│   ├── utils.cpp     # QEMU process management
│   ├── lock.h       # Cross-platform mutex
│   ├── structDefine.h # Shared memory structures
│   └── register_types.cpp # Godot extension registration
├── project/          # Godot test project
│   └── node_2d.gd   # Demo usage script
└── bin/             # Compiled libraries
```

## Usage

### GDScript Usage

```gdscript
extends ReaderClass

var screen_sprites: Array[Sprite2D] = []

func _ready() -> void:
    startMachine(['-m', '2048'])

func _process(_delta: float) -> void:
    sync_connection(_delta)
    var count = get_screen_count()

    if count > 0 and screen_sprites.size() != count:
        _setup_sprites(count)

    for i in range(screen_sprites.size()):
        if get_usable(i):
            screen_sprites[i].texture = get_texture(i)
```

### API Reference

| Method | Description |
|--------|------------|
| `startMachine(args: PackedStringArray)` | Start QEMU with given arguments |
| `stopMachine()` | Stop the QEMU process |
| `sync_connection(delta: float)` | Sync frame data to textures |
| `get_screen_count() -> int` | Get number of screens |
| `get_usable(index: int) -> bool` | Check if screen is available |
| `get_width(index: int) -> int` | Get screen width |
| `get_height(index: int) -> int` | Get screen height |
| `get_texture(index: int) -> ImageTexture` | Get screen texture |

## Building

### Windows

```bash
scons platform=windows target=template_debug
```

### Linux

```bash
scons platform=linux target=template_debug
```

## How It Works

1. **QEMU Connection**: Spawns QEMU with `--display sharedbuffer,id=sharedbuf_N`
2. **Shared Memory**: Connects to control block (`/QemuCtrl_sharedbuf_N`) and data buffers
3. **Threaded Reading**: Background thread reads pixel data from shared memory
4. **Format Conversion**: Converts various PIXMAN formats to RGBA8
5. **Texture Update**: Main thread syncs data to Godot textures

### Multi-Screen Support

Each virtual screen has its own:
- Shared memory mapping
- Lock (mutex) for synchronization
- Double buffering (A/B buffers)
- Independent dimensions

## Supported Pixel Formats

- `PIXMAN_a8r8g8b8` / `PIXMAN_x8r8g8b8`
- `PIXMAN_a8b8g8r8` / `PIXMAN_x8b8g8r8`
- `PIXMAN_b8g8r8a8` / `PIXMAN_b8g8r8x8`
- `PIXMAN_r8g8b8a8` / `PIXMAN_r8g8b8x8`
- `PIXMAN_rgba_float` (96bpp/128bpp)

## License

MIT
