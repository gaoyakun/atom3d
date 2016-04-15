rem usage: build_doc.bat chm_dir chm_name input_dir
@echo off

if exist %1\%2.chm del /f /q %1\%2.chm

echo 创建项目文档...
copy /Y doxy_template.cfg doxy_template_%2.cfg > nul
echo OUTPUT_DIRECTORY = %1 >> doxy_template_%2.cfg
echo PROJECT_NAME = %2 >> doxy_template_%2.cfg
echo INPUT = %3 >> doxy_template_%2.cfg
echo CHM_FILE = %2.chm >> doxy_template_%2.cfg
doxygen.exe doxy_template_%2.cfg
del /f /s /q doxy_template_%2.cfg > nul
if not exist %1\html\index.hhp goto doxy_error

echo 转换html文件编码到GBK...
call utf8-gbk.bat %1\html\index.hhp
if exist %1\html\index.hhc call utf8-gbk.bat %1\html\index.hhc
if exist %1\html\index.hhk call utf8-gbk.bat %1\html\index.hhk
call html_utf8-gbk.bat %1\html\*.html

echo 创建chm文件...
regsvr32 /s hhctrl.ocx
hhc %1\html\index.hhp > nul
if not exist %1\html\%2.chm goto hhc_error
copy /Y %1\html\%2.chm %1 > nul

echo 删除临时文件...
del /S /F /Q %1\html > nul
rd /s /q %1\html > nul
echo 生成完毕，chm已经保存在目录%1中.
goto quit

:hhc_error
echo 生成CHM文件失败！
goto quit

:doxy_error
echo 创建项目文档失败！

:quit
set errorlevel=0