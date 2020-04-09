#/usr/bin/env bash

marlinmt_completion() {
  if ! [ -x "$(command -v MarlinMTShellCompletion)" ]; then
    return
  fi
  config=""
  current_id=0
  comp_marlinmt=""
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
      comp_marlinmt="$comp_marlinmt $arg"
    fi
    ((current_id++))
  done
  opts=`MarlinMTShellCompletion $comp_marlinmt`
  ret=$?
  if ! [ $ret == 0 ]; then
    return 
  fi
  opts=`echo $opts | tr '\n' ' '`  
  COMPREPLY=($(compgen -W '$opts' -- "${COMP_WORDS[${COMP_CWORD}]}"))
}

complete -o default -o nospace -F marlinmt_completion MarlinMT
