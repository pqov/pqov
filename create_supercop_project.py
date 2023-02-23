#!/usr/bin/python

import sys
import os, errno, shutil
from shutil import copyfile

base_dir_name = './crypto_sign'

if 2 == len(sys.argv) :
  base_dir_name = sys.argv[1]
  print ( "base_dir_name: " + base_dir_name )

file_dir = './supercop-project'

imple_names = [ 'ref' , 'amd64' , 'avx2' , 'neon' ]

imple_dirs = {
 'ref'   : 'ref' ,
 'amd64' : 'amd64' ,
 'avx2'  : 'avx2' ,
 'neon'  : 'neon'
}

imple_srcs = {
    'ref'   : ['src','src/ref','utils'] ,
    'amd64' : ['src','src/ref','src/amd64','utils'] ,
    'avx2'  : ['src','src/ref','src/amd64','src/ssse3','src/avx2','utils','utils/x86aesni'],
    'neon'  : ['src','src/ref','src/amd64','src/neon','utils','utils/neon_aesinst']
}

extra_files = {
  'ref'   : [  ] ,
  'amd64' : [ ('architectures',f'{file_dir}/architectures.x86') ] ,
  'avx2'  : [ ('architectures',f'{file_dir}/architectures.x86') ] ,
  'neon'  : [ ('architectures',f'{file_dir}/architectures.arm') ]
}



params_variants_names =  [ 'OVIp' , 'OVIppkc' , 'OVIppkcskc' , 'OVIs' , 'OVIspkc' , 'OVIspkc_skc' , 'OVIII' , 'OVIIIpkc' , 'OVIIIpkcskc' , 'OVV' , 'OVVpkc' , 'OVVpkcskc' ]

modify_paramname = {
  'OVIp'       : [  ] ,
  'OVIppkc'    : [ ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC" ) ] ,
  'OVIppkcskc' : [ ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC_SKC" )] ,
  'OVIs'       : [ ('params.h' , "#define _OV256_112_44" , "#define _OV16_160_64" ) ] ,
  'OVIspkc'    : [ ('params.h' , "#define _OV256_112_44" , "#define _OV16_160_64" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC" ) ] ,
  'OVIspkcskc' : [ ('params.h' , "#define _OV256_112_44" , "#define _OV16_160_64" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC_SKC" )] ,
  'OVIII'      : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_184_72" ) ] ,
  'OVIIIpkc'   : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_184_72" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC" ) ] ,
  'OVIIIpkcskc': [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_184_72" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC_SKC" )] ,
  'OVV'        : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_244_96" ) ] ,
  'OVVpkc'     : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_244_96" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC" ) ] ,
  'OVVpkcskc'  : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_244_96" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC_SKC" )] 
}


modify_projname = {
  'ref'    : [ ('api.h' , '//#define _SUPERCOP_' , '#define _SUPERCOP_' ),('config.h' , '#define _UTILS_OPENSSL_' , '#define _UTILS_SUPERCOP_' ) ] ,
  'amd64'  : [ ('api.h' , '//#define _SUPERCOP_' , '#define _SUPERCOP_' ),('config.h' , '#define _UTILS_OPENSSL_' , '#define _UTILS_SUPERCOP_' ),( 'config.h' , "//#define _BLAS_UINT64_" , "#define _BLAS_UINT64_" ) ] ,
  'neon'   : [ ('api.h' , '//#define _SUPERCOP_' , '#define _SUPERCOP_' ),('config.h' , '#define _UTILS_OPENSSL_' , '#define _UTILS_SUPERCOP_' ),( 'config.h' , "//#define _BLAS_NEON_"   , "#define _BLAS_NEON_" ) , ('config.h' , "//#define _UTILS_NEONAES_" , "#define _UTILS_NEONAES_" )] ,
  'avx2'   : [ ('api.h' , '//#define _SUPERCOP_' , '#define _SUPERCOP_' ),('config.h' , '#define _UTILS_OPENSSL_' , '#define _UTILS_SUPERCOP_' ),( 'config.h' , "//#define _BLAS_AVX2_"   , "#define _BLAS_AVX2_" ) , ('config.h' , "//#define _UTILS_AESNI_" , "#define _UTILS_AESNI_" )    , ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ]
}

