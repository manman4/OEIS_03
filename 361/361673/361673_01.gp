M=26;

a(n) = n!*sum(k=0, n\5, 1/(k!^3*(2*k)!*(n-5*k)!));
for(n=0, M, print1(a(n), ", "));
