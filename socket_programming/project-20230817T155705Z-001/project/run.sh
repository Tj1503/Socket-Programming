make
n=$1
p=$2

mkdir -p output
for ((i=1 ; i<=$n ; i++))
do
	timeout 60 ./client-phase$p ./client$i-config.txt ./files/client$i/ > ./output/op-c$i-p$p.txt &
done
