M=12;

a128854(n) = numbpart(n^3);
a304176(n) = polcoef(prod(k=1, n, 1/(1-x^k+x*O(x^(n^3-n)))), n^3-n);
a238608(n) = polcoef(prod(k=1, n, 1/(1-x^k+x*O(x^(n^3)))), n^3);
a(n) = a128854(n)+a304176(n)-a238608(n);
for(n=0, M, print1(a(n), ", "));