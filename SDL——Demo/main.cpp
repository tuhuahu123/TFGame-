#define SDL_MAIN_HANDLED
#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_mixer.h>
#include<SDL_ttf.h> 
#include<SDL2_gfxPrimitives.h> //����ͼ�λ���

#include <fstream> // Add this include to fix the incomplete type error

#include <sstream>
#include <cJSON.h>



void teset_json()  //����json�ļ��Ƿ�򿪳ɹ�
{
   

	std::ifstream file("test.json"); //��json�ļ�
   if (!file.good())
   {
       std::cout << "�ļ���ʧ��" << std::endl; 
       return;
   }


   std::stringstream str_stream;  //�����ַ�����
   str_stream << file.rdbuf();   //��ȡ�ļ�����
   file.close();                //�ر��ļ�

   cJSON* json_root = cJSON_Parse(str_stream.str().c_str());  //����json�ļ� 

   cJSON* json_name = cJSON_GetObjectItem(json_root, "name");  //��ȡname�ֶ�
   cJSON* json_age = cJSON_GetObjectItem(json_root, "age");    //��ȡage�ֶ�
   cJSON* json_pets = cJSON_GetObjectItem(json_root, "pets");    //��ȡpets�ֶ�

   std::cout << "name:" << json_name->valuestring << std::endl;  //���name�ֶ�
   std::cout << "age:" << json_age->valueint << std::endl;        //���age�ֶ�

   std::cout << "pets:" << std::endl;                             //���pets�ֶ�
   cJSON* Json_item = nullptr; //����json����
   cJSON_ArrayForEach(Json_item, json_pets)  //����pets�ֶ�
   {
	   std::cout << Json_item->valuestring << std::endl;  //���pets�ֶ�
   }
}

void test_csv()
{
	std::ifstream file("test.csv"); //��csv�ļ�

    if (!file.good())
    {
		std::cout << "�ļ���ʧ��" << std::endl;
		return;
    }
	std::string str_line;  //�����ַ���
    while (std::getline(file, str_line))  //���ж�ȡ�ļ�
    {
		std::string str_grid;  //�����ַ���
		std::stringstream str_stream(str_line);  //�����ַ����� 

        while (std::getline(str_stream, str_grid, ','))  //�����ȡ���ŷָ�������
        {
            std::cout << str_grid << " ";  //�������
        }
		std::cout << std::endl;  //����
    }
	file.close();  //�ر��ļ�
}

