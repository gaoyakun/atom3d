pushd build_atom_vc9

for /R %%f in (.) do if exist %%f\%1\ATOM_dbghlp.pdb copy %%f\%1\ATOM_dbghlp.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_archive.pdb copy %%f\%1\ATOM_archive.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_audio.pdb copy %%f\%1\ATOM_audio.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_commondlg.pdb copy %%f\%1\ATOM_commondlg.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_devicedb.pdb copy %%f\%1\ATOM_devicedb.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_engine.pdb copy %%f\%1\ATOM_engine.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_geometry.pdb copy %%f\%1\ATOM_geometry.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_image.pdb copy %%f\%1\ATOM_image.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_kernel.pdb copy %%f\%1\ATOM_kernel.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_render.pdb copy %%f\%1\ATOM_render.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_script.pdb copy %%f\%1\ATOM_script.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_studio.pdb copy %%f\%1\ATOM_studio.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_thread.pdb copy %%f\%1\ATOM_thread.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_ttfont.pdb copy %%f\%1\ATOM_ttfont.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_vfs.pdb copy %%f\%1\ATOM_vfs.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOMX.pdb copy %%f\%1\ATOMX.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_maxexporter.pdb copy %%f\%1\ATOM_maxexporter.pdb ..\sym\%1
for /R %%f in (.) do if exist %%f\%1\ATOM_mayaexporter2.pdb copy %%f\%1\ATOM_mayaexporter2.pdb ..\sym\%1

popd