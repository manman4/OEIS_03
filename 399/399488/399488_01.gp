a(n) = matpermanent(matrix(n, n, i, j, lcm(i, j)>=n));
for(n=0, 18, print1(a(n),", "));

