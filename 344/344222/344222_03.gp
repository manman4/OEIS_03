M=67;

a(n) = n*sumdiv(n, d, 4^omega(d)/d);
for(n=1, M, print1(a(n), ", "));