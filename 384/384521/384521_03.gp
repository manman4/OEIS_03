\\ a(n) = 5^(n+1)/7 * Sum_{k>=0} k^n * (2/7)^k.
a(n) = 5^(n+1)/7 * sum(k=0, 300, k^n * (2/7)^k)+0.;
for(n=0, 15, print1(a(n),", "));
for(n=0, 15, print1(round(a(n)), ", "));

