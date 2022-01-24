ipcs -m | grep lx > file
echo 'total num: '
ipcs -m | grep lx | wc -l
awk '{print $2}' file > out
awk '{print "ipcrm -m "$0}' out > cmd
echo 'command write to file.'
source cmd
echo 'command file execute completed.'
rm file out cmd
echo 'clear shared memory success.'
