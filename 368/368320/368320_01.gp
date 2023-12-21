b(n, t) = sum(k=0, n, t^k*k!*stirling(n, k, 2));
a(n, m=3, t=2) = my(u=1+1/t); u^m*b(n, t)-(1/t)*sum(j=0, m-1, u^j*(m-1-j)^n);
for(n=0, 18, print1(a(n), ", "));

\\ 実質以下と同じ
A(n) = polcoef(sum(m=0, n, 2^m*m!*x^m/prod(k=1, m, 1-k*x+x*O(x^n))), n);
A004123(n) = A(n-1);
\\ offsetに注意
for(n=1, 20, print1(A004123(n), ", "));
a(n) = (27/8)*A004123(n+1) - (1/2)*(2^n + 3/2 + (9/4)*0^n);
for(n=0, 20, print1(a(n), ", "));