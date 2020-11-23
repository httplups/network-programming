for i in {1..100}
do
	# $1 --> porta
	./cliente 127.0.0.1 $1 &
done