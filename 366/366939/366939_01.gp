a(n) = sum(k=1, n, (-1)^(k-1) * binomial(k+3,4) * (n\k) );
for(n=1, 42, print1(a(n), ", "));
