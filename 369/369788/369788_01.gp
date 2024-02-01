M=54;

a(n) = #Set(Vec(prod(k=1, n, 1+x^prime(k))));
for(n=0, M, print1(a(n), ", "));