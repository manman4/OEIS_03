M=10;

a(n) = sumdiv(n, d, d!^n);
for(n=1, M, print1(a(n), ", "));