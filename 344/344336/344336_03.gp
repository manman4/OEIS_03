M=62;

a(n) = sumdiv(n, d, 9^omega(d));
for(n=1, M, print1(a(n), ", "));