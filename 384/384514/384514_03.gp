\\ a(n) = 6^(n+1) * Sum_{k>=0} k^n * (1/7)^(k+1).
a(n) = 6^(n+1) * sum(k=0, 200, k^n * (1/7)^(k+1))+0.;
for(n=0, 15, print1(a(n),", "));
for(n=0, 15, print1(round(a(n)), ", "));

