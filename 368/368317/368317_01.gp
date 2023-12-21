b(n, t) = sum(k=0, n, t^k*k!*stirling(n, k, 2));
a(n, m=4, t=1) = my(u=1+1/t); u^m*b(n, t)-(1/t)*sum(j=0, m-1, u^j*(m-1-j)^n);
for(n=0, 19, print1(a(n), ", "));

\\ 実質以下と同じ
A000670(n) = polcoef(sum(m=0, n, m!*x^m/prod(k=1, m, 1-k*x+x*O(x^n))), n);
for(n=0, 20, print1(A000670(n), ", "));
a(n) = 16*A000670(n) - (3^n + 2*2^n + 4 + 8*0^n);
for(n=0, 20, print1(a(n), ", "));