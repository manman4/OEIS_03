\\ T(n,k) = [x^n] x^k * Sum_{i=0..k-1} 1 / Product_{j=1..i} (1 - x^j).
T(n,k) = polcoeff(x^k * sum(i=0, k-1, 1 / prod(j=1, i, 1 - x^j + x*O(x^n))), n);
for(n=1, 20, for(k=1, n, print1(T(n,k),", ")));