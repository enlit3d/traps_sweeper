#One of the more useful ones is the copy_directory that is commonly used in post-build step to copy configuration and support files (configs, DLLs, бн) to the binary directory so program can be directly executed in a IDE or debugger.
#Unfortunately this command always overwrites the contents of the destination directory so all changes in the target folder are lost, this is a problem when developer wants to keep local configuration changes during development.
#https://bravenewmethod.com/2017/06/18/update_directory-command-for-cmake/

file(GLOB_RECURSE _file_list RELATIVE "${src_dir}" "${src_dir}/*")

foreach( each_file ${_file_list} )
  set(destinationfile "${dst_dir}/${each_file}")
  set(sourcefile "${src_dir}/${each_file}")
  if(NOT EXISTS ${destinationfile} OR ${sourcefile} IS_NEWER_THAN ${destinationfile})
    get_filename_component(destinationdir ${destinationfile} DIRECTORY)
    file(COPY ${sourcefile} DESTINATION ${destinationdir})
  endif()
endforeach(each_file)