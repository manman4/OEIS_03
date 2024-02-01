M=50;

a(n) = #Set(Vec(prod(k=1, n, 1+x^k^2)));
for(n=0, M, print1(a(n), ", "));