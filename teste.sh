pids=""
for i in {1..10}
do
    ./cliente 192.168.0.10 1024 &
    pids="$pids $!"
done

for pid in $pids; do
    wait $pid
done
