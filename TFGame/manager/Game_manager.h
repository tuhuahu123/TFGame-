#pragma once

#include"manager.h"
#include <SDL_video.h>
#include <SDL_log.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>


class GameManager : public Manager<GameManager>
{
	friend class Manager<GameManager>; // 友元类
public:
	
	int run(int argc,char** argv)
	{
		Uint64 last_counter = SDL_GetPerformanceCounter(); //获取高精度计时器
		const Uint64 counter_freq = SDL_GetPerformanceFrequency(); //获取高精度计时器频率


		while (!is_quit)
		{
			while (SDL_PollEvent(&event))
			{
				Uint64 current_counter = SDL_GetPerformanceCounter();					//获取当前时间
				//decltype(current_counter) delta = current_counter - last_counter;		//时间差
				double delta = (double)(current_counter - last_counter) / counter_freq; //计算帧间隔时间
				last_counter = current_counter;											//更新时间
				if (delta*1000<1000.0/60)  
				{
					SDL_Delay((Uint32)(1000.0 / 60 - delta * 1000));  //延迟
					//SDL_Delay((Uint32)1000.0 / 60 - delta * 1000);  
				}

				on_input();							              // 输入					
				on_update(0.0);                                  // 更新
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 设置渲染器颜色)
				SDL_RenderClear(renderer);					   // 清空渲染器
				on_render();				                  // 渲染
				SDL_RenderPresent(renderer);                 // 显示渲染器
			}
		}
		return 0;
	}

protected:
	GameManager() 
	{
		init_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0, "SDL初始化失败"); // 初始化SDL
		init_assert(IMG_Init(IMG_INIT_PNG) == IMG_INIT_PNG, "SDL_image 初始化失败"); // 初始化SDL_image
		init_assert(Mix_Init(MIX_INIT_MP3), u8"SDL_mixer 初始化失败!");
		init_assert(!TTF_Init(), u8"SDL_ttf 初始化失败!");

		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048); // 初始化音频 44100Hz, 2声道, 2048字节缓冲区 
		SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");   // 设置输入法显示 

		window = SDL_CreateShapedWindow(u8"TUTU的塔防", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN); // 创建窗口	 参数：标题，位置，大小，显示
		init_assert(window, "窗口创建失败"); // 窗口创建失败

		// 创建自定义按钮
		SDL_MessageBoxButtonData buttons[] = {
			{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, u8"确定" },  // 默认按钮
		};

		// 创建自定义消息框
		SDL_MessageBoxData messageboxdata = {
			SDL_MESSAGEBOX_INFORMATION,  // 类型
			window,                      // 父窗口
			u8"开发者信息",              // 标题
			u8"开发作者QQ：1030131545\n\n点击确定进入游戏", // 内容
			SDL_arraysize(buttons),      // 按钮数量
			buttons,                     // 按钮数组
			NULL                         // 颜色方案（使用默认）
		};

		int buttonid;  // 按钮ID
		SDL_ShowMessageBox(&messageboxdata, &buttonid);  // 显示消息框

		//创建渲染器启用硬件加速 且 启用垂直同步 且 启用渲染到纹理功能
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE); 	    
		init_assert(renderer, "渲染器创建失败"); // 渲染器创建失败


	}

	~GameManager()
	{
		SDL_DestroyRenderer(renderer); // 销毁渲染器
		SDL_DestroyWindow(window);     // 销毁窗口
		TTF_Quit();                    // 退出TTF
		Mix_Quit();                    // 退出MIX
		IMG_Quit();                    // 退出IMG
		SDL_Quit();                    // 退出SDL

	}

private:
	SDL_Event event;      
	bool is_quit = false;  


	SDL_Window* window = nullptr;      // 窗口
	SDL_Renderer* renderer = nullptr; //渲染器
	

	// 初始化断言
	void init_assert(bool flag, const char* err_msg)
	{
		if (flag) return;
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"游戏启动失败", err_msg,window);  // 初始化失败
	}

	
	void on_input()  
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				is_quit = true;
			}
		}
	}

	void on_update(double delta)
	{

	}

	void on_render()
	{

	}



};
