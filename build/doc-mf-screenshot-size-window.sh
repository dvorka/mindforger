#!/bin/bash
# Set window size for creation of screenshots for web mf.com: 1360x768 (then use Screenshot app)

echo "IMPORTANT: before running this script make sure window is NOT maximized or half sized - use rectangle button that allows window resizing (using right-down corner)"

wmctrl -r "MindForger - Thinking Notebook - 0.6.0 - Technology Preview" -e 0,0,0,1360,768

# eof
