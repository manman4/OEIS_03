M=5;

a347607(n) = numbpart(n^n);
a347606(n) = polcoef(prod(k=1, n, 1/(1-x^k+x*O(x^(n^n-n)))), n^n-n);
a238000(n) = polcoef(prod(k=1, n, 1/(1-x^k+x*O(x^(n^n)))), n^n);
a(n) = a347607(n)+a347606(n)-a238000(n);
for(n=0, M, print1(a(n), ", "));