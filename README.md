```
arm-none-eabi-gdb Gameboy.elf
target remote 192.168.1.4:2345
```

if squirrels go too far off the screen they will loop around to the other side. Might want to put a check for this later