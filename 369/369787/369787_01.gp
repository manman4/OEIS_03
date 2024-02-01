M=51;

a(n) = #Set(Vec(prod(k=1, n, 1+x^(k*(k+1)/2))));
for(n=0, M, print1(a(n), ", "));