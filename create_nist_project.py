#!/usr/bin/python

import sys
import os, errno, shutil
from shutil import copyfile

base_dir_name = './pqov_nist_submission'
file_dir = './nist-submission-project'

if 2 == len(sys.argv) :
  base_dir_name = sys.argv[1]
  print ( "base_dir_name: " + base_dir_name )

imple_names = [ 'ref' , 'amd64' , 'avx2' , 'neon' , 'gfni' ]

imple_dirs = {
 'ref'   : 'Reference_Implementation' ,
 'amd64' : 'Optimized_Implementation/amd64' ,
 'avx2'  : 'Optimized_Implementation/avx2' ,
 'neon'  : 'Optimized_Implementation/neon' ,
 'gfni'  : 'Optimized_Implementation/gfni' ,
}

imple_srcs = {
    'ref'   : ['src','src/ref','utils'] ,
    'amd64' : ['src','src/ref','src/amd64','utils'] ,
    'avx2'  : ['src','src/ref','src/amd64','src/ssse3','src/avx2','utils','utils/x86aesni'],
    'neon'  : ['src','src/ref','src/amd64','src/neon','utils','utils/neon_aesinst'],
    'gfni'  : ['src','src/ref','src/amd64','src/ssse3','src/avx2','src/gfni','utils','utils/x86aesni'],
}

extra_files = {
  'ref'   : [ ('README.md',f'{file_dir}/README.md.nist'), ('Makefile',f'{file_dir}/Makefile.nist') , ('generate_KAT.sh',f'{file_dir}/generate_KAT.sh.nist') ,
              ('nistkat',f'{file_dir}/PQCgenKAT_sign.c') ,
              ('nistkat','utils/nistkat')  , ('sign_api-test.c','unit_tests/sign_api-test.c')] ,
  'amd64' : [ ('README.md',f'{file_dir}/README.md.nist'), ('Makefile',f'{file_dir}/Makefile.nist') , ('generate_KAT.sh',f'{file_dir}/generate_KAT.sh.nist'),
              ('sign_api-test.c','unit_tests/sign_api-test.c') ,
              ('nistkat','utils/nistkat') , ('nistkat',f'{file_dir}/PQCgenKAT_sign.c') ] ,
  'avx2'  : [ ('README.md',f'{file_dir}/README.md.nist'), ('Makefile',f'{file_dir}/Makefile.nist.avx2') , ('generate_KAT.sh',f'{file_dir}/generate_KAT.sh.nist') ,
              ('sign_api-test.c','unit_tests/sign_api-test.c') ,
              ('nistkat','utils/nistkat') , ('nistkat',f'{file_dir}/PQCgenKAT_sign.c') ] ,
  'gfni'  : [ ('README.md',f'{file_dir}/README.md.nist'), ('Makefile',f'{file_dir}/Makefile.nist.gfni') , ('generate_KAT.sh',f'{file_dir}/generate_KAT.sh.nist') ,
              ('sign_api-test.c','unit_tests/sign_api-test.c') ,
              ('nistkat','utils/nistkat') , ('nistkat',f'{file_dir}/PQCgenKAT_sign.c') ] ,
  'neon'  : [ ('README.md',f'{file_dir}/README.md.nist'), ('Makefile',f'{file_dir}/Makefile.nist.neon') , ('generate_KAT.sh',f'{file_dir}/generate_KAT.sh.nist') ,
              ('sign_api-test.c','unit_tests/sign_api-test.c') ,
              ('nistkat','utils/nistkat') , ('nistkat',f'{file_dir}/PQCgenKAT_sign.c') ] ,
}



params_variants_names =  [ 'Ip' , 'Ip_pkc' , 'Ip_pkc_skc' , 'Is' , 'Is_pkc' , 'Is_pkc_skc' , 'III' , 'III_pkc' , 'III_pkc_skc' , 'V' , 'V_pkc' , 'V_pkc_skc' ]

