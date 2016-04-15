@echo off
pushd .\tools\doc
call build_doc.bat ..\..\doc atom3d ..\..\include
popd