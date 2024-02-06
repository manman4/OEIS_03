a(n) = vecsum(apply(x->vecprod(Set(x))^2, partitions(n))); 
for(n=0, 31, print1(a(n),", ")) 