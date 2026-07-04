
a(n) = sum(k=0, n, binomial(4*k-5, k));
for(n=0, 15, print1(a(n), ", "));

check_recurrence(n) = {
  3*n*(3*n-5)*(3*n-7)*a(n) - (n-1)*(283*n^2-977*n+840)*a(n-1) + 4*(4*n-5)*(4*n-6)*(4*n-7)*a(n-2);
};
for(n=2, 15, print1(check_recurrence(n), ", "));
