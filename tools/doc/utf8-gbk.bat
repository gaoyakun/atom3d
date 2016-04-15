@echo off
for %%f in (%1) do copy %%f %%f.utf8 > nul
for %%f in (%1) do iconv -c -f utf-8 -t gbk %%f.utf8 > %%f