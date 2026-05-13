\\ T(n,k) = [x^n] x^k * Sum_{i=0..k-1} 1 / Product_{j=1..i} (1 - x^j).
T(n,k) = polcoeff(x^k * sum(i=0, k-1, 1 / prod(j=1, i, 1 - x^j + x*O(x^n))), n);
cnt=1;
for(n=1, 70, for(k=1, n, write("b394828_1.txt", cnt, " ", T(n, k)); cnt++));