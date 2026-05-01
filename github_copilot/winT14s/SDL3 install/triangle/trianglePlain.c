// TrianglePlain.c --- testcode used for exam 2024

#include <SDL3/SDL.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
volatile int running = 1;
void signal_handler(int signum) { running = 0; } // Signal handler for making Ctrl+C work
void clearScreen(SDL_Surface *surface) {
  SDL_FillSurfaceRect(surface, NULL, SDL_MapRGB(SDL_GetPixelFormatDetails(surface->format), NULL, 255, 255, 255));
}
void drawRect(SDL_Surface *surface, int x, int y, int width, int height, uint32_t color) {
  SDL_Rect rect = {x, y, width, height};
  SDL_FillSurfaceRect(surface, &rect, color);
}
void setPixel(SDL_Surface *surface, int x, int y, uint32_t color) {
  SDL_Rect rect = {x, y, 1, 1};
  SDL_FillSurfaceRect(surface, &rect, color);
}

typedef struct {
  uint16_t x; 
  uint16_t y;
} Point;
typedef struct { 
  Point p[3]; // 3 points
  uint16_t color; // 4 bits unused, 4 bits green, 4 bits blue, 4 bits red // 2024-11-30: Minor weakness, should be order R-G-B
} Triangle;  // but is now kept as it is, but solutions for exam adjusted accordingly

typedef struct Node {
  Triangle* t; // Pointer to a triangle
  struct Node* next; // Pointer to the next node
} Node;

uint32_t SDLcolor(SDL_Surface *sSurf, uint16_t color) {
  uint8_t r, g, b;
  r = (color & 0b111100000000) >> 8; // The original solution had the wrong order of the color bits wrt. the comment in typedef Triangle
  g = (color & 0b11110000) >> 4;
  b = (color & 0b1111);
  r = r << 4;
  g = g << 4;
  b = b << 4; 
  return SDL_MapRGB(SDL_GetPixelFormatDetails(sSurf->format), NULL, r, g, b);
}

int fillTriangle(SDL_Surface *surface, Triangle t) {  // Code by github copilot. JCM says he has some better code for this
  printf("Fill triangle: (%d,%d) (%d,%d), (%d,%d)\n", t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y);
  int minX = SDL_min(SDL_min(t.p[0].x, t.p[1].x), t.p[2].x);
  int maxX = SDL_max(SDL_max(t.p[0].x, t.p[1].x), t.p[2].x);
  int minY = SDL_min(SDL_min(t.p[0].y, t.p[1].y), t.p[2].y);
  int maxY = SDL_max(SDL_max(t.p[0].y, t.p[1].y), t.p[2].y);
  for (int y = minY; y <= maxY; y++) {
    for (int x = minX; x <= maxX; x++) {
      // Barycentric coordinates
      int w1 = (t.p[0].x * (t.p[2].y - t.p[0].y) + (y - t.p[0].y) * (t.p[2].x - t.p[0].x) - x * (t.p[2].y - t.p[0].y));
      int w2 = (t.p[1].x * (t.p[0].y - t.p[1].y) + (y - t.p[1].y) * (t.p[0].x - t.p[1].x) - x * (t.p[0].y - t.p[1].y));
      int w3 = (t.p[2].x * (t.p[1].y - t.p[2].y) + (y - t.p[2].y) * (t.p[1].x - t.p[2].x) - x * (t.p[1].y - t.p[2].y));
      if ((w1 >= 0 && w2 >= 0 && w3 >= 0) || (w1 <= 0 && w2 <= 0 && w3 <= 0)) {
        if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
          setPixel(surface, x, y, SDLcolor(surface, t.color));
        }
      }
    }
  }
  return 0;
}

void trianglePlain(Triangle* t, uint32_t * word1, uint32_t * word2) {
  *word1 = t->p[0].x << 23;
  *word1 |= t->p[0].y << 15;
  *word1 |= t->p[1].x << 6;
  *word1 |= ((t->p[1].y & 0b11111100) >> 2);
  *word1 |= ((t->p[1].y) >> 2);
  *word2 = (t->p[1].y & 0b11) << 30;
  *word2 |= t->p[2].x << 21;
  *word2 |= t->p[2].y << 13;
  *word2 |= ((t->color & 0b111111111111))  << 1;
}

void triangleCompact(Point* newP, uint16_t color, uint8_t replaceCode, uint32_t * word) {
  *word = newP->x << 23;
  *word |= newP->y << 15;
  *word |= (color & 0b111111111111)  << 3;
  *word |= replaceCode << 1;
}

int countNodes(Node* n) { // assume at least one node
  int nodeCount = 1;
  while (n->next != NULL) {
    nodeCount++;
    n = n->next;
  }
  return nodeCount;
}

