### Recommended dev environment

Debian is a good OS choice for development. As the IDE I am using CLion, with `./scripts/build.sh` set as the build program and `./scripts/run_cen64.sh` as the executable. This way CLion is will pick up locations for any build errors from the build script console output, which makes development easier.

I am running Debian in VmWare player with the project in a shared directory, this makes it easy to run any tools such as the Project64 emulator which do not run well in wine.

### Scripts

##### build.sh
Builds project, outputs .n64 file. 

- Requires wine. N64 libraries must be extracted to the wine C:/ folder, e.g. `~/.wine/drive_c/`.

http://ultra64.ca/files/software/other/sdks/n64sdk.7z
http://ultra64.ca/files/software/other/sdks/n64dev.zip

Usage: `./scripts/build.sh`

##### run_cen64.sh
Runs cen64 emulator on linux. 

- pifdata.bin must be placed in the `tools/cen64` directory.

Usage: `./scripts/run_cen64.sh`

##### import_model.sh
Generates n64 displaylist .h header file for .obj model.
- The .obj file, .mtl file, and the .bmp texture must be placed in `./models/model_name`.
- Model name and texture file names must be C compatible (no dots/spaces/dashes/etc).
- If using blender to export .obj file, export objects must be disabled and export groups must be enabled in the export settings.


Usage: `./scripts/import_model.sh model_name`

### Third-party tools

##### objn64
Converts .obj files to displaylist .h header files.

##### cen64
Cycle-accurate Nintendo 64 emulator (slow), Windows/Linux.

##### project64
Nintendo 64 emulator, Windows only.

