pids=""
for i in {1..10}
do
    # $1 --> porta
    # echo $i &
    ./cliente 127.0.0.1 $1 &
    pids="$pids $!"
done

for pid in $pids; do
    wait $pid
done
