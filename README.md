# A Vietnamese Input Method

A Vietnamese input method for X-Windows using XIM. To process Vietnamese characters, it uses [viEngine](src/viEngine/README.md). 

## Requirements

gnome-common are required to compile.

```bash
sudo apt-get install gnome-common
``` 

## Installation

1. Build

    ```bash
    ./autogen.sh
    make
    sudo make install    
    ```

2. Config

    Add the following lines to your ${HOME}/.xprofile or ${HOME}/.xinputrc
    ```bash
    export GTK_IM_MODULE=xim
    export XMODIFIERS=@im=vnim
    export QT_IM_MODULE=xim
    vnim &
    ```

## Usage

- vnim --vni: To start VNIM using VNI input
- vnim --verbose: To enable log
- Uses Ctrl + Shift or Alt + Z to switch between English & Vietnamese modes
 
## TODO

### v0.1.0
- [x] Support VNI
- [x] Allow to press w->u*
- [x] Allow to change settings while running
- [ ] ~~Auto commit text when the editing word is long enough~~

### v0.2.0
- [ ] Application indicator at tray bar
- [ ] Support Full Telex

## Author

- Nguyen Van Bong - vanbong@gmail.com

## License

Coryright (C) 2016 by Nguyen Van Bong. See LICENSE
