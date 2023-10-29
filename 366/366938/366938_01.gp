a(n) = sum(k=1, n, (-1)^(k-1) * binomial(k+2,3) * (n\k) );
for(n=1, 46, print1(a(n), ", "));
