# Gnome Shell

In order to create custom application launcher in Gnome Shell copy 
`mindforger.desktop` file (stored in this directory) to `/usr/share/applications`, 
`~/.local/share/applications` or directly to `~/Desktop`. Don't forget
to **update** its content to reflect MindForger's installation directory:

```
[Desktop Entry]
Encoding=UTF-8
Name=Eclipse IDE
Exec=/path/to/mindforger/executable
Icon=/path/to/mindforger/icon
Type=Application
Categories=Development;
```
