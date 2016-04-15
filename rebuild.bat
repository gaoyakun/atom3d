call "%VS120COMNTOOLS%\..\..\VC\vcvarsall.bat"
call atom_project_gen_vc12.bat
devenv build_atom_vc12\atom3d.sln /build Debug /project INSTALL
devenv build_atom_vc12\atom3d.sln /build RelWithDebInfo /project INSTALL
