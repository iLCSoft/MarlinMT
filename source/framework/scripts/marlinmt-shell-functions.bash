
# marlinmt-add-dll function.
# Adds a single or multiple libraries to the MARLINMT_DLL variable
# Usage: marlinmt-add-dll [options] lib1 [lib2 ...]
unset -f marlinmt-add-dll
function marlinmt-add-dll() { 

  in_full_libs=()
  dump=0
  while [[ $# -gt 0 ]]
  do
  key="$1"

  case $key in
      -d|--dump)
      dump=1
      shift # past argument
      ;;
      *)    # unknown option
      in_full_libs+=("$1") # save it in an array for later
      shift # past argument
      ;;
  esac
  done

  if (( ${#in_full_libs[*]} < 1 ))
  then
    echo "Usage: marlinmt-add-dll [options] lib1 [lib2 ...]"
    echo " Options"
    echo "     -d  --dump             Call MarlinMTDumpPlugins before exiting"
    return 1
  fi

  # First parse the input arguments
  in_full_libs=${in_full_libs[*]}
  in_libs=`basename -a $in_full_libs | tr '\n' ' '`
  dupl_in_libs=`echo $in_libs | tr ' ' '\n' | sort | uniq -Du`

  # Check for duplicated libraries in input
  if [ "p$dupl_in_libs" != "p" ]
  then
    echo "[ERROR] marlinmt-add-dll: check your inputs, you have duplicated library names:"
    dupl=`echo $dupl_in_libs | sort -u | sed 's/^/---> /'`
    echo $dupl
    return 1
  fi

  # If MARLINMT_DLL is not set, just export and exit
  if [ "p$MARLINMT_DLL" == "p" ]
  then
    export MARLINMT_DLL=`echo $in_full_libs | sed 's/ /:/g'`
    if [ $dump -eq 1 ]; then
      MarlinMTDumpPlugins
    fi
    return 0
  fi
  
  # Merge the input libraries and MARLINMT_DLL
  # then check for duplicates again
  all_full_libs="`sed 's/:/ /g' <<< "$MARLINMT_DLL"` $in_full_libs"
  all_libs=`basename -a $all_full_libs | tr '\n' ' '`
  all_dupl_libs=`echo $all_libs | tr ' ' '\n' | sort -b | uniq -Du`
  
  # Check for duplicated libraries
  if [ "p$all_dupl_libs" != "p" ]
  then
    echo "[ERROR] marlinmt-add-dll: check your inputs, found duplicates in MARLINMT_DLL:"
    dupl=`echo $all_dupl_libs | sort -u | sed 's/^/---> /'`
    echo $dupl
    return 1
  fi
  
  # Add libraries to MARLINMT_DLL. Use readlink to get the full path to libraries
  export MARLINMT_DLL=$MARLINMT_DLL:`readlink -f $in_full_libs | tr '\n' ' ' | tr -d '[:space:]' | tr ' ' ':'`
  if [ $dump -eq 1 ]; then
    MarlinMTDumpPlugins
  fi
  echo "MARLINMT_DLL=$MARLINMT_DLL"
  return 0
}



# marlinmt-rm-dll function.
# Remove one or multiple libraries from the MARLINMT_DLL variable
unset -f marlinmt-rm-dll
function marlinmt-rm-dll() { 

  if (( $# != 1 ))
  then
    echo "Usage: marlinmt-rm-dll pattern"
    return 1
  fi
  
  # If MARLINMT_DLL is not set, just exit
  if [ "p$MARLINMT_DLL" == "p" ]
  then
    echo "MARLINMT_DLL="
    return 0
  fi

  pattern=$1
  rm_dlls=`echo $MARLINMT_DLL | tr ':' '\n' | grep -E "$pattern"`
  keep_dlls=`echo $MARLINMT_DLL | tr ':' '\n' | grep -vE "$pattern"`
  
  if [ "p$rm_dlls" != "p" ]
  then
    export MARLINMT_DLL=`echo $keep_dlls | tr ' ' ':'`
    echo "MARLINMT_DLL=$MARLINMT_DLL"
    return 0
  fi
  
  echo "[WARNING] No library matching the pattern $pattern. MARLINMT_DLL unchanged"
  return -1
}
