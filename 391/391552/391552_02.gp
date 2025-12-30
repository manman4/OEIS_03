M=139;
\\ T(n, k) = Sum_{j=0..n} binomial(3, j)*Stirling2(n, k+j)*(k + j)!.
T(n, k) = sum(j=0, n, binomial(3, j)*stirling(n, k+j, 2)*(k+j)!);
cnt = 0;
for(n=0, M, for(k=0, n, write("/Users/xxx/Desktop/b391552_1.txt", cnt, " ", T(n, k)); cnt++));

