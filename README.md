
TỔNG QUAN : 

Đây là một dự án game 2D được phát triển bằng C++ với thư viện SDL2. Dự án được thiết kế để chạy trên Windows với trình biên dịch MinGW.


TRÌNH BIÊN DỊCH : MinGW


THƯ VIỆN :

	SDL2
	SDL2_image
	SDL2_mixer
 

CÔNG CỤ PHÁT TRIỂN : CodeBlocks


HƯỚNG DẪN CÀI ĐẶT TRÒ CHƠI :

	B1 : 	Tải dự án Game sử dụng GitHub Desktop hoặc lệnh: git clone https://github.com/Duonglt1812/GameProject.git
	B2 :	Copy các file đuôi "dll" vào đường dẫn "bin/Debug"
 	B3 :	Copy tệp assets vào đường dẫn "bin/Debug"
  	B4 : 	Mở file Game.exe trong "bin/Debug" để bắt đầu game


HƯỚNG DẪN CHƠI : 

	- Dùng 4 nút W(trên) - A(trái) - D(phải) - S(dưới) để điều hướng nhân vật.
 	- Dùng nút Space để nhân vật tấn công.
  	*Lưu ý : - Nhân vật khi không thể đi qua nước(water.png)
   		 - Nhân vật khi đi qua cỏ(grass.png) sẽ bị giảm 1 nửa tốc độ 

HƯỚNG PHÁT TRIỂN GAME : 

	* Đa dạng, cải tiến về map, đồ họa.
 	* Thêm các màn, phần thưởng, quái và boss.
  	* Xây dựng thêm lối chơi như : thu thập vật phẩm/sinh tồn thay vì chỉ tiêu diệt kẻ địch.
   	* Thêm skill cho nhân vật. 
    * Thêm BXH người chơi chiến thắng nhanh nhất.
      

NGUỒN : 

 	- Code có sự học hỏi, tham khảo từ kênh youtube "Let's make game" với series "How to make a Game in C++ & SDL2 from scratch : https://www.youtube.com/watch?v=QQzAHcojEKg&list=PLhfAbcv9cehhkG7ZQK0nfIGJC_C-wSLrx
  	- Texture nhân vật được sử dụng từ website Craftpix.net : https://craftpix.net/freebies/free-pixel-art-tiny-hero-sprites/?num=1&count=869&sq=32x32%20character&pos=4
	- Các texture tile map : 	- grass : https://opengameart.org/content/32x32-grass-tile
 				 	- dirt : https://opengameart.org/content/dirt-texture-pack
	      				- water : cắt ra từ https://opengameart.org/content/basic-map-32x32-by-ivan-voirol
   	- Sound : 
    			- Theme_sound : https://pixabay.com/sound-effects/soundtracksong-66467
    		  	- attack_sound : https://pixabay.com/sound-effects/big-punch-short-with-male-moan-83735
				- winning_sound : https://pixabay.com/sound-effects/goodresult-82807
				- losing_sound : https://pixabay.com/sound-effects/losing-horn-313723

