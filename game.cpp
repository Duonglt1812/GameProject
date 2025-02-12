#include "game.hpp"
#include<iostream>

using namespace std;

SDL_Texture* playerTex;
SDL_Rect srcR, destR;

Game::Game(){}
Game::~Game() {}


void Game::init(const char* title,int xpos,int ypos,int width,int height, bool fullscreen)
{
   int flags=0;
   if (fullscreen) {
    flags = SDL_WINDOW_FULLSCREEN;
   }
   if (SDL_Init(SDL_INIT_EVERYTHING) == 0)  { cout<<"initialized!"<<endl;
   window = SDL_CreateWindow(title,xpos,ypos,width,height,flags) ;

   if (window) { cout<<"window created!"<<endl; }
   renderer = SDL_CreateRenderer(window, -1, 0);

   if (renderer) { cout<<"renderer created!"<<endl;
   SDL_SetRenderDrawColor(renderer,255,0,255,0); }
   isRunning = true;
   }
   else { isRunning = false;}
SDL_Surface* tmpSurface = IMG_Load("C:/codeblock/letungduongSDL/Spongebob.png");
 playerTex = SDL_CreateTextureFromSurface(renderer,tmpSurface);
   SDL_FreeSurface(tmpSurface);
}
void Game::handleEvents() {
SDL_Event event;
SDL_PollEvent(&event);
switch(event.type) {
case SDL_QUIT: isRunning = false;
break;
default :
    break;

}
}
void Game::update()
{
    cnt++;
    destR.h = 100;
    destR.w = 100;
    destR.x = cnt/10;
    cout<<cnt<<endl;
}
void Game::render()
{
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,playerTex,NULL,&destR);
    SDL_RenderPresent(renderer);
}
void Game::clean()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(playerTex);
    SDL_Quit();
    cout<<"Game cleaned!"<<endl;
}

