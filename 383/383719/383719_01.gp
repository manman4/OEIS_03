\\ a(n) = Pell(n) * Pell(n-1) * Pell(n-2) * Pell(n-3) * Pell(n-4) / 3480.

pell(n) = ([2, 1; 1, 0]^n)[2, 1];
p(n, k) = prod(j=0, k-1, pell(n-j));
a(n) = p(n, 5)/p(5, 5);

print(p(5, 5));

for(n=5, 20, print1(a(n), ", "));


