if [ -d %PEGASUS_REPOSITORY_DIR"/root#PG_Internal" ]
then
  #
  # Save the current repository to prev_repository
  #
  if [[ -d %PEGASUS_REPOSITORY_DIR ]]
  then
    if [[ -d %PEGASUS_PREV_REPOSITORY_DIR ]]
    then
      mv %PEGASUS_PREV_REPOSITORY_DIR %PEGASUS_PREV_REPOSITORY_DIR`date '+%Y-%m-%d-%s.%N'`.rpmsave;
    fi
    mv %PEGASUS_REPOSITORY_DIR %PEGASUS_PREV_REPOSITORY_DIR
    mkdir %PEGASUS_REPOSITORY_DIR
  fi
fi
