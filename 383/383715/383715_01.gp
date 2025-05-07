pell(n) = ([2, 1; 1, 0]^n)[2, 1];
p(n, k) = prod(j=0, k-1, pell(n-j));
a099927(n, k) = p(n, k)/p(k, k);
T(n, k) = (-1)^((k+1)\2)*a099927(n, k);

for(n=0, 9, for(k=0, n, print1(T(n, k), ", ")));
