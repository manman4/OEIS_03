\\ G.f. of column k: q^k * Sum_{i=0..k-1} (k-i) * q^i / Product_{j=1..i} (1 - q^j).
T(n,k) = polcoeff(x^k * sum(i=0, k-1, (k-i) * x^i / prod(j=1, i, 1 - x^j + x*O(x^n))), n);
cnt=1;
for(n=1, 140, for(k=1, n, write("b394828_1.txt", cnt, " ", T(n, k)); cnt++));