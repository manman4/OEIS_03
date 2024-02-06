a(n) = vecsum(apply(x->vecprod(Set(x))^3, partitions(n))); 
for(n=0, 28, print1(a(n),", ")) 