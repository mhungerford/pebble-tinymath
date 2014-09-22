
#
# This file is the default set of rules to compile a Pebble project.
#
# Feel free to customize this to your needs.
#

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')

def build(ctx):
    ctx.load('pebble_sdk')
    ctx.env.CFLAGS.append('-Wno-strict-aliasing')

    #comment this out to test normal softfloat and compare size
    ctx.env.CFLAGS.append('-DUSE_FIXED_POINT=1')

    #force the code to only use double implementations and storage
    ctx.env.CFLAGS.append('-Dfloat=double')

    ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c'),
                    target='pebble-app.elf')

    ctx.pbl_bundle(elf='pebble-app.elf',
                   js=ctx.path.ant_glob('src/js/**/*.js'))
