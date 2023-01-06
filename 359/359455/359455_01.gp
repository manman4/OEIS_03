M=100;

a(n) = sumdiv(n, d, (n-d)%d^3==0);
for(n=1, M, print1(a(n), ", "));