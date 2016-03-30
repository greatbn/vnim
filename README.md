# A Vietnamese Input Method

A Vietnamese input method for X-Windows using XIM. 

## Requirements

autoconf, automake, gnome-common, x11-dev are required to compile. 

## Installation

1. Build

    ```bash
    ./autogen.sh
    make
    sudo make install    
    ```

2. Config

    Add the following lines to your ${HOME}/.xprofile or ${HOME}/.bashrc
    ```bash
    export GTK_IM_MODULE=xim
    export XMODIFIERS=@im=vnim
    export QT_IM_MODULE=xim
    vnim-xim &
    ```

## Usage

- Use vnim-xim --vni: To start VNIM using VNI input method
- Uses Ctrl + Shift or Alt + Z to switch between English & Vietnamese modes
 
## Todo

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
