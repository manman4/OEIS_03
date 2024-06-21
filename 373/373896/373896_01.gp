default(parisize, 12000000000);

a008457(n) = sumdiv(n, d, (-1)^(n-d)*d^3);
a341397(n) = 1+16*sum(k=1, n, a008457(k));
a(n) = a341397(10^n);

for(n=0, 9, print1(a(n), ", "))

