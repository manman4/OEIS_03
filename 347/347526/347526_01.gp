M=108;

a(n) = sumdiv(n, d, d^4<=n);
for(n=1, M, print1(a(n), ", "));