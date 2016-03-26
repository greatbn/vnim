# A Vietnamese Input Method

A Vietnamese input method using for X-Windows using XIM. 

## Requirements

autoconf, automake, gnome-common are required to compile. 

## Installation

```bash
./autogen.sh
make
sudo make install    
```

## Usage

Add the following lines to your ${HOME}/.profile or ${HOME}/.bashrc
```bash
export GTK_IM_MODULE=xim
export XMODIFIERS=@im=vnim
export QT_IM_MODULE=xim
vnim-xim &
```

## TODO

- [ ] Support VNI
- [ ] Application indicator at tray bar
- [x] Allow to press w->u*
- [ ] Allow to change settings while running
- [ ] Auto commit text when the editing word is olng enough

## Author

- Bong Nguyen - vanbong@gmail.com

## License

Coryright (C) 2016 by Bong Nguyen. See LICENSE
