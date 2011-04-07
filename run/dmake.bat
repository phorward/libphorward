@echo off
echo ********************
echo * DEBUG MAKE START *
echo ********************
nmake DEBUG=/D__WITH_TRACE %*
echo *******************
echo * DEBUG MAKE DONE *
echo *******************