uint32_t* plainFormat(Node* n) {
  int nodeCount = countNodes(n);
//  uint32_t* plain = (uint32_t*) malloc(nodeCount * 2 * sizeof(uint32_t));
  uint32_t* plain = malloc(nodeCount * 2 * sizeof(uint32_t));
  for (int i = 0; i < nodeCount*2; i += 2) {
    trianglePlain(n->t, &plain[i], &plain[i+1]);
    n = n->next;
    if (n == NULL) { 
      plain[i+1] |= 0b1; // Set the last bit to 1 (stop bit)
    }
  }
  return plain;
}

void drawPlain(uint32_t* obj, SDL_Surface* sf) { // assumes at least one Triangle
  bool lastTriangle = false;
  while (!lastTriangle) {
    uint32_t word1 = *obj++; 
    uint32_t word2 = *obj++;    
    Triangle t = {0};
    t.p[0].x = (word1 & 0b11111111100000000000000000000000) >> 23;
    t.p[0].y = (word1 &          0b11111111000000000000000) >> 15;
    t.p[1].x = (word1 &                  0b111111111000000) >> 6;
    t.p[1].y = (word1 &                           0b111111) << 2;
    t.p[1].y |= ((word2 &           0b11000000000000000000000000000000) >> 30); // 2 bits from word2
    t.p[2].x = (word2  &              0b111111111000000000000000000000) >> 21;
    t.p[2].y = (word2  &                       0b111111110000000000000) >> 13;
    t.color = (word2 &                                 0b1111111111110) >> 1;
    fillTriangle(sf, t);
    lastTriangle = (word2 & 0b1);
  }
};

int main(int argc, char *argv[]) { 

  SDL_Window *window = NULL;
  SDL_Surface *screenSurface = NULL;
  signal(SIGINT, signal_handler);   // Set up signal handler for Ctrl+C
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }
  window = SDL_CreateWindow("TDT4258 SDL Aanimation Demo", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (window == NULL) { 
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }

  screenSurface = SDL_GetWindowSurface(window);
  printf("Simulated VGA %dx%d pixels\n", SCREEN_WIDTH, SCREEN_HEIGHT); 
  clearScreen(screenSurface);    

  const uint16_t GREEN = 0b11110000;  
  const uint16_t YELLOW = 0b111111110000;  
  const uint16_t  RED = 0b111100000000;
  const uint16_t BLUE = 0b1111;

  // This did not compile after the changes to array of points in Triangle ? .
  // Triangle t1 = {{40, 130}, { 40, 170}, {160, 170}, GREEN};
  // Triangle t2 = {{40, 130}, {160, 170}, {160,  130}, YELLOW};
  // Triangle t3 = {{40, 130}, {160, 130}, {130,  90}, RED};
  // Triangle t4 = {{40, 130}, {130,  90}, {100,  50}, BLUE};

Triangle t1, t2, t3, t4;
{
  t1.p[0].x = 40;
  t1.p[0].y = 130;
  t1.p[1].x = 40;
  t1.p[1].y = 170;
  t1.p[2].x = 160;
  t1.p[2].y = 170;
  t1.color = GREEN;
  t2.p[0].x = 40;
  t2.p[0].y = 130;
  t2.p[1].x = 160;
  t2.p[1].y = 170;
  t2.p[2].x = 160;
  t2.p[2].y = 130;
  t2.color = YELLOW;
  t3.p[0].x = 40;
  t3.p[0].y = 130;
  t3.p[1].x = 160;
  t3.p[1].y = 130;
  t3.p[2].x = 130;
  t3.p[2].y = 90;
  t3.color = RED;
  t4.p[0].x = 40;
  t4.p[0].y = 130;
  t4.p[1].x = 130;
  t4.p[1].y = 90;
  t4.p[2].x = 100;
  t4.p[2].y = 50;
  t4.color = BLUE;
}
 
  Node node4 = {&t4, NULL}; // Gives order t4 -> t3 -> t2 -> t1
  Node node3 = {&t3, &node4};
  Node node2 = {&t2, &node3};
  Node object = {&t1, &node2};
  
  uint32_t* objPlain = plainFormat(&object); 
  clearScreen(screenSurface);
  SDL_UpdateWindowSurface(window);
  printf("Drawing from plain format\n");
  drawPlain(objPlain, screenSurface); // Draws the object stored in plain format
  SDL_UpdateWindowSurface(window);

  SDL_Event e;
  while (running) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_EVENT_QUIT) {
        break;
      }
    }
  }
  SDL_DestroyWindow(window); SDL_Quit();
  return 0;
}