int main()
{
    teset_json();  //����json�ļ��Ƿ�򿪳ɹ�
	std::cout << "-------------------" << std::endl;
    test_csv();  //����csv�ļ��Ƿ�򿪳ɹ�

    /*--- SDL��ʼ�� ---*/
    SDL_Init(SDL_INIT_EVERYTHING);     // ����ȫ���������ܣ���Ƶ/����/�ֱ��ȣ�
    IMG_Init(IMG_INIT_PNG);            // ���PNGͼƬ֧��
    Mix_Init(MIX_INIT_OGG);            // ���OGG����֧��
    TTF_Init();                        // ���������ʾ����

    /*--- �������� ---*/
    Mix_OpenAudio(4100,                // �Ǳ�׼���������ʣ�������44100��
        MIX_DEFAULT_FORMAT,   // Ĭ��������ʽ
        2,                    // ������
        2048);                // ������������С

    /*--- ������Ϸ���� ---*/
    SDL_Window* window = SDL_CreateWindow(
        u8"�������",                 // ���ڱ���(֧������)
        SDL_WINDOWPOS_CENTERED,       // �Զ�����
        SDL_WINDOWPOS_CENTERED,       // �Զ�����
        1200,                         // ���ڿ�
        720,                          // ���ڸ�
        SDL_WINDOW_SHOWN              // ������ʾ����
    );

    
    SDL_Renderer* renderer = SDL_CreateRenderer(   // ������Ⱦ��
        window,                                    // Ҫ��ʾ�Ĵ���
        -1,                                        // �Զ�ѡ���Կ�
        SDL_RENDERER_ACCELERATED                   // ʹ���Կ�����
    );

    // ��ȾͼƬ//

    SDL_Surface* suf_imamge = IMG_Load("assets/image.png");                   // ����ͼƬ
    SDL_Texture* text_imge = SDL_CreateTextureFromSurface(renderer, suf_imamge); // ��ͼƬת��Ϊ����

    SDL_Event event;  // �¼�����
    SDL_Point pos_cursor = { 0,0 }; // ���λ��
    SDL_Rect rect_image; // ͼƬλ��,����λ��

    rect_image.w = suf_imamge->w;  // ͼƬ���
    rect_image.h = suf_imamge->h;

    //������Ⱦ//
    TTF_Font* font = TTF_OpenFont("assets/ipix.ttf", 32);  // ��������
    SDL_Color color = {0, 0, 0, 0};         // ������ɫ
    SDL_Surface* text_surface = TTF_RenderUTF8_Blended(font, u8"�������", color);       // ��Ⱦ����
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface); // ������ת��Ϊ����
    SDL_Rect rect_texture; // ����λ��

    rect_texture.w = text_surface->w; // ������
    rect_texture.h = text_surface->h;

    //��������//
	Mix_Music* music = Mix_LoadMUS("assets/BG_BGM.mp3"); // ��������
    Mix_FadeInMusic(music, -1, 1000);  //�������ֲ����������ļ������Ŵ���������ʱ��
 
	
    int fps = 60;           // ����֡��
    Uint64 last_counter = SDL_GetPerformanceCounter();  // ��ȡ��ǰʱ��
	Uint64 counter_freq = SDL_GetPerformanceFrequency(); // ��ȡ��ǰʱ�����

    bool is_quit = false;   // ����Ƿ��˳���Ϸ

    while (!is_quit) // ��Ϸ��ѭ��
    {
        while (SDL_PollEvent(&event))     // ������ȡ�¼������ƴ�����ȡ�ż���
        {
            if (event.type == SDL_QUIT)// ��⵽���ڹر��źţ����X��ť��
            {
                is_quit = true;// �����Ҫ�˳�����
            } 
            else if (event.type == SDL_MOUSEMOTION)  // �������ƶ�
            {
                pos_cursor.x = event.motion.x;  // ��¼X����
                pos_cursor.y = event.motion.y;  // ��¼Y����
                //std::cout << "���λ�ã�" << pos_cursor.x << " " << pos_cursor.y << std::endl;
            }

        }

		// ����֡��
		Uint64 counter = SDL_GetPerformanceCounter();   // ���ص�ǰ�ĸ߾��ȼ�����ֵ
		//std::cout << "��ǰʱ�䣺" << counter << std::endl;

		double delta = (double)(counter - last_counter) / counter_freq;   //����֡���ʱ��
		last_counter = counter;                                          // ������һ�ε�ʱ��                     
		if (delta * 1000 < 1000.0 / 60)            // ֡������   1000.0 / 60 ��ʾÿ֡�����ʱ����(����)������Ŀ����60FPS
        {
			SDL_Delay((Uint32)((1000.0 / 60) - (delta * 1000)));   //ͨ��SDL_Delay()�����ʵ���ʱ���ȴ�����Ŀ��֡ʱ��
			//��ӡ֡��
			//std::cout << "֡�ʣ�" << 1.0 / delta << std::endl;
        }

         // ����ͼƬλ��
        rect_image.x = pos_cursor.x;
        rect_image.y = pos_cursor.y;

        rect_texture.x = pos_cursor.x;
        rect_texture.y = pos_cursor.y;



      

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // ������Ⱦ����ɫ
        SDL_RenderClear(renderer);  // �����Ⱦ��
        
        
		SDL_RenderCopy(renderer, text_imge, nullptr, &rect_image);         // ��ȾͼƬ
		SDL_RenderCopy(renderer, text_texture, nullptr, &rect_texture);   // ��Ⱦ����
      
		filledCircleRGBA(renderer, pos_cursor.x, pos_cursor.y, 50, 255, 0, 0, 125); //����Բ��
        
        SDL_RenderPresent(renderer);   // ˢ����Ļ��ʾ
            
    }
}