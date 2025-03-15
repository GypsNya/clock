# clock

use stm32F429igt chip, 7 inch lcd screen and esp8266, make it display time.

device as server open 80 port, listen clien send time to sync.

but because esp8266-01 is not quite stable, so maybe you need restart device to build tcp server.

also you can choose a picture and put it into sd card to display on screen as background.

this project use `Tiny JPEG Decompressor`, `fatfs` and `lvgv`, thanks to them auther.

fatfs:
http://elm-chan.org/fsw/ff/00index_e.html

Tiny JPEG Decompressor:
http://elm-chan.org/fsw/tjpgd/00index.html

lvgv:
https://lvgl.io/
