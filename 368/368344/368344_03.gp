\\ a(n) = 1/2 * Sum_{k=0..n} floor(3^k/13)
a(n) = 1/2 * sum(k=0, n, 3^k\13);
for(n=0, 20, print1(a(n), ", "));

\\ a(n) = = Sum_{k=0..n} floor(3^k/26)
a(n) = sum(k=0, n, 3^k\26);
for(n=0, 20, print1(a(n), ", "));
 