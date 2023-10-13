M=71;

a(n) = sumdiv(n, d, 8^omega(d));
for(n=1, M, print1(a(n), ", "));