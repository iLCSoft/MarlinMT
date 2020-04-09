#/usr/bin/env bash

marlin_completion() {
  if ! [ -x "$(command -v MarlinShellCompletion)" ]; then
    return
  fi
  config=""
  current_id=0
  comp_marlin=""
  # Loop over current command lien arguments
  # 1) Skip the executable name argument
  # 2) Skip the last argument which is the one we want to complete
  for arg in $COMP_LINE; do
    # echo "Current id is $current_id"
    if [ $current_id == 0 ]; then
      # echo "Skipping arg no $current_id"
      ((current_id++))
      continue
    fi
    if [ "$arg" != "${COMP_WORDS[${COMP_CWORD}]}" ]; then
      comp_marlin="$comp_marlin $arg"
    fi
    ((current_id++))
  done
  opts=`MarlinShellCompletion $comp_marlin`
  ret=$?
  if ! [ $ret == 0 ]; then
    return 
  fi
  opts=`echo $opts | tr '\n' ' '`  
  COMPREPLY=($(compgen -W '$opts' -- "${COMP_WORDS[${COMP_CWORD}]}"))
}

complete -o default -o nospace -F marlin_completion MarlinMT
