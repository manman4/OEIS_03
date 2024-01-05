\\ a(n) = (n!)^3 * Sum_{k=0..n} k^2/(k!)^3.
a(n) = n!^3*sum(k=0, n, k^2/k!^3);
for(n=0, 14, print1(a(n), ", "));
