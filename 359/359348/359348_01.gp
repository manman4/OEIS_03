M=40;

a(n) = vecmax(Vec(prod(k=1, n, 1+x^(k*(k+1)/2))));
for(n=0, M, print1(a(n), ", "));