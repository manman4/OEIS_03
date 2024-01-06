\\ a(n) = A008544(n) * Sum_{k=0..n} 1/A008544(k).

a008544(n) = prod(k=1, n, 3*k-1);
for(n=0, 18, print1(a008544(n), ", "))
a(n) = a008544(n)*sum(k=0, n, 1/a008544(k));
for(n=0, 18, print1(a(n), ", "))