# -*- mode: python ; coding: utf-8 -*-

import os
from pathlib import Path

# Include all files in Modules/Templates/ and Images/
template_files = [(str(p), os.path.join('Modules', 'Templates')) for p in Path(os.path.join('Modules', 'Templates')).glob('*')]
image_files    = [(str(p), os.path.join('Images')) for p in Path(os.path.join('Images')).glob('*')]

datas = template_files + image_files

a = Analysis(
    ['AtamsAutogen.py'],
    pathex=['.'],  # Ensures relative imports work
    binaries=[],
    datas=datas,
    hiddenimports=[
        # Add any hidden imports here, e.g.:
        # 'customtkinter',
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='AtamsAutogen',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,
    # icon='Images/icon.icns',  # Uncomment and set if you have an icon
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    app=True, 
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='AtamsAutogen',
)
