```
arm-none-eabi-gdb Gameboy.elf
target remote 192.168.1.4:2345
```

- fix hiding the buildings
- fix text hidden behind sprites

```c
// lilly
int x = -51;
int y = 134;

// sparks
int x = 83;
int y = 127;

// baxter
int x = 205;
int y = -127;

// goodrich
int x = 188;
int y = 96;

// hays
int x = -70;
int y = -128;

// detchon
int x = -183;
int y = -71;

// chapel
int x = 248;
int y = 3;

// center
int x = 69;
int y = -97;
```