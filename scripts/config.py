import curses
import json
import os
import subprocess
from pathlib import Path

CONFIG_FILE = "build_config.json"

class Configurator:
    def __init__(self):
        self.config = {
            "compiler": "gcc",
            "optimization": "O0",
            "features": {
                "debug": True
            }
        }

    def load_config(self):
        if Path(CONFIG_FILE).exists():
            with open(CONFIG_FILE) as f:
                self.config.update(json.load(f))

    def save_config(self):
        with open(CONFIG_FILE, 'w') as f:
            json.dump(self.config, f, indent=4)

    def generate_makefile(self):
        with open("common.mk", 'w') as f:
            f.write(f"""# Generated configuration
CC := {self.config['compiler']}
LD := {'ld.lld' if 'clang' in self.config['compiler'] else 'ld'}
AR := {'ld.lld' if 'llvm-ar' in self.config['compiler'] else 'ar'}
CFLAGS := -m64 -g -{self.config['optimization']} \\
          -nostdlib -ffreestanding -Wall -Wextra \\
          -fno-builtin -fno-PIC -fno-stack-protector \\
          -mcmodel=kernel

{'CFLAGS += -target x86_64-pc-none-elf -march=x86-64' if self.config['compiler'] == 'clang' else ''}
""")

def draw_menu(stdscr):
    configurator = Configurator()
    configurator.load_config()
    
    curses.curs_set(0)
    current_row = 0
    menu_items = [
        "Compiler",
        "Optimization",
        "Features",
        "Generate Makefile",
        "Exit"
    ]
    features_menu = [
        "Debug"
    ]

    while True:
        stdscr.clear()
        h, w = stdscr.getmaxyx()

        stdscr.addstr(0, w//2 - len("HolyOS Build Configurator")//2, "HolyOS Build Configurator")
        
        for idx, item in enumerate(menu_items):
            x = w//2 - len(item)//2
            y = h//2 - len(menu_items)//2 + idx
            if idx == current_row:
                stdscr.attron(curses.A_REVERSE)
                stdscr.addstr(y, x, item)
                stdscr.attroff(curses.A_REVERSE)
            else:
                stdscr.addstr(y, x, item)
        
        status = f"Current: {configurator.config['compiler']} -{configurator.config['optimization']}"
        stdscr.addstr(h-1, 0, status)
        
        key = stdscr.getch()
        
        if key == curses.KEY_UP and current_row > 0:
            current_row -= 1
        elif key == curses.KEY_DOWN and current_row < len(menu_items)-1:
            current_row += 1
        elif key == curses.KEY_ENTER or key in [10, 13]:
            if menu_items[current_row] == "Compiler":
                configurator.config['compiler'] = "gcc" if configurator.config['compiler'] == "clang" else "clang"
            elif menu_items[current_row] == "Optimization":
                opts = ["O0", "O1", "O2", "O3", "Os"]
                current = opts.index(configurator.config['optimization'])
                configurator.config['optimization'] = opts[(current + 1) % len(opts)]
            elif menu_items[current_row] == "Features":
                feature_menu(stdscr, configurator)
            elif menu_items[current_row] == "Generate Makefile":
                subprocess.run(["make", "clean"])
                configurator.generate_makefile()
                configurator.save_config() 
                return
            elif menu_items[current_row] == "Exit":
                return

def feature_menu(stdscr, configurator):
    current_row = 0
    features = list(configurator.config['features'].keys())
    
    while True:
        stdscr.clear()
        h, w = stdscr.getmaxyx()
        
        stdscr.addstr(0, w//2 - len("Feature Toggle")//2, "Feature Toggle")
        
        for idx, feature in enumerate(features):
            x = w//2 - 10
            y = h//2 - len(features)//2 + idx
            status = "[X]" if configurator.config['features'][feature] else "[ ]"
            if idx == current_row:
                stdscr.attron(curses.A_REVERSE)
                stdscr.addstr(y, x, f"{status} {feature}")
                stdscr.attroff(curses.A_REVERSE)
            else:
                stdscr.addstr(y, x, f"{status} {feature}")
        
        key = stdscr.getch()
        
        if key == curses.KEY_UP and current_row > 0:
            current_row -= 1
        elif key == curses.KEY_DOWN and current_row < len(features)-1:
            current_row += 1
        elif key == curses.KEY_ENTER or key in [10, 13]:
            configurator.config['features'][features[current_row]] = not configurator.config['features'][features[current_row]]
        elif key == 27:
            return

if __name__ == "__main__":
    curses.wrapper(draw_menu)
    print("Configuration saved.")
