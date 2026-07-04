
a(n) = sum(k=0, n, binomial(3*k-4, k));
for(n=0, 15, print1(a(n), ", "));

check_recurrence(n) = {
  2*n*(2*n-5)*a(n) - (n-1)*(31*n-60)*a(n-1) + 3*(3*n-4)*(3*n-5)*a(n-2);
};
for(n=2, 15, print1(check_recurrence(n), ", "));


