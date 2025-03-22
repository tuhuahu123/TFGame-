#define SDL_MAIN_HANDLED
#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h> 
#include<SDL2_gfxPrimitives.h> //基本图形绘制

#include <fstream> // Add this include to fix the incomplete type error

#include <sstream>
#include <cJSON.h>



void teset_json()  //测试json文件是否打开成功
{
   

	std::ifstream file("test.json"); //打开json文件
   if (!file.good())
   {
       std::cout << "文件打开失败" << std::endl; 
       return;
   }


   std::stringstream str_stream;  //创建字符串流
   str_stream << file.rdbuf();   //读取文件内容
   file.close();                //关闭文件

   cJSON* json_root = cJSON_Parse(str_stream.str().c_str());  //解析json文件 

   cJSON* json_name = cJSON_GetObjectItem(json_root, "name");  //获取name字段
   cJSON* json_age = cJSON_GetObjectItem(json_root, "age");    //获取age字段
   cJSON* json_pets = cJSON_GetObjectItem(json_root, "pets");    //获取pets字段

   std::cout << "name:" << json_name->valuestring << std::endl;  //输出name字段
   std::cout << "age:" << json_age->valueint << std::endl;        //输出age字段

   std::cout << "pets:" << std::endl;                             //输出pets字段
   cJSON* Json_item = nullptr; //创建json对象
   cJSON_ArrayForEach(Json_item, json_pets)  //遍历pets字段
   {
	   std::cout << Json_item->valuestring << std::endl;  //输出pets字段
   }
}

void test_csv()
{
	std::ifstream file("test.csv"); //打开csv文件

    if (!file.good())
    {
		std::cout << "文件打开失败" << std::endl;
		return;
    }
	std::string str_line;  //创建字符串
    while (std::getline(file, str_line))  //逐行读取文件
    {
		std::string str_grid;  //创建字符串
		std::stringstream str_stream(str_line);  //创建字符串流 

        while (std::getline(str_stream, str_grid, ','))  //逐个读取逗号分隔的数据
        {
            std::cout << str_grid << " ";  //输出数据
        }
		std::cout << std::endl;  //换行
    }
	file.close();  //关闭文件
}

int main()
{
    teset_json();  //测试json文件是否打开成功
	std::cout << "-------------------" << std::endl;
    test_csv();  //测试csv文件是否打开成功

    /*--- SDL初始化 ---*/
    SDL_Init(SDL_INIT_EVERYTHING);     // 启动全部基础功能（视频/声音/手柄等）
    IMG_Init(IMG_INIT_PNG);            // 添加PNG图片支持
    Mix_Init(MIX_INIT_OGG);            // 添加OGG音乐支持
    TTF_Init();                        // 添加文字显示功能

    /*--- 声音设置 ---*/
    Mix_OpenAudio(4100,                // 非标准声音采样率（正常用44100）
        MIX_DEFAULT_FORMAT,   // 默认声音格式
        2,                    // 立体声
        2048);                // 声音缓冲区大小

    /*--- 创建游戏窗口 ---*/
    SDL_Window* window = SDL_CreateWindow(
        u8"你好世界",                 // 窗口标题(支持中文)
        SDL_WINDOWPOS_CENTERED,       // 自动居中
        SDL_WINDOWPOS_CENTERED,       // 自动居中
        1200,                         // 窗口宽
        720,                          // 窗口高
        SDL_WINDOW_SHOWN              // 立即显示窗口
    );

    
    SDL_Renderer* renderer = SDL_CreateRenderer(   // 创建渲染器
        window,                                    // 要显示的窗口
        -1,                                        // 自动选择显卡
        SDL_RENDERER_ACCELERATED                   // 使用显卡加速
    );

    // 渲染图片//

    SDL_Surface* suf_imamge = IMG_Load("assets/image.png");                   // 加载图片
    SDL_Texture* text_imge = SDL_CreateTextureFromSurface(renderer, suf_imamge); // 将图片转换为纹理

    SDL_Event event;  // 事件对象
    SDL_Point pos_cursor = { 0,0 }; // 鼠标位置
    SDL_Rect rect_image; // 图片位置,字体位置

    rect_image.w = suf_imamge->w;  // 图片宽高
    rect_image.h = suf_imamge->h;

    //字体渲染//
    TTF_Font* font = TTF_OpenFont("assets/ipix.ttf", 32);  // 加载字体
    SDL_Color color = {0, 0, 0, 0};         // 字体颜色
    SDL_Surface* text_surface = TTF_RenderUTF8_Blended(font, u8"你好世界", color);       // 渲染字体
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface); // 将字体转换为纹理
    SDL_Rect rect_texture; // 字体位置

    rect_texture.w = text_surface->w; // 字体宽高
    rect_texture.h = text_surface->h;

    //加载音乐//
	Mix_Music* music = Mix_LoadMUS("assets/BG_BGM.mp3"); // 加载音乐
    Mix_FadeInMusic(music, -1, 1000);  //淡入音乐参数：音乐文件，播放次数，淡入时间
 
	
    int fps = 60;           // 设置帧率
    Uint64 last_counter = SDL_GetPerformanceCounter();  // 获取当前时间
	Uint64 counter_freq = SDL_GetPerformanceFrequency(); // 获取当前时间计数

    bool is_quit = false;   // 标记是否退出游戏

    while (!is_quit) // 游戏主循环
    {
        while (SDL_PollEvent(&event))     // 持续提取事件（类似从信箱取信件）
        {
            if (event.type == SDL_QUIT)// 检测到窗口关闭信号（点击X按钮）
            {
                is_quit = true;// 标记需要退出程序
            } 
            else if (event.type == SDL_MOUSEMOTION)  // 检测鼠标移动
            {
                pos_cursor.x = event.motion.x;  // 记录X坐标
                pos_cursor.y = event.motion.y;  // 记录Y坐标
                //std::cout << "鼠标位置：" << pos_cursor.x << " " << pos_cursor.y << std::endl;
            }

        }

		// 计算帧率
		Uint64 counter = SDL_GetPerformanceCounter();   // 返回当前的高精度计数器值
		//std::cout << "当前时间：" << counter << std::endl;

		double delta = (double)(counter - last_counter) / counter_freq;   //计算帧间隔时间
		last_counter = counter;                                          // 更新上一次的时间                     
		if (delta * 1000 < 1000.0 / 60)            // 帧率限制   1000.0 / 60 表示每帧理想的时间间隔(毫秒)，这里目标是60FPS
        {
			SDL_Delay((Uint32)((1000.0 / 60) - (delta * 1000)));   //通过SDL_Delay()进行适当延时，等待到达目标帧时间
			//打印帧率
			//std::cout << "帧率：" << 1.0 / delta << std::endl;
        }

         // 设置图片位置
        rect_image.x = pos_cursor.x;
        rect_image.y = pos_cursor.y;

        rect_texture.x = pos_cursor.x;
        rect_texture.y = pos_cursor.y;



      

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // 设置渲染器颜色
        SDL_RenderClear(renderer);  // 清除渲染器
        
        
		SDL_RenderCopy(renderer, text_imge, nullptr, &rect_image);         // 渲染图片
		SDL_RenderCopy(renderer, text_texture, nullptr, &rect_texture);   // 渲染字体
      
		filledCircleRGBA(renderer, pos_cursor.x, pos_cursor.y, 50, 255, 0, 0, 125); //绘制圆形
        
        SDL_RenderPresent(renderer);   // 刷新屏幕显示
            
    }
}