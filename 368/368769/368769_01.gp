\\ a(n) = (n!)^3 * Sum_{k=1..n} 1/(k!)^3.
a(n) = n!^3*sum(k=1, n, 1/k!^3);
for(n=0, 14, print1(a(n), ", "));
