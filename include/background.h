#ifndef BACKGROUND
#define BACKGROUND

typedef struct{
	int x_pos;
	int y_pos;
} Map;

extern Map *map; // global pointer which give the map position
void text_init();
void update_text_display();
void map_init();
void mini_map_init();
void loading_screen_init();
void loading_screen_display();
void window_init();

#endif /*BACKGROUND*/