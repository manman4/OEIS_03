\\ a(n) = (n+1)^2 * (n!)^3 * Sum_{k=0..n} 1/((k+1)^2 * (k!)^3).
a(n) = (n+1)^2*n!^3*sum(k=0, n, 1/((k+1)^2*k!^3));
for(n=0, 13, print1(a(n), ", "));
