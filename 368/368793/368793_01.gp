\\ a(n) = (3*n-2)!!! * Sum_{k=0..n} k/(3*k-2)!!!.

a007559(n) = prod(k=1, n, 3*k-2);
for(n=0, 20, print1(a007559(n), ", "))
a(n) = a007559(n)*sum(k=0, n, k/a007559(k));
for(n=0, 18, print1(a(n), ", "))