default(parisize, 1200000000)
M=1000;

seq(n) = my(A=x +x*O(x^(n+1)), V=[1]); for(i=1, n, V=Vec(A); A = x + sum(k=1, n-1, V[k]*x^k/(1-x^k +x*O(x^n)) )^3 ); A;
v = seq(M);
for(n=1, M, write("/Users/xxx/Desktop/b382318_01.txt", n, " ", polcoef(v, n)));