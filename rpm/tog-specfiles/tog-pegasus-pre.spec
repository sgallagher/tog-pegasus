if [ -d %PEGASUS_REPOSITORY_DIR"/root#PG_Internal" ]
then
  #
  # Save the current repository to prev_repository
  #
  if [[ -d %PEGASUS_REPOSITORY_DIR ]]
  then
    if [[ -d %PEGASUS_PREV_REPOSITORY_DIR ]]
    then
      rm -rf %PEGASUS_PREV_REPOSITORY_DIR
    fi
    mv %PEGASUS_REPOSITORY_DIR %PEGASUS_PREV_REPOSITORY_DIR
    mkdir %PEGASUS_REPOSITORY_DIR
  fi
fi
