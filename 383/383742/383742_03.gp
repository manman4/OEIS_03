a002203(n) = my(w=quadgen(8)); (1+w)^n + (1-w)^n;

\\ A(0,k) = 0, A(1,k) = 1; A(n,k) = A002203(k) * A(n-1,k) - (-1)^k * A(n-2,k) for n > 1.
a(n, k) = if(n<2, n, a002203(k) * a(n-1, k) - (-1)^k * a(n-2, k));
for(n=0, 10, for(k=0, n, print1(a(k, n-k),", ")));