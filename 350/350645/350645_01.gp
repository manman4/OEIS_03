\\ a(0) = 1; a(n) = (1/n) * Sum_{k=1..n} k * binomial(2*k,k) * binomial(2*n-k-1,n-1).
a(n) = if(n==0, 1, sum(k=1, n, k*binomial(2*k, k)*binomial(2*n-k-1, n-1))/n);

N=100;
for(n=0, N, print1(a(n),", "));

\\ D-finite with recurrence 3*n*(n-1)*a(n) - 2*(n-1)*(14*n-13)*a(n-1) + (64*n^2-192*n+140)*a(n-2) = 0.
check_recurrence(n) = {
  3*n*(n-1)*a(n) - 2*(n-1)*(14*n-13)*a(n-1) + (64*n^2-192*n+140)*a(n-2);
};
for(n=2, 15, print1(check_recurrence(n), ", "));

\\ for(n=0, N, write("b350645_1.txt", n, " ", a(n)));


