\\ a(n) = (n!)^6 * (H(n,2)^3 + 3*H(n,2)*H(n,4) + 2*H(n,6))/6 where H(n,r) = Sum_{k=1..n} 1/k^r.
h(n, r) = sum(k=1, n, 1/k^r);
a(n) = (n!)^6 * (h(n, 2)^3 + 3*h(n, 2)*h(n, 4) + 2*h(n, 6))/6;
for(n=0, 25, print1(a(n), ", "));