modify_dirname = {
  'OVIs/neon'       : [ ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ] ,
  'OVIspkc/neon'    : [ ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ] ,
  'OVIspkcskc/neon' : [ ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ]
}


#
# mk dirs
#

def mkdir( dir_name ):
    try:
        os.makedirs(dir_name)
        print( 'mkdir: ' + dir_name  )
    except OSError as e:
        print( e )
        pass
        #if e.errno != errno.EEXIST:
            #raise


def make_all_dirs():
  print( "create all dirs:" )
  mkdir( base_dir_name )
  for ii in params_variants_names:
    pp1 = base_dir_name + '/' + ii
    mkdir( pp1 )
    for pp in imple_names:
      ppii = pp1 + '/' + imple_dirs[pp]
      mkdir( ppii )


print( "\n----------------- mk dirs -----------------" )
make_all_dirs()

#
# copy files
#

def copy_files( dest_dir , src_dir ):
   #print( 'processing: %s -> %s' % (src_dir , dest_dir ) )
   if os.path.isfile( src_dir ) :
      shutil.copy2( src_dir, dest_dir)
      print('copy files: %s\t<--\t%s' % (dest_dir, src_dir) )
   elif os.path.isdir( src_dir ) :
      src_files = os.listdir(src_dir)
      for file_name in src_files:
         full_file_name = os.path.join(src_dir, file_name)
         if (os.path.isfile(full_file_name)):
            shutil.copy2(full_file_name, dest_dir)
            print('copy files: %s\t<--\t%s/%s' % ( dest_dir , src_dir, file_name ) )
   else :
      print( "(???) don't know how to process: %s <- %s (???)" % (dest_dir,src_dir) )



def copy_files_for( proj ):
  for pv in params_variants_names :
     dest_dir_name = '/'.join( [ base_dir_name , pv , imple_dirs[proj] ] )
     for jj in imple_srcs[proj] :
        copy_files( dest_dir_name , jj )
     if proj in extra_files :
        for cmd in extra_files[proj] :  copy_files( dest_dir_name + '/' + cmd[0] , cmd[1] )


for proj in imple_names :
  print( f"\n-----------------\ncopy files for project {proj}\n-----------------" )
  copy_files_for( proj )

#
#  modify files
#

def modify_file( file_name , phrase , new_content ):
  try:
    content = list( open( file_name ) )
  except :
    raise
  line_idx = -1
  for i in range( len(content) ) :
    if content[i].startswith( phrase ) :
      line_idx = i
      break
  if line_idx < 0 :
     print( "no line in %s starts with %s" % (file_name,phrase) )
     return line_idx
  print( "chage %s:%d -> %s" % ( file_name , line_idx , new_content ) )
  try:
    fp = open( file_name , "w" )
  except :
    return
  for i in range(len(content)):
    if line_idx == i :
      fp.write( new_content + '\n' )
    else:
      fp.write( content[i] )
  fp.close()



def modify_files_for( proj , m_projname , m_paramname , m_dirname ):
  for pv in params_variants_names :
     dest_dir_name = '/'.join( [ base_dir_name , pv , imple_dirs[proj] ] )
     if proj in m_projname :
        for cmd in m_projname[proj] :  modify_file( dest_dir_name + '/' + cmd[0] , cmd[1] , cmd[2] )
     if pv in m_paramname :
        for cmd in m_paramname[pv] :  modify_file( dest_dir_name + '/' + cmd[0] , cmd[1] , cmd[2] )
     dirname = pv + '/' + proj
     if dirname in m_dirname :
        for cmd in m_dirname[dirname] :  modify_file( dest_dir_name + '/' + cmd[0] , cmd[1] , cmd[2] )


for proj in imple_names :
  print( f"\n-----------------\nmodify files for project {proj}\n-----------------" )
  modify_files_for( proj , modify_projname , modify_paramname , modify_dirname )


exit()

