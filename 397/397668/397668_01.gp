
a(n) = sum(k=0, n, binomial(5*k-6, k));
for(n=0, 15, print1(a(n), ", "));

check_recurrence(n) = {
  4*n*(4*n-6)*(4*n-7)*(4*n-9)*a(n) - (n-1)*(3*n-5)*(23*n-42)*(49*n-72)*a(n-1) + 5*(5*n-6)*(5*n-7)*(5*n-8)*(5*n-9)*a(n-2);
};
for(n=2, 15, print1(check_recurrence(n), ", "));


