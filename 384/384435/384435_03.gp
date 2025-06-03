\\ a(n) = 2^(n+1)/5 * Sum_{k>=0} k^n * (3/5)^k.
a(n) = 2^(n+1)/5 * sum(k=0, 200, k^n * (3/5)^k)+0.;
for(n=0, 15, print1(a(n),", "));
for(n=0, 15, print1(round(a(n)), ", "));

