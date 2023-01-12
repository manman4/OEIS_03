M=33;

a(n) = sumdiv(n, d, (2*d)^(n/d))/2;
for(n=1, M, print1(a(n), ", "));