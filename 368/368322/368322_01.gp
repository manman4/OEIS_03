b(n, t) = sum(k=0, n, t^k*k!*stirling(n, k, 2));
a(n, m=2, t=3) = my(u=1+1/t); u^m*b(n, t)-(1/t)*sum(j=0, m-1, u^j*(m-1-j)^n);
for(n=0, 17, print1(a(n), ", "));

\\ 実質以下と同じ
A032033(n) = polcoef(sum(m=0, n, 3^m*m!*x^m/prod(k=1, m, 1-k*x+x*O(x^n))), n);
for(n=0, 20, print1(A032033(n), ", "));
a(n) = (16/9)*A032033(n) - (1/3)*(1 + (4/3)*0^n);
for(n=0, 20, print1(a(n), ", "));