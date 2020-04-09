
# marlin-add-dll function.
# Adds a single or multiple libraries to the MARLIN_DLL variable
# Usage: marlin-add-dll [options] lib1 [lib2 ...]
unset -f marlin-add-dll
function marlin-add-dll() { 

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
    echo "Usage: marlin-add-dll [options] lib1 [lib2 ...]"
    echo " Options"
    echo "     -d  --dump             Call MarlinDumpPlugins before exiting"
    return 1
  fi

  # First parse the input arguments
  in_full_libs=${in_full_libs[*]}
  in_libs=`basename -a $in_full_libs | tr '\n' ' '`
  dupl_in_libs=`echo $in_libs | tr ' ' '\n' | sort | uniq -Du`

  # Check for duplicated libraries in input
  if [ "p$dupl_in_libs" != "p" ]
  then
    echo "[ERROR] marlin-add-dll: check your inputs, you have duplicated library names:"
    dupl=`echo $dupl_in_libs | sort -u | sed 's/^/---> /'`
    echo $dupl
    return 1
  fi

  # If MARLIN_DLL is not set, just export and exit
  if [ "p$MARLIN_DLL" == "p" ]
  then
    export MARLIN_DLL=`echo $in_full_libs | sed 's/ /:/g'`
    if [ $dump -eq 1 ]; then
      MarlinDumpPlugins
    fi
    return 0
  fi
  
  # Merge the input libraries and MARLIN_DLL
  # then check for duplicates again
  all_full_libs="`sed 's/:/ /g' <<< "$MARLIN_DLL"` $in_full_libs"
  all_libs=`basename -a $all_full_libs | tr '\n' ' '`
  all_dupl_libs=`echo $all_libs | tr ' ' '\n' | sort -b | uniq -Du`
  
  # Check for duplicated libraries
  if [ "p$all_dupl_libs" != "p" ]
  then
    echo "[ERROR] marlin-add-dll: check your inputs, found duplicates in MARLIN_DLL:"
    dupl=`echo $all_dupl_libs | sort -u | sed 's/^/---> /'`
    echo $dupl
    return 1
  fi
  
  # Add libraries to MARLIN_DLL. Use readlink to get the full path to libraries
  export MARLIN_DLL=$MARLIN_DLL:`readlink -f $in_full_libs | tr '\n' ' ' | tr -d '[:space:]' | tr ' ' ':'`
  if [ $dump -eq 1 ]; then
    MarlinDumpPlugins
  fi
  echo "MARLIN_DLL=$MARLIN_DLL"
  return 0
}



# marlin-rm-dll function.
# Remove one or multiple libraries from the MARLIN_DLL variable
unset -f marlin-rm-dll
function marlin-rm-dll() { 

  if (( $# != 1 ))
  then
    echo "Usage: marlin-rm-dll pattern"
    return 1
  fi
  
  # If MARLIN_DLL is not set, just exit
  if [ "p$MARLIN_DLL" == "p" ]
  then
    echo "MARLIN_DLL="
    return 0
  fi

  pattern=$1
  rm_dlls=`echo $MARLIN_DLL | tr ':' '\n' | grep -E "$pattern"`
  keep_dlls=`echo $MARLIN_DLL | tr ':' '\n' | grep -vE "$pattern"`
  
  if [ "p$rm_dlls" != "p" ]
  then
    export MARLIN_DLL=`echo $keep_dlls | tr ' ' ':'`
    echo "MARLIN_DLL=$MARLIN_DLL"
    return 0
  fi
  
  echo "[WARNING] No library matching the pattern $pattern. MARLIN_DLL unchanged"
  return -1
}
