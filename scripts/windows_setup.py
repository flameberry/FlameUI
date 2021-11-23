import os
import pathlib
import subprocess
from shutil import rmtree
from Dependencies import flame_ui_project_dir
from Dependencies import flame_ui_project_name

os.chdir(flame_ui_project_dir)

print('[FLAMEUI]: Running CMake...')
output = subprocess.run(
    [
        'cmake',
        '-DMSDFGEN_INSTALL=Off',
        '-DMSDFGEN_BUILD_STANDALONE=Off',
        '-Wno-dev',
        '-S.',
        '-Bbuild/visual_studio',
        '-G"Visual Studio 16 2019"'
    ]
)

if output.returncode != 0:
    print('[FLAMEUI]: CMake process failed.')
    build_path = pathlib.Path(flame_ui_project_dir / 'build/visual_studio')
    if build_path.exists():
        print('[FLAMEUI]: Cleaning up some files...')
        rmtree(build_path)
        print(f'[FLAMEUI]: Removed directory \"{build_path.resolve()}\"')

    print('[FLAMEUI]: Finished process.')
    exit()

vs_soln_files = list(pathlib.Path(flame_ui_project_dir / 'build/visual_studio').glob('**/*.sln'))

if not vs_soln_files:
    print('[FLAMEUI]: Failed to generate build files, cleaning up...')
    rmtree(flame_ui_project_dir / 'build/visual_studio')
    print(f'[FLAMEUI]: Removed directory \"{(flame_ui_project_dir / "build/visual_studio").resolve()}\"')
    exit()

flame_ui_sln_file_path = ''

for sln_file in vs_soln_files:
    if sln_file.name == f'{flame_ui_project_name}.sln':
        flame_ui_sln_file_path = sln_file.resolve()

print(f'[FLAMEUI]: The Visual Studio Solution file is generated in \"{flame_ui_sln_file_path}\"')
print('[FLAMEUI]: Do you want to open the Visual Studio Solution file?(Y/n)')
should_open_sln_file = input('[FLAMEUI]: ')

if should_open_sln_file.lower() == 'y':
    print('[FLAMEUI]: Opening Visual Studio Solution...')
    result = subprocess.run(['open', f'{flame_ui_sln_file_path}'])
    if result != 0:
        print('[FLAMEUI]: Failed to open Visual Studio Solution file!')
        print(f'[FLAMEUI]: You might want to check the "{(flame_ui_project_dir / "build/visual_studio").resolve()}" directory, for the solution file.')
        print('[FLAMEUI]: Finished process.')
        exit()
    print('[FLAMEUI]: Done.')
elif should_open_sln_file.lower() == 'n':
    print('[FLAMEUI]: Finished process.')
else:
    print('[FLAMEUI]: Unrecognized input. Stopping process.')
    print(f'[FLAMEUI]: Note: If you want to open the visual studio solution yourself, open the explorer and double click on the solution file which can be found at \"{flame_ui_sln_file_path}\"')
