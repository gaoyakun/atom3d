@echo off
call utf8-gbk.bat %1
fr %1 -f:charset=UTF-8 -t:charset=gb2312