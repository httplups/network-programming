for i in 'seq 1 to 10'
do
echo "conectando cliente $i"
./client 127.0.0.1 1024
done