\\ a(n) = (n!)^4 * (H(n,4)^3 - 3*H(n,4)*H(n,8) + 2*H(n,12))/6 where H(n,r) = Sum_{k=1..n} 1/k^r.
h(n, r) = sum(k=1, n, 1/k^r);
a(n) = (n!)^4 * (h(n,4)^3 - 3*h(n,4)*h(n,8) + 2*h(n,12))/6;
for(n=0, 30, print1(a(n),", "));
