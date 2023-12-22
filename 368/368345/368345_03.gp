\\ a(n) = 1/3 * Sum_{k=0..n} floor(4^k/21)
a(n) = 1/3 * sum(k=0, n, 4^k\21);
for(n=0, 27, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..n} floor(4^k/63).
a(n) = sum(k=0, n, 4^k\63);
for(n=0, 20, print1(a(n), ", "));
 