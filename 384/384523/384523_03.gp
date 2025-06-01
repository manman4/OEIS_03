\\ a(n) = 3^(n+1)/7 * Sum_{k>=0} k^n * (4/7)^k.
a(n) = 3^(n+1)/7 * sum(k=0, 300, k^n * (4/7)^k)+0.;
for(n=0, 15, print1(a(n),", "));
for(n=0, 15, print1(round(a(n)), ", "));

