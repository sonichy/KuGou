s="[Desktop Entry]\nName=酷狗\nComment=Music player\nExec=`pwd`/KuGou\nIcon=`pwd`/icon/KuGou.svg\nPath=`pwd`\nTerminal=false\nType=Application\nCategories=AudioVideo;"
echo $s > KuGou.desktop
cp `pwd`/KuGou.desktop ~/.local/share/applications/KuGou.desktop