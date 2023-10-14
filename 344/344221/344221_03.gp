M=66;

a(n) = n*sumdiv(n, d, 3^omega(d)/d);
for(n=1, M, print1(a(n), ", "));