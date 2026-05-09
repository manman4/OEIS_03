\\ a(n) = (n!)^3 * (H(n,3)^2 - H(n,6))/2 where H(n,r) = Sum_{k=1..n} 1/k^r.
h(n, r) = sum(k=1, n, 1/k^r);
a(n) = (n!)^3 * (h(n,3)^2 - h(n,6))/2;
for(n=0, 30, print1(a(n),", "));
