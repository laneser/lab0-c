# Test performance of sort compare to linux queue sort
option fail 0
option malloc 0
new
ih dolphin 1000000
it gerbil 1000000
reverse
time sort
option linux_qsort 1
new
ih dolphin 1000000
it gerbil 1000000
reverse
time sort

