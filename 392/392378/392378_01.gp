T(n,m) = my(A=1+x); for(i=1, n, A = 1+x + subst(A, x, x*A +x*O(x^n)) - A); polcoef(A^m, n);   

\\ A(n,0) = 0^n.
\\ A(0,1) = A(1,1) = 1;   A(n,1) =         Sum_{j=1..n-1} A(j,1) * A(n-j,j).
\\ For k > 1, A(0,k) = 1; A(n,k) = (1/n) * Sum_{j=1..n} (j*(k+1)-n) * A(j,1) * A(n-j,k).
a(n, k, m=1) = if(n*k<=1, k^n, if(k==1, m*sum(j=1, n-1, a(j, 1)*a(n-j, j)), sum(j=1, n, ((k+1)*j-n)*a(j, 1)*a(n-j, k))/n));
for(n=0, 10, for(k=0, n, print1(a(k,n-k), ", ")));
for(n=0, 10, for(k=0, n, print1(a(k,n-k) - T(k,n-k), ", ")));
