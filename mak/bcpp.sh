for i
do
    echo $i
    bcpp -ylcnc $i > $i.bak
    mu copy $i.bak $i
done