modify_paramname = {
  'Ip'         : [ ] ,
  'Ip_pkc'     : [ ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC" ) ] ,
  'Ip_pkc_skc' : [ ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC_SKC" )] ,
  'Is'         : [ ('params.h' , "#define _OV256_112_44" , "#define _OV16_160_64" ) ] ,
  'Is_pkc'     : [ ('params.h' , "#define _OV256_112_44" , "#define _OV16_160_64" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC" ) ] ,
  'Is_pkc_skc' : [ ('params.h' , "#define _OV256_112_44" , "#define _OV16_160_64" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC_SKC" )] ,
  'III'        : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_184_72" ) ] ,
  'III_pkc'    : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_184_72" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC" ) ] ,
  'III_pkc_skc': [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_184_72" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC_SKC" )] ,
  'V'          : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_244_96" ) ] ,
  'V_pkc'      : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_244_96" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC" ) ] ,
  'V_pkc_skc'  : [ ('params.h' , "#define _OV256_112_44" , "#define _OV256_244_96" ), ('params.h' , "#define _OV_CLASSIC" , "#define _OV_PKC_SKC" )] ,
}


modify_projname = {
  'amd64'  : [ ( 'config.h' , "//#define _BLAS_UINT64_" , "#define _BLAS_UINT64_" ) ] ,
  'neon'   : [ ( 'config.h' , "//#define _BLAS_NEON_"   , "#define _BLAS_NEON_" ) , ('config.h' , "//#define _UTILS_NEONAES_" , "#define _UTILS_NEONAES_" )] ,
  'avx2'   : [ ( 'config.h' , "//#define _BLAS_AVX2_"   , "#define _BLAS_AVX2_" ) , ('config.h' , "//#define _UTILS_AESNI_" , "#define _UTILS_AESNI_" )    , ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ] ,
  'gfni'   : [ ( 'config.h' , "//#define _BLAS_AVX2_"   , "#define _BLAS_AVX2_\n#define _BLAS_GFNI_" ) , ('config.h' , "//#define _UTILS_AESNI_" , "#define _UTILS_AESNI_" ) ] ,
}


modify_dirname = {
  'neon/Is'         : [ ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ] ,
  'neon/Is_pkc'     : [ ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ] ,
  'neon/Is_pkc_skc' : [ ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ] ,
  'gfni/Is'         : [ ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ] ,
  'gfni/Is_pkc'     : [ ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ] ,
  'gfni/Is_pkc_skc' : [ ('config.h' , "//#define _MUL_WITH_MULTAB_" , "#define _MUL_WITH_MULTAB_" ) ] ,
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
  for pp in imple_names:
    pp1 = base_dir_name + '/' + imple_dirs[pp]
    mkdir( pp1 )
    mkdir( pp1 + '/nistkat')
    for ii in params_variants_names:
      ppii = pp1 + '/' + ii
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

def modify_files_for( proj , m_projname , m_paramname , m_dirname ):
  for pv in params_variants_names :
     dest_dir_name = '/'.join( [ base_dir_name , imple_dirs[proj] , pv ] )
     if proj in m_projname :
        for cmd in m_projname[proj] :  modify_file( dest_dir_name + '/' + cmd[0] , cmd[1] , cmd[2] )
     if pv in m_paramname :
        for cmd in m_paramname[pv] :  modify_file( dest_dir_name + '/' + cmd[0] , cmd[1] , cmd[2] )
     dirname = pv + '/' + proj
     if dirname in m_dirname :
        for cmd in m_dirname[dirname] :  modify_file( dest_dir_name + '/' + cmd[0] , cmd[1] , cmd[2] )



def copy_files_for( proj ):
  for pv in params_variants_names :
     dest_dir_name = '/'.join( [ base_dir_name , imple_dirs[proj] , pv ] )
     for jj in imple_srcs[proj] :
        copy_files( dest_dir_name , jj )
  if proj in extra_files :
    for cmd in extra_files[proj] :  copy_files( base_dir_name + '/' + imple_dirs[proj] + '/' + cmd[0] , cmd[1] )


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
     dest_dir_name = '/'.join( [ base_dir_name , imple_dirs[proj] , pv ] )
     if proj in m_projname :
        for cmd in m_projname[proj] :  modify_file( dest_dir_name + '/' + cmd[0] , cmd[1] , cmd[2] )
     if pv in m_paramname :
        for cmd in m_paramname[pv] :  modify_file( dest_dir_name + '/' + cmd[0] , cmd[1] , cmd[2] )
     dirname = proj + '/' + pv
     if dirname in m_dirname :
        for cmd in m_dirname[dirname] :  modify_file( dest_dir_name + '/' + cmd[0] , cmd[1] , cmd[2] )


for proj in imple_names :
  print( f"\n-----------------\nmodify files for project {proj}\n-----------------" )
  modify_files_for( proj , modify_projname , modify_paramname , modify_dirname )


exit()

